/*
* $Id: as_test_dev_cf_card.c,v 1.3 2007/03/16 20:39:41 lizhijie Exp $
*/

#include <stdio.h>
#include "aslib.h"
#include "as_test.h"
#include <sys/time.h>
#include "as_cf_card.h"

static unsigned int __overturn_data_int(unsigned int ot_data)
{
	unsigned char temp_char[4]={0,0,0,0},i;
	unsigned int temp_int=0;

	temp_int = ot_data;

	temp_char[0] |= ot_data;
	temp_char[1] |= ot_data >> 8;
	temp_char[2] |= ot_data >> 16;
	temp_char[3] |= ot_data >> 24;

	for(i=0;i<4;i++)
	{
		temp_int = temp_int << 8;
		temp_int |= temp_char[i];
	}
	
	return temp_int;
}

static unsigned short int __overturn_data_short_int(unsigned short int ot_data)
{
	unsigned char temp_char[2]={0,0},i;
	unsigned short int temp_short_int=0;
	
	temp_short_int = ot_data;
		
	temp_char[0] |= ot_data;
	temp_char[1] |= ot_data >> 8;
	for(i=0;i<2;i++)
	{
		temp_short_int = temp_short_int << 8;
		temp_short_int |= temp_char[i];
	}
		
	return temp_short_int;
}


static int __cf_data_search(ext3_super_block *super_block)
{
	unsigned short int  block_magic;
	unsigned int blocks_size;
	unsigned int blocks_count;
	unsigned int temp;
	int sum_size;

	block_magic=__overturn_data_short_int(super_block->s_magic);
	if(block_magic != S_MAIGC_VALU)
	{
		AS_MSG_LOG(AS_LOG_ERR, "\tAssist CompactFlash Card is not EXT2/EXT3 filesystem!");
		return -AS_CFC_NOT_EXT_FAIL;
	}

	AS_MSG_LOG(AS_LOG_INFO,"\tFile System\t\t: EXT2/EXT3");
	AS_MSG_LOG(AS_LOG_INFO,"\tInode Count\t\t: %d",__overturn_data_int(super_block ->s_inodes_count));

	blocks_count=__overturn_data_int(super_block ->s_blocks_count);
	AS_MSG_LOG(AS_LOG_INFO,"\tBlock Count\t\t: %d",blocks_count);
	
	temp=__overturn_data_int(super_block ->s_log_block_size);
	if(temp == 0)
	{
		blocks_size=1024;
	}
	else if(temp == 1)
	{
		blocks_size=4096;
	}
	else
	{
		return -AS_CFC_BLOCK_SIZE_FAIL;
	}
	AS_MSG_LOG(AS_LOG_INFO,"\tBlock Size\t\t: %d Bytes",blocks_size);
	
	AS_MSG_LOG(AS_LOG_INFO,"\tBlock Per Group\t\t: %d",__overturn_data_int(super_block ->s_inodes_per_group));

	sum_size = blocks_count * blocks_size;
	AS_MSG_LOG(AS_LOG_INFO,"\tTotal Space\t\t: %d Bytes",sum_size);

	AS_MSG_LOG(AS_LOG_INFO,"Test Result \t: Assist CompactFlash Card test is successful!");
	return 0;
}

int as_test_cf_card(void *data)
{
	int res;
	int fd;
	ext3_super_block *cfc;
	unsigned char str[__AS_STR_LENTH];
	unsigned char rcf[__AS_RCF_LENTH];

	sprintf(str,"/%s/%s\n",__AS_DIR_NAME,__AS_DIR_CF_CARD);
	fd = open("/dev/hda1", O_RDWR);
	if (fd < 0) 
	{		
		AS_MSG_LOG(AS_LOG_ERR, "Unable to open %s: %s", str, strerror(errno));		
		return -1;	
	}

	AS_MSG_LOG(AS_LOG_INFO,"IDE Dvice CompactFlash Card Info:");
	
	res = read(fd,rcf,__AS_RCF_LENTH);
	if(res < 0)
	{
		AS_MSG_LOG(AS_LOG_ERR,"fail to read %s: %s", str, strerror(errno));
		return -1;
	}

	cfc = (ext3_super_block *)&rcf[1024];

	return __cf_data_search(cfc);
	
}

