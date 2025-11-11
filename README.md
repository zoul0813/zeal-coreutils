# Zeal coreutils

This is a coreutils collection for [Zeal 8-bit Computer](https://zeal8bit.com/).

This collection contains the core utilities for interacting with [Zeal 8-Bit OS](https://github.com/Zeal8bit/Zeal-8-bit-OS/).

**The following utilities are included:**

* cat - output the contents of a file to the terminal
* cp - copy src file to dst file
* crc32 - print the crc32 of each file argument
* du - show size of each directory
* echo - echo the arguments to the terminal
* hexdump - output the contents of a file to the terminal in hex format
* ls - list a directory
* mkdir - make a directory
* mv - move src file to dst file
* rm - remove/delete a file
* setfont - allows you to change terminal fonts
* touch - create an empty file
* tree - show a directory tree

**The following utilties are not currently implemented:**

* dir
* less
* xfer

## Installation

Copy these utilities to your ROM Disk, or install them on any other disk.

## Building from source

Make sure that you have [ZDE](https://github.com/zoul0813/zeal-dev-environment) installed.

Then open a terminal, go to the source directory and type the following commands:

```shell
    zde cmake
```
