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

/*  */
#define ADDRESS_START				0x50140000
#define ADDRESS_END					0x50400000


#define MAIN_BLOCK_SIZE_BY_WORD		0x8000

/* initrd :2048K space equal 64 Blocks :size is 0x002c,0000 */
#define BLOCK_COUNT		88		/**/

#define SRC_DATA_BUF				0x100000

#define BUS_WIDTH_16				0


void erase_jffs2_3m_blocks()
{
	ulong32 BlockAddress = ADDRESS_START;
	volatile ushort16 status;
	int     i;
	ulong32 count = 0;

	for(i=0;i<BLOCK_COUNT;i++)
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

void unlock_jffs2_3m_blocks()
{
	ulong32 BlockAddress = ADDRESS_START;
	volatile ushort16 status;
	int		i;
	ulong32 count = 0;

	for(i=0;i<BLOCK_COUNT;i++)
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


void burn_jffs2_3m_blocks()
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


void flash_jffs2_3m_image()
{
	unlock_jffs2_3m_blocks();
	erase_jffs2_3m_blocks();
	burn_jffs2_3m_blocks();
}

