AVR Template
============

## Toolchain

### Ubuntu

The AVR toolchain, C libraries, and the avrdude downloader/uploader, can be
installed from the Ubuntu repositories:

    apt-get install gcc-avr avr-libc avrdude

## Building

1. Clone the [avr-template](https://github.com/uctools/avr-templates) repository
   (or fork it and clone your own repository).

        git clone git@github.com:uctools:avr-template

2. Modify the Makefile:
    * Set TARGET to the desired name of the output file (eg: TARGET = main)
    * Set SOURCES to a list of your sources (eg: SOURCES = main.c two.c three.c)
    * Set MCU to the MSP430 part number to build for (eg: MCU = atmega328p)

3. Run `make`

4. The output files will be created in the 'build' folder
