/*
* $Id: as_flash.c,v 1.11 2007/03/16 20:39:41 lizhijie Exp $
*/

#include "as_misc_dev.h"

#include "aslib.h"

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include <sys/ioctl.h>
#include <stdarg.h>
#include <linux/mtd/mtd.h>
#include <ctype.h>

AS_FLASH_INFO_T		factoryDefault = 
{
	serialNo		:	"01234567899876543210",
	userName	:	"lizhijie",
	passwd		:	"admin",
	
	wanMac		:	"00:02:B3:01:01:01",
	wanIp		:	"219.111.2.40",
	wanMask		:	"255.255.255.255",
	
	lanMac		:	"00:02:B3:02:02:02",
	lanIp		:	"192.168.3.112",
	lanMask		:	"255.255.255.0",
	
};

static int __as_flash_region_erase(int Fd, int start, int count, int unlock, int regcount)
{
	int i, j;
	region_info_t *reginfo;

	reginfo = calloc(regcount, sizeof(region_info_t));

	for(i = 0; i < regcount; i++)
	{
		reginfo[i].regionindex = i;
		if(ioctl(Fd,MEMGETREGIONINFO,&(reginfo[i])) != 0)
			return 8;
		else
			AS_MSG_LOG(AS_LOG_INFO,"Region %d is at %d of %d sector and with sector "
					"size %x\n", i, reginfo[i].offset, reginfo[i].numblocks,
					reginfo[i].erasesize);
	}

	// We have all the information about the chip we need.

	for(i = 0; i < regcount; i++)
	{ 	//Loop through the regions
		region_info_t * r = &(reginfo[i]);

		if((start >= reginfo[i].offset) &&(start < (r->offset + r->numblocks*r->erasesize)))
			break;
	}

	if(i >= regcount)
	{
		AS_MSG_LOG(AS_LOG_ERR,"Starting offset %x not within chip.", start);
		return 8;
	}

	//We are now positioned within region i of the chip, so start erasing
	//count sectors from there.

	for(j = 0; (j < count)&&(i < regcount); j++)
	{
		erase_info_t erase;
		region_info_t * r = &(reginfo[i]);

		erase.start = start;
		erase.length = r->erasesize;

		if(unlock != 0)
		{ 	//Unlock the sector first.
			if(ioctl(Fd, MEMUNLOCK, &erase) != 0)
			{
				AS_MSG_LOG(AS_LOG_ERR,"MTD Unlock failure : %s", strerror(errno) );
				close(Fd);
				return 8;
			}
		}
	
		fflush(stdout);
		if(ioctl(Fd, MEMERASE, &erase) != 0)
		{
			AS_MSG_LOG(AS_LOG_ERR, "MTD Erase failure : %s", strerror(errno) );
			close(Fd);
			return 8;
		}

		start += erase.length;
		if(start >= (r->offset + r->numblocks*r->erasesize))
		{ 	//We finished region i so move to region i+1
			i++;
		}
	}

	return 0;
}


static int __as_flash_non_region_erase(int Fd, int start, int count, int unlock)
{
	mtd_info_t meminfo;

	if (ioctl(Fd,MEMGETINFO,&meminfo) == 0)
	{
		erase_info_t erase;

		erase.start = start;

		erase.length = meminfo.erasesize;

		for (; count > 0; count--) 
		{
			fflush(stdout);

			if(unlock != 0)
			{
				if(ioctl(Fd, MEMUNLOCK, &erase) != 0)
				{
					AS_MSG_LOG(AS_LOG_ERR,"MTD Unlock failure : %s", strerror(errno) );
					close(Fd);
					return 8;
				}
			}

			if (ioctl(Fd,MEMERASE,&erase) != 0)
			{      
				AS_MSG_LOG(AS_LOG_ERR, "MTD Erase failure : %s", strerror(errno) );
				close(Fd);
				return 8;
			}
			erase.start += meminfo.erasesize;
		}
	}
	return 0;
}

/* lock */
static int __as_flash_lock(int dev_fd)
{
  	int num_sectors;
  	int ofs;
   	struct mtd_info_user mtdInfo;
  	struct erase_info_user mtdLockInfo;

  	if(ioctl(dev_fd, MEMGETINFO, &mtdInfo))
  	{
    		AS_MSG_LOG(AS_LOG_ERR, "Could not get MTD device info :%s", strerror(errno) );
    		close(dev_fd);
    		return (EXIT_FAILURE);
  	}

	ofs = LOCK_START;
	num_sectors = LOCK_SECTORS;
	
  	if(ofs > mtdInfo.size - mtdInfo.erasesize)
  	{
    		AS_MSG_LOG(AS_LOG_ERR, "%x is beyond device size %x",ofs,(unsigned int)(mtdInfo.size - mtdInfo.erasesize));
    		exit(1);
  	}

  	if (num_sectors == -1) 
	{
	  	num_sectors = mtdInfo.size/mtdInfo.erasesize;
  	}
  	else 
	{
	  	if(num_sectors > mtdInfo.size/mtdInfo.erasesize)
	  	{
	    		AS_MSG_LOG(AS_LOG_ERR, "%d are too many sectors, device only has %d",num_sectors,(int)(mtdInfo.size/mtdInfo.erasesize));
	    		return (EXIT_FAILURE);
	  	}
  	}

 	mtdLockInfo.start = ofs;
  	mtdLockInfo.length = num_sectors * mtdInfo.erasesize;
  	if(ioctl(dev_fd, MEMLOCK, &mtdLockInfo))
  	{
    		AS_MSG_LOG(AS_LOG_ERR, "Could not lock MTD device : %s", strerror(errno) );
    		close(dev_fd);
    		return (EXIT_FAILURE);
  	}
	
   	return EXIT_SUCCESS;
}


static int __as_flash_read(void *data, int length)
{
	int fd;
	struct mtd_info_user mtd;
	ssize_t result;

	fd = safe_open (FLASH_SERIAL_DEVICE,O_SYNC | O_RDWR);
   	if (ioctl(fd, MEMGETINFO, &mtd) < 0)
	{
		AS_MSG_LOG(AS_LOG_ERR, "This doesn't seem to be a valid MTD flash device : %s", strerror(errno) );
		close(fd);
		return (EXIT_FAILURE);
	}

   	safe_rewind(fd, FLASH_SERIAL_OFFSET, FLASH_SERIAL_DEVICE);

	result = read (fd, data, length);
	if(length != result)
	{
		if (result < 0)
		{
			AS_MSG_LOG(AS_LOG_ERR,"While reading data from serialno failure :%s", strerror(errno) );
			return (EXIT_FAILURE);
		}
		
		AS_MSG_LOG(AS_LOG_WARNING, "Short read count returned while reading from serialno failure");
		return (EXIT_FAILURE);
	}

	return result;
}

static int __as_flash_write(void *data, int length)
{
   	int					i, flags = FLAG_VERBOSE;//FLAG_NONE;
   	size_t				result,written;
	int					regcount;
	int					dev_fd;
	unsigned char			*buf;
   	struct mtd_info_user 	mtd;
  	struct erase_info_user	mtdLockInfo;

   	/* get some info about the flash device */
   	dev_fd = safe_open (FLASH_SERIAL_DEVICE,O_SYNC | O_RDWR);
   	if (ioctl (dev_fd, MEMGETINFO, &mtd) < 0)
	{
		AS_MSG_LOG(AS_LOG_INFO, "ioctl(): %m");
		AS_MSG_LOG(AS_LOG_ERR,"This doesn't seem to be a valid MTD flash device!");
		close(dev_fd);
		return (EXIT_FAILURE);
	}

  	mtdLockInfo.start = 0;
  	mtdLockInfo.length = mtd.size;
  	if(ioctl(dev_fd, MEMUNLOCK, &mtdLockInfo))
  	{
    		AS_MSG_LOG(AS_LOG_ERR,"Could not unlock MTD device %s : %s", FLASH_SERIAL_DEVICE, strerror(errno) );
    		close(dev_fd);
    		return(EXIT_FAILURE);
  	}

   	/*****************************************************
	* erase enough blocks so that we can write the file *
	*****************************************************/
	if (ioctl(dev_fd, MEMGETREGIONCOUNT, &regcount) == 0)
	{
		if(regcount == 0)
		{
			i = __as_flash_non_region_erase(dev_fd, FLASH_SERIAL_OFFSET, 1, 0);
		}
		else
		{
			i = __as_flash_region_erase(dev_fd, FLASH_SERIAL_OFFSET, 1, 0, regcount);
		}
	}

   	/* write the entire file to flash */
   	safe_rewind(dev_fd, FLASH_SERIAL_OFFSET, FLASH_SERIAL_DEVICE);

	/* write to device */
	result = write (dev_fd, data, length );
	if ( length != result)
	{
		if (flags & FLAG_VERBOSE) 
			AS_MSG_LOG(AS_LOG_INFO,"\n");
		
		if (result < 0)
		{
			AS_MSG_LOG(AS_LOG_ERR,
				"While writing data to 0x%.8x-0x%.8x on %s: %m",
					written,
						written + SERIAL_BUFSIZE,
							FLASH_SERIAL_DEVICE);
			return (EXIT_FAILURE);
		}
		
		return (EXIT_FAILURE);
	}
	
	AS_MSG_DEBUG(AS_LOG_DEBUG,"Wrote %d bytes", result );

   	/* verify that flash == file data */
	buf = (unsigned char *)malloc(length);
	if(!buf)
	{
		close(dev_fd);
		return EXIT_SUCCESS;
	}
	
   	safe_rewind(dev_fd, FLASH_SERIAL_OFFSET, FLASH_SERIAL_DEVICE);
	safe_read( dev_fd, FLASH_SERIAL_DEVICE, buf, length, flags & FLAG_VERBOSE);

//	as_default_info_debug((AS_FLASH_INFO_T*)buf);
	
	if (memcmp (data,buf, length))
	{
		AS_MSG_LOG(AS_LOG_INFO,"Flash does not seem to match flash data want to write");
		close(dev_fd);
		return (EXIT_FAILURE);
	}

	result = __as_flash_lock(dev_fd);
	close(dev_fd);

	return result;
}

int	as_default_write(AS_FLASH_INFO_T *defaultInfo)
{
	AS_FLASH_INFO_T info;
	
	memset(&info, 0 , sizeof(AS_FLASH_INFO_T) );
	memcpy(&info, defaultInfo, sizeof(AS_FLASH_INFO_T) );
	
	info.magicStart[0] = AS_FLASH_MAGIC_START;
	info.magicStart[1] = AS_FLASH_MAGIC_START;
	info.magicEnd[0] = AS_FLASH_MAGIC_END;
	info.magicEnd[1] = AS_FLASH_MAGIC_END;

//	as_default_info_debug(&info);
	return __as_flash_write( &info, sizeof(AS_FLASH_INFO_T) );
}

int	as_default_read(AS_FLASH_INFO_T *info)
{
	int result;

	result = __as_flash_read(info, sizeof(AS_FLASH_INFO_T) );
	if(result != sizeof(AS_FLASH_INFO_T) )
	{
		AS_MSG_LOG(AS_LOG_ERR,"Flash read length error!Should be %d, but %d",sizeof(AS_FLASH_INFO_T), result );
		return -1;
	}
	
	if(info->magicStart[0]!= AS_FLASH_MAGIC_START ||
		info->magicStart[1]!= AS_FLASH_MAGIC_START ||
		info->magicEnd[0]!= AS_FLASH_MAGIC_END ||
		info->magicEnd[1]!= AS_FLASH_MAGIC_END )
	{
		AS_MSG_LOG(AS_LOG_ERR, "Flash Data without validate Magic, so Ignored");
		return -2;
	}

#if 0
	char	*p;
	printf("read Factory Default : (%p)\n", info);
	as_default_info_debug(info);
	p = (char *)&info;
	for(i=0; i<  sizeof(AS_FLASH_INFO_T) ; i++)
	{
		*p = toascii(*p);
		p++;
	}
#endif

	return 0;
}

int as_read_serial_no(unsigned char *read_serial_no, int length)
{
	AS_FLASH_INFO_T info;
	if(as_default_read( &info) )
	{
		AS_MSG_LOG(AS_LOG_ERR,"read Factory Default failed");
		return -1;
	}

	strncpy(read_serial_no, info.serialNo, (length>strlen(info.serialNo))?strlen(info.serialNo):length );
	return 0;
}

int as_write_serial_no (unsigned char *serNO)
{
	AS_FLASH_INFO_T info, *p;
	if(as_default_read( &info) )
	{
		strncpy(factoryDefault.serialNo, serNO, (AS_FLASH_ITEM_LENGTH> strlen(serNO))?strlen(serNO):AS_FLASH_ITEM_LENGTH );
		p = &factoryDefault;
	}
	else
	{
		strncpy(info.serialNo, serNO, (AS_FLASH_ITEM_LENGTH> strlen(serNO))?strlen(serNO):AS_FLASH_ITEM_LENGTH );
		p = &info;
	}
	
	return as_default_write(p);
}

void as_default_info_debug(AS_FLASH_INFO_T *info)
{
	AS_MSG_LOG(AS_LOG_INFO,"\nBegin Magic \t: 0x%x:0x%x", info->magicStart[0], info->magicStart[1] );
	AS_MSG_LOG(AS_LOG_INFO,"Device ID \t: %s", info->serialNo );
	AS_MSG_LOG(AS_LOG_INFO,"User Name \t: %s", info->userName);
	AS_MSG_LOG(AS_LOG_INFO,"Password \t: %s", info->passwd );
	AS_MSG_LOG(AS_LOG_INFO,"WAN IP \t\t: %s", info->wanIp );
	AS_MSG_LOG(AS_LOG_INFO,"WAN Mask \t: %s", info->wanMask );
	AS_MSG_LOG(AS_LOG_INFO,"WAN MAC \t: %s", info->wanMac );
	AS_MSG_LOG(AS_LOG_INFO,"LAN IP \t\t: %s", info->lanIp );
	AS_MSG_LOG(AS_LOG_INFO,"LAN Mask \t: %s", info->lanMask );
	AS_MSG_LOG(AS_LOG_INFO,"LAN MAC \t: %s", info->lanMac);
	AS_MSG_LOG(AS_LOG_INFO,"End Magic \t: 0x%x:0x%x\n", info->magicEnd[0], info->magicEnd[1] );
}

