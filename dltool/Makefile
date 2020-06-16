# dltool/Makefile
#
# Makefile for SMDK24XX dltool
#
# (c) 2004,2006 Ben Dooks, Ben Dooks <ben-smdk2440@fluff.org>
#

.PHONY:	clean

dltool:	dltool.c
	$(CC) -O2 -Wall -g -lusb -o smdk-usbdl dltool.c

clean:
	-rm smdk-usbdl
	-rm *.o