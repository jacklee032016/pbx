

#define FLASH_CMD_READ					0x00FF
#define FLASH_CMD_ERASE				0x0020
#define FLASH_CMD_WRITE				0x0040
#define FLASH_CMD_UNLOCK				0x0060
#define FLASH_CMD_STATUS				0x0070
#define FLASH_CMD_READ_ID				0x0090
#define FLASH_CMD_CONFIRM				0x00D0

#define FlashCommandClear       			0x50
#define FlashCommandLoadPB      			0xE0
#define FlashCommandPBWrite     			0x0C
#define FlashCommandSuspend     			0xB0
#define FlashCommandResume      			0xD0
#define FlashCommandReadESR     			0x71
#define FlashCommandQueryCFI    		0x98
#define FlashCommandSCSErase    		0x28
#define FlashCommandSCSWrite    			0xE8
#define FlashStatusReady        			0x80
#define FlashStatusSuspended    			0x40
#define FlashStatusError        				0x3E
#define FlashStatusBlockError   			0x3F

#define SCSREADY 						0x0080  /* Scalable Command Set */

typedef unsigned char	uchar8;
typedef unsigned short	ushort16;
typedef unsigned long	ulong32;
typedef unsigned char	* addr32;

#define FLASH_START						0x50000000
#define KERNEL_START					0x50040000
#define ROOT_FS_START					0x501C0000

#define MAIN_BLOCK_SIZE_BY_WORD		0x20000

#define BLOCK_COUNT_OF_KERNEL			12
#define OPTION_COUNT_OF_KERNEL		12

#define SRC_DATA_BUF					0x100000


void flash_kernel_image()
{
	ulong32 BlockAddress = KERNEL_START;
	volatile ushort16 status = 0;
	ulong32 data = SRC_DATA_BUF;
	ulong32 dst = 0;
	ulong32 Temp_BlockAddress = KERNEL_START;
	ulong32 i = 0;

	for(i = 0;i< BLOCK_COUNT_OF_KERNEL;i++ )
	{
		*((ushort16*)BlockAddress) = FLASH_CMD_UNLOCK;
	
		*((ushort16*)BlockAddress) = FLASH_CMD_CONFIRM;

		do
		{
			*((ushort16*)BlockAddress) = FLASH_CMD_STATUS;
			status = *((ushort16*)(BlockAddress));
		}while (!(status & SCSREADY)); 

		Temp_BlockAddress = BlockAddress;
		*((ushort16*)Temp_BlockAddress) = FLASH_CMD_READ;
		
		BlockAddress += MAIN_BLOCK_SIZE_BY_WORD;
		
	}
	

	
	BlockAddress = KERNEL_START;
	for(i = 0;i< BLOCK_COUNT_OF_KERNEL;i++ )
	{
		*((ushort16*)BlockAddress) = FLASH_CMD_ERASE;

    		*((ushort16*)BlockAddress) = FLASH_CMD_CONFIRM;
	    
		do
		{
			*((ushort16*)BlockAddress) = FLASH_CMD_STATUS;
			status = *((ushort16*)(BlockAddress));
		}while (!(status & SCSREADY)); 

		Temp_BlockAddress = BlockAddress;
		*((ushort16*)Temp_BlockAddress) = FLASH_CMD_READ;

		BlockAddress += MAIN_BLOCK_SIZE_BY_WORD;
	}
	


	BlockAddress = KERNEL_START;
	status = 0;
	while(BlockAddress < ROOT_FS_START)
	{
		i = 0;
		
		while (i < MAIN_BLOCK_SIZE_BY_WORD)
		{
			
			dst = BlockAddress + i ;
			
			*((ushort16*)(dst)) = FLASH_CMD_WRITE;

			*((ushort16*)dst)=*((ushort16*)data);
			
			data += 2;
			i += 2;

			do
			{
				*((ushort16*)BlockAddress) = FLASH_CMD_STATUS;
				status = *((ushort16*)BlockAddress);
			}while (!(status & SCSREADY)); 
	
		}    

		Temp_BlockAddress = BlockAddress;
		*((ushort16*)Temp_BlockAddress) = FLASH_CMD_READ;

		BlockAddress += MAIN_BLOCK_SIZE_BY_WORD;
	}

	i++;

}
















