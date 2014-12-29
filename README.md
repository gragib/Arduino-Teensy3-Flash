Arduino-Teensy3-Flash
=====================

Program the Teensy-Flash

Check sector is erased:
flashCheckSectorErased()

Erase a sector:
flashEraseSector()
Before erasing, the function checks the sector if it is empty. If so, it does nothing.
This is to prevent unnecessary stress.

Program a sector with unsigned int:
flashProgramWord().
The locations 0x400-0x400f are not written to prevent unnecessary stress to YOU ;-)
(Hope this works, i hav'nt tested it)

Please see the example.

The size of a sector is 2048 Bytes.
