/* dltool/dltool.c
 *
 * SMDK24XX USB Download Tool
 *
 * (c) 2004,2005 Ben Dooks
*/

#include <sys/types.h>
#include <sys/stat.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <getopt.h>
#include <fcntl.h>


#include <usb.h>

unsigned int   debug   = 0;
unsigned long  dl_addr = 0xfffffffeL;
unsigned long  dl_size = 0L;
unsigned char *dl_data = NULL;

char *dl_file = "download.dat";
char *dl_udev = NULL;
char *dl_ubus = NULL;

#define DBG(x) if (debug) { printf x; }

void write_u32(unsigned char *dp, unsigned long val)
{
	dp[0] = val;
	dp[1] = val>>8;
	dp[2] = val>>16;
	dp[3] = val>>24;
}

void write_header(unsigned char *data, unsigned long addr, unsigned long size)
{
	write_u32(data+0x00, addr);
	write_u32(data+0x04, size);
}

unsigned char *load_file(const char *fname, unsigned long *size,
			 unsigned long *fsize)
{
	struct stat st;
	unsigned char *blk;
	int fd;

	if (stat(fname, &st) < 0) {
		perror("fstat");
		goto err;
	}

	*fsize = st.st_size;
	*size = st.st_size + 10;

	blk = malloc(*size);
	if (blk == NULL) {
		perror("malloc");
		goto err;
	}
	memset(blk, 0, *size);

	fd = open(fname, O_RDONLY);
	if (fd < 0) {
		perror("open");
		goto err_free;
	}

	if (read(fd, blk+8, st.st_size) < st.st_size) {
		perror("read");
		goto err_free;
	}

	close(fd);
	return blk;

 err_free:
	free(blk);
 err:
	return NULL;
       
}

void calc_cksum(unsigned char *data, ssize_t len)
{
	unsigned int cksum = 0;
	unsigned char *cp = (data + len);

	len -= 10;
	cp -= 2;

	data += 8;

	for (; len >= 0; len--) {
		cksum += data[len];
	}

	cksum &= 0xffff;

	printf("=> Data checksum %04x\n", cksum);

	cp[0] = cksum;
	cp[1] = cksum >> 8;
}

int verify_device(struct usb_device *dev)
{
	DBG(("dev %p: configurations %d\n",
	     dev, dev->descriptor.bNumConfigurations));
	
	if (dev->descriptor.bNumConfigurations != 1)
		return 0;

	DBG(("\t=> bLength %d\n", dev->descriptor.bLength));
	DBG(("\t=> bType   %d\n", dev->descriptor.bDescriptorType));
	DBG(("\t=> bcdUSB  %x\n", dev->descriptor.bcdUSB));
	DBG(("\t=> idVendor %x\n", dev->descriptor.idVendor));
	DBG(("\t=> idProduct %x\n", dev->descriptor.idProduct));

	if (dev->descriptor.idVendor == 0x04e8 &&
	    dev->descriptor.idProduct == 0x1234)
		return 1;

	return 0;
}

struct option long_opts[] =  {
	{
		.name		= "address",
		.has_arg	= 1,
		.flag		= NULL,
		.val		= 'a'
	},
	{
		.name		= "bus",
		.has_arg	= 1,
		.flag		= NULL,
		.val		= 'b'
	},
	{
		.name		= "dev",
		.has_arg	= 1,
		.flag		= NULL,
		.val		= 'd'
	},
	{
		.name		= "file",
		.has_arg	= 1,
		.flag		= NULL,
		.val		= 'f'
	},
	{
		.name		= "show",
		.has_arg	= 0,
		.flag		= NULL,
		.val		= 's'
	},
	{
		.name		= "debug",
		.has_arg	= 0,
		.flag		= NULL,
		.val		= 'x',
	},
	{
		.name		= NULL
	}
};

int flg_show = 0;
#define TRY(expr) do { int rc = (expr); if (rc) { printf("FAILED '%s'\n", #expr);} } while (0)

int main(int argc, char **argv)
{
	struct usb_bus *bus, *busp;
	struct usb_device *result = NULL;
	struct usb_device *found  = NULL;
	unsigned long fsize;
	usb_dev_handle *devh;
	int ret;

	printf("SMDK42XX USB Download Tool\n");
	printf("Version 0.20 (c) 2004,2005,2006"
	       "Ben Dooks <ben-linux@fluff.org>\n");
	printf("\n");

	while (1) {
		int index = 0;
		int c;

		c = getopt_long(argc, argv, "a:b:d:f:s", long_opts, &index);

		DBG(("option index %d\n",c ));

		if (c == -1)
			break;

		switch (c) {
		case 's':
			flg_show = 1;
			break;

		case 'a':
			dl_addr = strtoul(optarg, NULL, 16);
			break;

		case 'f':
			dl_file = optarg;
			break;

		case 'b':
			dl_ubus = optarg;
			break;

		case 'd':
			dl_udev = optarg;
			break;

		case 'x':
			debug = 1;
		}
	}
	
	usb_init();
	usb_find_busses();
	usb_find_devices();
	
	bus = usb_get_busses();

	DBG(("usb_get_busses: %p\n", bus));
       
	for (busp = bus; busp != NULL; busp = busp->next) {	  
		struct usb_device *dev;

		DBG(("bus %p: dirname %s\n", busp, busp->dirname));

		if (dl_ubus) {
			if (strcmp(busp->dirname, dl_ubus) != 0)
				continue;
		}

		for (dev = busp->devices; dev != NULL; dev = dev->next) {
			DBG(("dev %p filename %s\n", dev, dev->filename));

			if (!verify_device(dev))
				continue;

			if (flg_show) {
				printf("bus %s: device %s\n", 
				       busp->dirname, dev->filename);
				continue;
			}

			found = dev;

			if (dl_udev) {
				if (strcmp(dev->filename, dl_udev) == 0) {
					result = dev;
					break;
				}
			}
		}

		if (result != NULL)
			break;
	}

	if (flg_show)
		return 0;

	DBG(("device %p, found %p\n", result, found));

	if (result == NULL && found != NULL)
		result = found;

	if (result == NULL) {
		fprintf(stderr, "failed to find device\n");
		return 1;
	}

	printf("=> found device: bus %s, dev %s\n",
	       result->bus->dirname, result->filename);

	dl_data = load_file(dl_file, &dl_size, &fsize);
	if (dl_data == NULL) {
		printf("failed to load %s\n", dl_file);
		return 1;
	}

	printf("=> loaded %ld bytes from %s\n", fsize, dl_file);

	devh = usb_open(result);
	if (devh == NULL) {
		perror("usb_open");
		return 1;
	}
	if (usb_claim_interface(devh, 0) < 0) {
		perror("usb_claim_interface");
		usb_close(devh);
		return 1;
	}
	if (usb_set_configuration(devh, 1) < 0) {
		perror("usb_set_configuration");
		usb_close(devh);
		return 1;
	}

	char *files[] = {
		"part1.bin",
		"part2.bin",
		"part3.bin",
		"part1.bin",
		"part4.bin",
		"part5.bin",
		"part6.bin",
	};
	free(dl_data);
	dl_data = NULL;

	int i;
	for (i = 0; i < sizeof(files)/sizeof(files[0]); i++)
	{
		dl_file = files[i];
		printf("Downloading file %s\n", dl_file);

		DBG(("claim interface\n"));
		dl_data = load_file(dl_file, &dl_size, &fsize);
		if (dl_data == NULL) {
			printf("failed to load %s\n", dl_file);
			return 1;
		}
		printf("=> Downloading %ld bytes to 0x%08lx\n", dl_size, dl_addr);
		write_header(dl_data, dl_addr, dl_size);
		calc_cksum(dl_data, dl_size);

		ret = usb_bulk_write(devh, 2, (void *)dl_data, dl_size, 50*1000*1000);
		printf("=> usb_bulk_write() returned %d\n", ret);

		if (ret != dl_size) {
			printf("failed to write %ld bytes\n", dl_size);
			exit(-1);
		}

		free(dl_data);
		dl_data = NULL;
	}

	usb_release_interface(devh, 0);
	usb_close(devh);
	devh = NULL;
	return 0;
}
