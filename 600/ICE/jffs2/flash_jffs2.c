/* Burned just blocks that jffs2 occupied             */
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

/*  
#define ADDRESS_START			0x50200000
#define ADDRESS_END				0x50400000
*/
/* change as following for new flash map 
	JFFS2 root filesystem 2368K
lizhijie 2004.12.29
*/
#define ADDRESS_START			0x501B0000
#define ADDRESS_END				0x50400000

#define MAIN_BLOCK_SIZE_BY_WORD		0x8000

/* initrd :2048K space equal 64 Blocks */
/* changed as following lizhijie 2004.12.29
#define BLOCK_COUNT_OF_KERNEL	64		
*/

/* for 2368 K JFFS2 file system, lizhijie 2004.12.29
*/
#define BLOCK_COUNT_OF_KERNEL	74		

#define SRC_DATA_BUF				0x100000

#define BUS_WIDTH_16				0


void erase_jffs2_blocks()
{
	ulong32 BlockAddress = ADDRESS_START;
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


	BlockAddress = ADDRESS_START;
	*((ushort16*)(BlockAddress <<BUS_WIDTH_16)) = FLASH_CMD_READ;

}

void unlock_jffs2_blocks()
{
	ulong32 BlockAddress = ADDRESS_START;
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

	BlockAddress = ADDRESS_START;
	*((ushort16*)(BlockAddress<<BUS_WIDTH_16)) = FLASH_CMD_READ;
}


void burn_jffs2_blocks()
{
	ulong32 BlockAddress = ADDRESS_START;
	ulong32 pointer = ADDRESS_START;
	ulong32 data = SRC_DATA_BUF;
	volatile ushort16 status;
	ulong32 count = 0;
	

	while (BlockAddress < ADDRESS_END )
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

	BlockAddress = ADDRESS_START;
	*((ushort16*)(BlockAddress<<BUS_WIDTH_16)) = FLASH_CMD_READ;

}


void flash_jffs2_image()
{
	unlock_jffs2_blocks();
	erase_jffs2_blocks();
	burn_jffs2_blocks();
}

