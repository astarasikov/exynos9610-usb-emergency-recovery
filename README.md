# Intro
This is a USB recovery mode download tool for Samsung A505FN (Exynos9610).
It is based on the "smdk-tools-v0.20" tool for S3C2410 (see second commit),
the only practical difference is using a different endpoint and sending multiple data
chunks at once.

References
==========
http://www.fluff.org/ben/smdk/tools/downloads/smdk-tools-v0.20.tar.gz

USB Download/Recovery implementations
=====================================
These ones are open-source.

https://gist.github.com/suapapa/1244403/73d95a53361f00784d16ee5fda220734d22ddb65
http://www.fluff.org/ben/smdk/tools/downloads/
https://gitweb.gentoo.org/repo/gentoo.git/tree/dev-embedded/smdk-dltool/smdk-dltool-0.20-r4.ebuild

https://forum.xda-developers.com/showpost.php?p=18485215&postcount=146
https://github.com/coreos/dev-util/blob/master/host/lib/write_firmware.py

https://android.googlesource.com/device/casio/koi-uboot/+/marshmallow-mr1-wear-release/arch/arm/include/asm/arch-exynos/smc.h
https://github.com/hardkernel/u-boot/blob/odroidxu3-v2012.07/board/samsung/smdk4x12/smc.c
SMC_CMD_LOAD_UBOOT
