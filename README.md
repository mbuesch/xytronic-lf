Xytronic LF-1600 Open Source Firmware
=====================================

Copyright (c) 2015-2016 Michael Buesch <m@bues.ch>

Licensed under the terms of the GNU General Public License version 2 or (at your option) any later version. 

Installing the firmware:
------------------------

1. Set the fuse bits of the AtMega88 to:

   `LFUSE = %%M88_LFUSE%%`

   `HFUSE = %%M88_HFUSE%%`

   `EFUSE = %%M88_EFUSE%%`

   Set the fuse bits of the AtMega328p to:

   `LFUSE = %%M328P_LFUSE%%`

   `HFUSE = %%M328P_HFUSE%%`

   `EFUSE = %%M328P_EFUSE%%`

   Note that the original firmware uses different fuses. So this step must not be omitted.

   If you use avrdude, a command similar to the following can be used:

     `avrdude -B 200 -p m88 -c avrisp2 -P usb -U lfuse:w:%%M88_LFUSE%%:m -U hfuse:w:%%M88_HFUSE%%:m -U efuse:w:%%M88_EFUSE%%:m`

     or

     `avrdude -B 200 -p m328p -c avrisp2 -P usb -U lfuse:w:%%M328P_LFUSE%%:m -U hfuse:w:%%M328P_HFUSE%%:m -U efuse:w:%%M328P_EFUSE%%:m`

2. Flash the firmware image file xytronic-lf.hex to the program memory of the AtMega88.

   If you use avrdude, a command similar to the following can be used:

     `avrdude -B 1 -p m88 -c avrisp2 -P usb -U flash:w:xytronic-lf.hex`

     or

     `avrdude -B 1 -p m328p -c avrisp2 -P usb -U flash:w:xytronic-lf.hex`

3. Write the EEPROM image file xytronic-lf.eep.hex to the EEPROM memory of the AtMega88.

   If you use avrdude, a command similar to the following can be used:

     `avrdude -B 1 -p m88 -c avrisp2 -P usb -U eeprom:w:xytronic-lf.eep.hex`

     or

     `avrdude -B 1 -p m328p -c avrisp2 -P usb -U eeprom:w:xytronic-lf.eep.hex`
