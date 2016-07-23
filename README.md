Xytronic LF-1600 Open Source Firmware
=====================================

Copyright (c) 2015-2016 Michael Buesch <m@bues.ch>

Licensed under the terms of the GNU General Public License version 2 or (at your option) any later version. 

Installing the firmware:
------------------------

1. Set the fuse bits of the AtMega88 to:

   `LFUSE = %%LFUSE%%`

   `HFUSE = %%HFUSE%%`

   `EFUSE = %%EFUSE%%`

   Note that the original firmware uses different fuses. So this step must not be omitted.

   If you use avrdude, a command similar to the following can be used:

     `avrdude -B 200 -p m88 -c avrisp2 -P usb -U lfuse:w:%%LFUSE%%:m -U hfuse:w:%%HFUSE%%:m -U efuse:w:%%EFUSE%%:m`

2. Flash the firmware image file xytronic-lf.hex to the program memory of the AtMega88.

   If you use avrdude, a command similar to the following can be used:

     `avrdude -B 1 -p m88 -c avrisp2 -P usb -U flash:w:xytronic-lf.hex`

3. Write the EEPROM image file xytronic-lf.eep.hex to the EEPROM memory of the AtMega88.

   If you use avrdude, a command similar to the following can be used:

     `avrdude -B 1 -p m88 -c avrisp2 -P usb -U eeprom:w:xytronic-lf.eep.hex`
