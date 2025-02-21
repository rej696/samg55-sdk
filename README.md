# SAMG55 Xplained Pro
setup a dummy blinky

used the samples provided in https://github.com/Microchip-MPLAB-Harmony/core_apps_sam_g55

This dev kit uses a SAMG55J19 chip

## Dependencies
You need a arm-none-eabi-gcc compiler (reasonably up to date)
you can set the compiler prefix with the PREFIX variable.

build with `make build` or `PREFIX=/opt/arm-none-eabi-toolchain/bin/arm-none-eabi make build`

You also need the edbg tool for programming: https://github.com/ataradov/edbg

## BSP/Libraries
extracted CMSIS Core pack and Microchip SAMG_DFP pack from MPLAB. These are in the lib directory.

We certainly don't need all these files, but its good to have them to start with.

There are two linker scripts provided, one for flash and one for sram. Not been
able to get the sram one to work/program, but the flash one works fine. You can swap these in the makefile

## Programming
The SAMG55 dev kit uses an embedded debugger (EDBG) which seems to be a
microsemi-ism that allows you to program over a usb cable. Microchips MPLABX
IDE uses some sort of python script interface that allows it to use CMSIS-DAP
to communicate with the embedded debugger on the board and program/debug the
chip. I wasn't able to figure out how to get that to work nicely (beyond using the IPE cmdline tool, though I didn't need to look into it)

Luckly, someone on the internet has written a guide to programming thes SAM devices: https://mtm.cba.mit.edu/2021/2021-10_microcontroller-primer/edbg/

Their guide points to this CMSIS_DAP compatible programming tool from: https://github.com/ataradov/edbg

program with `make flash`. Requires sudo currently need to figure out a way round that.

seems like you might be able to use openocd, for on target debugging https://mtm.cba.mit.edu/2021/2021-10_microcontroller-primer/openOCD/
