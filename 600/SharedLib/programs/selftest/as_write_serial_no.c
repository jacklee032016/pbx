/*
* $Id: as_write_serial_no.c,v 1.6 2007/03/16 19:29:34 lizhijie Exp $
*/

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/mtd/mtd.h>

#include	<ctype.h>

#include "as_flash.h"


int as_write_serial_no (void )
{
static int dev_fd = -1;

   	const char *filename = NULL,*device = NULL;
   	int i,flags = FLAG_NONE;
   	size_t result,size,written;
   	struct mtd_info_user mtd;
   	struct stat filestat;
   	struct mtd_info_user mtdInfo;
  	struct erase_info_user mtdLockInfo;
   	unsigned char dest[BUFSIZE];
	
	unsigned char serial_no[SERIAL_BUFSIZE];
	unsigned char *serNO;
	
	serNO = serial_no;
	printf("please enter 422' serial NO [20B] :\t");
	scanf("%s", serNO);

	if(serNO==NULL)
	{
		printf("error:  please enter 422 serial NO as (#write_serial_no ********)");
		exit (EXIT_FAILURE);
	}

	for(i=0;i<20;i++)
	{
		if(*(serNO+i) == '\0')
		{
			printf("error:  you enter 422 serial NO under the 20B");
			exit (EXIT_FAILURE);
		}
	}

   	/* get some info about the flash device */
   	dev_fd = safe_open (FLASH_SERIAL_DEVICE,O_SYNC | O_RDWR);
   	if (ioctl (dev_fd,MEMGETINFO,&mtd) < 0)
	{
		AS_MSG_DEBUG(AS_LOG_DEBUG, "ioctl(): %m");
		AS_MSG_LOG(AS_LOG_ERR,"This doesn't seem to be a valid MTD flash device!");
		exit (EXIT_FAILURE);
	}

   	/********************************
	unlock flash block1 at 0x20000-0x2ffff
	**********************************/

  	if(ioctl(dev_fd, MEMGETINFO, &mtdInfo))
  	{
    		AS_MSG_LOG(AS_LOG_ERR, "Could not get MTD device info : %s", strerror(errno) );
    		close(dev_fd);
    		exit(1);
  	}

  	mtdLockInfo.start = 0;
  	mtdLockInfo.length = mtdInfo.size;
  	if(ioctl(dev_fd, MEMUNLOCK, &mtdLockInfo))
  	{
    		AS_MSG_LOG(AS_LOG_ERR,"Could not unlock MTD device: %s", FLASH_SERIAL_DEVICE);
    		close(dev_fd);
    		exit(1);
  	}

   	/* does it fit into the device/partition? */
   	if (filestat.st_size > mtd.size)
	{
		AS_MSG_LOG(AS_LOG_ERR,"%s won't fit into %s!",filename,device);
		exit (EXIT_FAILURE);
	}

   	/*****************************************************
	* erase enough blocks so that we can write the file *
	*****************************************************/
	int regcount;
	if (ioctl(dev_fd,MEMGETREGIONCOUNT,&regcount) == 0)
	{
		if(regcount == 0)
		{
			i = non_region_erase(dev_fd, FLASH_SERIAL_OFFSET, 1, 0);
		}
		else
		{
			i = region_erase(dev_fd, FLASH_SERIAL_OFFSET, 1, 0, regcount);
		}
	}

   	/**********************************
	* write the entire file to flash *
	**********************************/
	
   	safe_rewind(dev_fd,device);

   	if (flags & FLAG_VERBOSE)
		AS_MSG_LOG(AS_LOG_INFO,,"Writing data: 0k/%luk (0%%)",KB (filestat.st_size));
	
   	size = SERIAL_BUFSIZE;
   	i = BUFSIZE;
   	written = 0;

   	while (size)
	{
		if (flags & FLAG_VERBOSE)
		  	AS_MSG_LOG(AS_LOG_INFO,,"\rWriting data: %dk/%luk (%lu%%)",
				KB (written + SERIAL_BUFSIZE),
				  	KB (filestat.st_size),
				  		PERCENTAGE (written + i,filestat.st_size));

		/* write to device */

		result = write (dev_fd,serial_no,SERIAL_BUFSIZE);
		if (SERIAL_BUFSIZE != result)
		{
			if (flags & FLAG_VERBOSE)
				AS_MSG_LOG(AS_LOG_INFO,"\n");
			if (result < 0)
			{
				AS_MSG_LOG(AS_LOG_ERR,
					"While writing data to 0x%.8x-0x%.8x on %s: %m",
						written,
							written + SERIAL_BUFSIZE,
								device);
				exit (EXIT_FAILURE);
			}
			
			AS_MSG_LOG(AS_LOG_INFO,
				"Short write count returned while writing to x%.8x-0x%.8x on %s: %d/%lu bytes written to flash",
					written,
						written + SERIAL_BUFSIZE,
							device,
								written + result,
									filestat.st_size);
			exit (EXIT_FAILURE);
		}

		written += SERIAL_BUFSIZE;
		size -= SERIAL_BUFSIZE;
	}
   	if (flags & FLAG_VERBOSE)
		AS_MSG_LOG(AS_LOG_INFO,
			"\rWriting data: %luk/%luk (100%%)",
				KB (filestat.st_size),
					KB (filestat.st_size));
   	AS_MSG_LOG(AS_LOG_INFO, "Wrote %d / %luk bytes",written,filestat.st_size);

   	/**********************************
	* verify that flash == file data *
	**********************************/

   	safe_rewind (dev_fd,device);
   	size = SERIAL_BUFSIZE;
   	i = SERIAL_BUFSIZE;
   	written = 0;
   	if (flags & FLAG_VERBOSE)
		AS_MSG_LOG(AS_LOG_INFO,"Verifying data: 0k/%luk (0%%)",KB (filestat.st_size));
	
   	while (size)
	{
		if (flags & FLAG_VERBOSE)
			AS_MSG_LOG(AS_LOG_INFO,
				"\rVerifying data: %dk/%luk (%lu%%)",
					KB (written + SERIAL_BUFSIZE),
						KB (filestat.st_size),
					  		PERCENTAGE (written +SERIAL_BUFSIZE,filestat.st_size));

		/* read from device */

		safe_read (dev_fd,device,dest,SERIAL_BUFSIZE,flags & FLAG_VERBOSE);

		/* compare buffers */

		if (memcmp (serial_no,dest,SERIAL_BUFSIZE))
		{
			AS_MSG_LOG(AS_LOG_ERR,
				"File does not seem to match flash data. First mismatch at 0x%.8x-0x%.8x",
					written,
						written + SERIAL_BUFSIZE);
			exit (EXIT_FAILURE);
		}

		written += SERIAL_BUFSIZE;
		size -= SERIAL_BUFSIZE;
	}
	
   	if (flags & FLAG_VERBOSE)
		AS_MSG_LOG(AS_LOG_INFO, "\rVerifying data: %luk/%luk (100%%)",
			KB (filestat.st_size), KB (filestat.st_size));
	AS_MSG_LOG(AS_LOG_INFO,"Verified %d / %luk bytes",written,filestat.st_size);

	AS_MSG_LOG(AS_LOG_INFO,"success to update serial-No ,");
	AS_MSG_LOG(AS_LOG_INFO,"422 serial NO is :");
	for(i=0;i<SERIAL_BUFSIZE;i++)
	{
		AS_MSG_LOG(AS_LOG_ERR, "%d",*(dest+i)-48);
	}
	AS_MSG_LOG(AS_LOG_ERR, "\n");
	
   	exit (EXIT_SUCCESS);
}

