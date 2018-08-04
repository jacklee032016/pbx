/* Burned just blocks that Bootloader occupied             */
/* Modified by Li zhijie 2004.08.02                        */
#define FLASH_CMD_READ			0xFFFF
#define FLASH_CMD_ERASE			0x2020
#define FLASH_CMD_WRITE			0x4040
#define FLASH_CMD_UNLOCK		0x6060
#define FLASH_CMD_STATUS		0x7070
#define FLASH_CMD_READ_ID		0x9090
#define FLASH_CMD_CONFIRM		0xD0D0


#define SCSREADY 0x0080  /* Scalable Command Set */

typedef unsigned char	uchar8;
typedef unsigned short	ushort16;
typedef unsigned long	ulong32;
typedef unsigned char	* addr32;

/*  */
#define KERNEL_START				0x50030000
/* 
#define ROOT_FS_START				0x50130000
*/
/*  changed as following for release map of FLASH,
	kernel occupy about 1024+512 = 1536 Kx
 lizhijie 2004.12.29*/
#define ROOT_FS_START				0x501B0000

#define MAIN_BLOCK_SIZE_BY_WORD		0x8000

/* Kernel : 1M space equal 32 Blocks */
/*
#define BLOCK_COUNT_OF_KERNEL	32		
*/
/* change as 48 for about 1536K , lizhijie 2004.12.29 */
#define BLOCK_COUNT_OF_KERNEL		48		/**/

#define SRC_DATA_BUF				0x100000

#define BUS_WIDTH_16				0


void erase_kernel_blocks()
{
	ulong32 BlockAddress = KERNEL_START;
	volatile ushort16 status;
	int     i;
	ulong32 count = 0;

	for(i=0;i<BLOCK_COUNT_OF_KERNEL;i++)
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


	BlockAddress = KERNEL_START;
	*((ushort16*)(BlockAddress <<BUS_WIDTH_16)) = FLASH_CMD_READ;

}

void unlock_kernel_blocks()
{
	ulong32 BlockAddress = KERNEL_START;
	volatile ushort16 status;
	int		i;
	ulong32 count = 0;

	for(i=0;i<BLOCK_COUNT_OF_KERNEL;i++)
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

	BlockAddress = KERNEL_START;
	*((ushort16*)(BlockAddress<<BUS_WIDTH_16)) = FLASH_CMD_READ;
}


void burn_kernel_blocks()
{
	ulong32 BlockAddress = KERNEL_START;
	ulong32 pointer = KERNEL_START;
	ulong32 data = SRC_DATA_BUF;
	volatile ushort16 status;
	ulong32 count = 0;
	

	while (BlockAddress < ROOT_FS_START )
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

	BlockAddress = KERNEL_START;
	*((ushort16*)(BlockAddress<<BUS_WIDTH_16)) = FLASH_CMD_READ;

}


void flash_kernel_image()
{
	unlock_kernel_blocks();
	erase_kernel_blocks();
	burn_kernel_blocks();
}

