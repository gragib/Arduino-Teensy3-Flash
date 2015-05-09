/*
	Arduino - Kinetis ("Teensy") Flashing Library
	(c) Frank Boesing, f.boesing@gmx.de

	License:

	Private and educational use allowed.

	If you need this library for commecial use, please
	ask me.

	In every case, keep this header.
*/


#ifndef flashKinetis_h_
#define flashKinetis_h_

#if defined(__MK20DX128__)
#define FLASH_SIZE			0x0001FFFF
#define FLASH_SECTOR_SIZE	0x800
#elif defined(__MK20DX256__)
#define FLASH_SIZE			0x0003FFFF
#define FLASH_SECTOR_SIZE	0x800
#else
#error NOT SUPPORTED
#endif

#include "kinetis.h"
#include "WProgram.h"

int flashCheckSectorErased(unsigned long *address);
int flashEraseSector(unsigned long *address);
int flashProgramWord(unsigned long *address, unsigned long *data);
void flashSetFlexRAM(void);
#endif