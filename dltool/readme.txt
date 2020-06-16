SMDK24XX USB Download Tool
==========================

(c) 2004,2006 Ben Dooks <ben-linux@fluff.org>

Introduction
------------

This provides a UNIX (Linux,etc) based download tool to
communicate with the Samsung SMDK24XX range of boards.

Requirements
------------

This tool requires a unix environment, with libusb installed
and available to the complier.

Build
-----

To make the code, use `make` to build the ouput file, which
is called smdk-usbdl

Running
-------

smdk-usbdl:

	-f <filename>	change the filename to download
			default is download.dat

	-a <address>	set the download address
			default is 0x30000000

	-s		show any smdk board(s) connected

	-b		select usb bus shown by `-s`

	-d		select usb device shown by `-s`

	-x		enable debugging output

Homepage
--------

http://www.fluff.org/ben/smdk/tools/
