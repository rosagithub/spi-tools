spi-tools
=========

SPI tools for the Linux kernel

This package is only a version of spidev_test.c from
  - Anton Vorontsov <avorontsov@ru.mvista.com>
  - MontaVista Software, Inc.
with some litle improvement.
It is a SPI tool for the Linux kernel.


CONTENTS
--------

A simple spidev tool to send read/write spi transaction.
The spidev driver must be installed.

INSTALLATION
------------

There's no configure script, so simply run "make" to build the tools, and
"make install" to install them. By default, files are installed in /usr/bin
but you can change this behavior by editing the Makefile file and setting
prefix to wherever you want. You may change the C compiler and the
compilation flags as well.

In a cross environment, you can use DEST_DIR make variable to prefix
the intall path. For example, do:
  $ make install DEST=$PWD/install


If you wish to cross compile, then just set the cross compiler prefix via
the CROSS_COMPILE make variable. For example, do:
  $ make CROSS_COMPILE=/opt/arm-2009q1/bin/arm-none-linux-gnueabi-


DOCUMENTATION
-------------

See the spidev_test usage from the console :

 *      -D --device   device to use (default /dev/spidev1.1)
 *      -s --speed    max speed (Hz)
 *      -d --delay    delay (usec)
 *      -b --bpw      bits per word
 *      -l --loop     loopback
 *      -H --cpha     clock phase
 *      -O --cpol     clock polarity
 *      -L --lsb      least significant bit first
 *      -C --cs-high  chip select active high
 *      -3 --3wire    SI/SO signals shared


QUESTIONS AND BUG REPORTS
-------------------------

See the linux kernel documentation for more information /usr/src/linux/Documentation/spi/

Please post your questions and bug reports to github:
  https://github.com/rosagithub/spi-tools
