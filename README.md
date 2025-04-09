Xytronic LF-1600 Open Source Firmware
=====================================

[Homepage](https://bues.ch/h/xytronic)

[Git repository](https://bues.ch/cgit/xytronic-lf.git)

[Github repository](https://github.com/mbuesch/xytronic-lf)

Copyright (c) 2015-2025 Michael Büsch <m@bues.ch> and contributors

Licensed under the terms of the GNU General Public License version 2 or (at your option) any later version. 

Selecting the correct firmware hex image:
-----------------------------------------

The `hex` subdirectory of this package contains various pre-built configurations of the firmware.

* There are hex images available for legacy (through-hole) boards and newer SMD boards.

* Images for controller types AtMega-88 (the original controller on legacy through-hole board) or AtMega-88p (the original controller on SMD board) or AtMega-328p (upgrade controller) are available.

* Images with or without debug support (debug vs. release) are available.

If you don't know which one pick, take either `hex/board_legacy/atmega88/release` or `hex/board_smd/atmega88p/release` depending on your board type.

Installing the firmware:
------------------------

1. Erase the chip:
    
   If you use avrdude, a command similar to the following can be used:

     `avrdude -B 200 -p m88 -c avrisp2 -P usb -e`

     or

     `avrdude -B 200 -p m88p -c avrisp2 -P usb -e`

     or

     `avrdude -B 200 -p m328p -c avrisp2 -P usb -e`

1. Set the fuse bits of the AtMega88 to:

   `LFUSE = %%M88_LFUSE%%`

   `HFUSE = %%M88_HFUSE%%`

   `EFUSE = %%M88_EFUSE%%`

   Set the fuse bits of the AtMega88p to:

   `LFUSE = %%M88P_LFUSE%%`

   `HFUSE = %%M88P_HFUSE%%`

   `EFUSE = %%M88P_EFUSE%%`

   Set the fuse bits of the AtMega328p to:

   `LFUSE = %%M328P_LFUSE%%`

   `HFUSE = %%M328P_HFUSE%%`

   `EFUSE = %%M328P_EFUSE%%`

   Note that the original firmware uses different fuses. So this step must not be omitted.

   If you use avrdude, a command similar to the following can be used:

     `avrdude -B 200 -p m88 -c avrisp2 -P usb -U lfuse:w:%%M88_LFUSE%%:m -U hfuse:w:%%M88_HFUSE%%:m -U efuse:w:%%M88_EFUSE%%:m`

     or

     `avrdude -B 200 -p m88p -c avrisp2 -P usb -U lfuse:w:%%M88P_LFUSE%%:m -U hfuse:w:%%M88P_HFUSE%%:m -U efuse:w:%%M88P_EFUSE%%:m`

     or

     `avrdude -B 200 -p m328p -c avrisp2 -P usb -U lfuse:w:%%M328P_LFUSE%%:m -U hfuse:w:%%M328P_HFUSE%%:m -U efuse:w:%%M328P_EFUSE%%:m`

2. Flash the firmware image file xytronic-lf.hex to the program memory of the AtMega88.

   If you use avrdude, a command similar to the following can be used:

     `avrdude -B 1 -p m88 -c avrisp2 -P usb -U flash:w:xytronic-lf.hex`

     or

     `avrdude -B 1 -p m88p -c avrisp2 -P usb -U flash:w:xytronic-lf.hex`

     or

     `avrdude -B 1 -p m328p -c avrisp2 -P usb -U flash:w:xytronic-lf.hex`

3. Write the EEPROM image file xytronic-lf.eep.hex to the EEPROM memory of the AtMega88.

   If you use avrdude, a command similar to the following can be used:

     `avrdude -B 1 -p m88 -c avrisp2 -P usb -U eeprom:w:xytronic-lf.eep.hex`

     or

     `avrdude -B 1 -p m88p -c avrisp2 -P usb -U eeprom:w:xytronic-lf.eep.hex`

     or

     `avrdude -B 1 -p m328p -c avrisp2 -P usb -U eeprom:w:xytronic-lf.eep.hex`
