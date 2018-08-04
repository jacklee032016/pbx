/* Burned just blocks that Bootloader occupied             */
/* Modified by Li zhijie 2004.08.02                        */
#define FLASH_CMD_READ			0xFFFF
#define FLASH_CMD_ERASE			0x2020
#define FLASH_CMD_WRITE			0x4040
#define FLASH_CMD_UNLOCK		0x6060
#define FLASH_CMD_STATUS		0x7070
#define FLASH_CMD_READ_ID		0x9090
#define FLASH_CMD_CONFIRM		0xD0D0

#define FlashCommandClear       0x50
#define FlashCommandLoadPB      0xE0
#define FlashCommandPBWrite     0x0C
#define FlashCommandSuspend     0xB0
#define FlashCommandResume      0xD0
#define FlashCommandReadESR     0x71
#define FlashCommandQueryCFI    0x98
#define FlashCommandSCSErase    0x28
#define FlashCommandSCSWrite    0xE8
#define FlashStatusReady        0x80
#define FlashStatusSuspended    0x40
#define FlashStatusError        0x3E
#define FlashStatusBlockError   0x3F

#define SCSREADY 0x0080  /* Scalable Command Set */

typedef unsigned char	uchar8;
typedef unsigned short	ushort16;
typedef unsigned long	ulong32;
typedef unsigned char	* addr32;

#define FLASH_START					0x50000000
#define KERNEL_START				0x50040000
#define ROOT_FS_START				0x50140000

#define MAIN_BLOCK_SIZE_BY_WORD		0x8000

/*program : 2 blocks; env : 1 blocks */
#define BLOCK_COUNT_OF_BOOTLOADER	6		

#define SRC_DATA_BUF				0x100000

#define BUS_WIDTH_16				0

void erase_bootloader_blocks()
{
	ulong32 BlockAddress = FLASH_START;
	volatile ushort16 status;
	int     i;
	ulong32 count = 0;

	for(i=0;i<BLOCK_COUNT_OF_BOOTLOADER;i++)
	{
		
		*((ushort16*)(BlockAddress<<BUS_WIDTH_16)) = FLASH_CMD_ERASE;
	    *((ushort16*)(BlockAddress<<BUS_WIDTH_16)) = FLASH_CMD_CONFIRM;
	    
	    count += 1;

		do
		{
			*((ushort16*)(BlockAddress<<BUS_WIDTH_16)) = FLASH_CMD_STATUS;
			status = *((ushort16*)(BlockAddress));
			
		}while (!(status & SCSREADY)); 
		
		BlockAddress += MAIN_BLOCK_SIZE_BY_WORD;
	}


	BlockAddress = FLASH_START;
	*((ushort16*)(BlockAddress <<BUS_WIDTH_16)) = FLASH_CMD_READ;

}

void unlock_bootloader_blocks()
{
	ulong32 BlockAddress = FLASH_START;
	volatile ushort16 status;
	int		i;
	ulong32 count = 0;

	for(i=0;i<BLOCK_COUNT_OF_BOOTLOADER;i++)
	{

		*((ushort16*)(BlockAddress<<BUS_WIDTH_16)) = FLASH_CMD_UNLOCK;
		*((ushort16*)(BlockAddress<<BUS_WIDTH_16)) = FLASH_CMD_CONFIRM;
		count += 1;

		{
			*((ushort16*)(BlockAddress<<BUS_WIDTH_16)) = FLASH_CMD_STATUS;
			status = *((ushort16*)(BlockAddress));
			
		}while (!(status & SCSREADY)); 

		BlockAddress += MAIN_BLOCK_SIZE_BY_WORD;
	}

	BlockAddress = FLASH_START;
	*((ushort16*)(BlockAddress<<BUS_WIDTH_16)) = FLASH_CMD_READ;
}


void burn_bootloader_blocks()
{
	ulong32 BlockAddress = FLASH_START;
	ulong32 pointer = FLASH_START;
	ulong32 data = SRC_DATA_BUF;
	volatile ushort16 status;
	ulong32 count = 0;
	

	while (BlockAddress < KERNEL_START )
	{
		while (pointer < BlockAddress + MAIN_BLOCK_SIZE_BY_WORD)
		{
			*((ushort16*)(BlockAddress<<BUS_WIDTH_16)) = FLASH_CMD_WRITE;

			*((ushort16*)pointer) = *((ushort16*)data);
			pointer += 2;
			data += 2;
			count += 2;

			do
			{
				*((ushort16*)(BlockAddress<<BUS_WIDTH_16)) = FLASH_CMD_STATUS;
				status = *((ushort16*)(BlockAddress));
			
			}while (!(status & SCSREADY)); 

		}
		BlockAddress += MAIN_BLOCK_SIZE_BY_WORD;

	}    

	BlockAddress = FLASH_START;
	*((ushort16*)(BlockAddress<<BUS_WIDTH_16)) = FLASH_CMD_READ;

}


void flash_program()
{
	unlock_bootloader_blocks();
	erase_bootloader_blocks();
	burn_bootloader_blocks();
}

