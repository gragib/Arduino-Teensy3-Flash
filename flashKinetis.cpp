/*
	Arduino - Kinetis ("Teensy") Flashing Library
	(c) Frank Boesing, f.boesing@gmx.de

	License:

	Private and educational use allowed.

	If you need this library for commecial use, please
	ask me.

	In every case, keep this header.
*/

#include "flashKinetis.h"

#define FCMD_READ_1S_BLOCK          	0x00
#define FCMD_READ_1S_SECTION        	0x01
#define FCMD_PROGRAM_CHECK          	0x02
#define FCMD_READ_RESOURCE          	0x03
#define FCMD_PROGRAM_LONG_WORD      	0x06
#define FCMD_ERASE_FLASH_BLOCK      	0x08
#define FCMD_ERASE_FLASH_SECTOR     	0x09
#define FCMD_PROGRAM_SECTOR         	0x0b
#define FCMD_READ_1S_ALL_BLOCKS     	0x40
#define FCMD_READ_ONCE              	0x41
#define FCMD_PROGRAM_ONCE           	0x43
#define FCMD_ERASE_ALL_BLOCKS       	0x44

#define FTFL_STAT_MGSTAT0 						0x01	// Error detected during sequence
#define FTFL_STAT_FPVIOL  						0x10	// Flash protection violation flag
#define FTFL_STAT_ACCERR  						0x20	// Flash access error flag
#define FTFL_STAT_CCIF    						0x80	// Command complete interrupt flag

#define FTFL_READ_MARGIN_NORMAL				0x00

/*
  ==================================================================
	Internal Functions
  ==================================================================
*/

#define FLASH_ALIGN(address,align) ((unsigned long*)((unsigned long)address & (~align)))

FASTRUN static int flashExec(void)																	// Execute Flash Command in RAM
{																																		// Returns non-zero if there was an error
	__disable_irq();
	FTFL_FSTAT = FTFL_STAT_CCIF;
	while (!(FTFL_FSTAT & FTFL_STAT_CCIF)) {;}
	__enable_irq();
	return (FTFL_FSTAT & (FTFL_STAT_ACCERR | FTFL_STAT_FPVIOL | FTFL_STAT_MGSTAT0));
}

static void flashInitCommand(unsigned char command, unsigned long *address)
{
	while (!(FTFL_FSTAT & FTFL_STAT_CCIF)) {;}
	FTFL_FSTAT  = 0x30;
	FTFL_FCCOB0 = command;
	FTFL_FCCOB1 = ((unsigned long)address) >> 16;
	FTFL_FCCOB2 = ((unsigned long)address) >> 8;
	FTFL_FCCOB3 = ((unsigned long)address);
}

/*
  ==================================================================
	User Functions
  ==================================================================
*/

int flashCheckSectorErased(unsigned long *address) 											// Check if sector is erased
{
	FLASH_ALIGN(address, FLASH_SECTOR_SIZE);
	flashInitCommand(FCMD_READ_1S_SECTION, address);
	const int num = FLASH_SECTOR_SIZE >> 2;
	FTFL_FCCOB4 = (unsigned char)(num >> 8);															// Number of longwords
	FTFL_FCCOB5 = (unsigned char)(num);
	FTFL_FCCOB6 = FTFL_READ_MARGIN_NORMAL;
	return flashExec();
}

int flashEraseSector(unsigned long *address)														// Erase Flash Sector
{
	FLASH_ALIGN(address, FLASH_SECTOR_SIZE);
	if (flashCheckSectorErased(address))
		{
			flashInitCommand(FCMD_ERASE_FLASH_SECTOR, address);
			return flashExec();
		}
	return 0;
}

int flashProgramWord(unsigned long *address, unsigned long *data)				// Program Flash, one long word (32 Bit)
{
	FLASH_ALIGN(address, 0x03);
	if (((((unsigned long)address)>=0x400) && ((unsigned long)address)<=0x40C))
		return 0;																														// Make sure not to write 0x400 - 0x40F
	flashInitCommand(FCMD_PROGRAM_LONG_WORD, address);
	FTFL_FCCOB4 = (unsigned char)(*data >> 24);            								// Enter the long word to be programmed
	FTFL_FCCOB5 = (unsigned char)(*data >> 16);
	FTFL_FCCOB6 = (unsigned char)(*data >> 8);
	FTFL_FCCOB7 = (unsigned char)*data;
	return flashExec();

}
