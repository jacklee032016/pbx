#include "as_misc_dev.h"

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
#include<ctype.h>


#define erase_start 131072
#define dev_write_serial_no	"/dev/mtd0"	

#define LOCK_START 0
#define LOCK_SECTORS 3

typedef int bool;
#define true 1
#define false 0

#define EXIT_FAILURE 1
#define EXIT_SUCCESS 0

/* for debugging purposes only */
#ifdef DEBUG
#undef DEBUG
#define DEBUG(fmt,args...) { log_printf (LOG_ERROR,"%d: ",__LINE__); log_printf (LOG_ERROR,fmt,## args); }
#else
#undef DEBUG
#define DEBUG(fmt,args...)
#endif

#define KB(x) ((x) / 1024)
#define PERCENTAGE(x,total) (((x) * 100) / (total))

/* size of read/write buffer */
#define BUFSIZE (10 * 1024)
#define SERIAL_BUFSIZE 20

/* cmd-line flags */
#define FLAG_NONE		0x00
#define FLAG_VERBOSE	0x01
#define FLAG_HELP		0x02
#define FLAG_FILENAME	0x04
#define FLAG_DEVICE		0x08

/* error levels */
#define LOG_NORMAL	1
#define LOG_ERROR	2

static void log_printf (int level,const char *fmt, ...)
{
   	FILE *fp = level == LOG_NORMAL ? stdout : stderr;
   	va_list ap;
   	va_start (ap,fmt);
   	vfprintf (fp,fmt,ap);
   	va_end (ap);
   	fflush (fp);
}

static int safe_open (const char *pathname,int flags)
{
   	int fd;

   	fd = open (pathname,flags);
   	if (fd < 0)
	{
		log_printf (LOG_ERROR,"While trying to open %s",pathname);
		if (flags & O_RDWR)
		log_printf (LOG_ERROR," for read/write access");
		else if (flags & O_RDONLY)
		log_printf (LOG_ERROR," for read access");
		else if (flags & O_WRONLY)
		log_printf (LOG_ERROR," for write access");
		log_printf (LOG_ERROR,": %m\n");
		exit (EXIT_FAILURE);
	}
	return (fd);
}

static void safe_read (int fd,const char *filename,void *buf,size_t count,bool verbose)
{
   	ssize_t result;

   	result = read (fd,buf,count);
   	if (count != result)
	{
		if (verbose) log_printf (LOG_NORMAL,"\n");
		if (result < 0)
		{
			log_printf (LOG_ERROR,"While reading data from %s: %m\n",filename);
			exit (EXIT_FAILURE);
		}
		log_printf (LOG_ERROR,"Short read count returned while reading from %s\n",filename);
		exit (EXIT_FAILURE);
	}
}

static void safe_rewind (int fd,const char *filename)
{
   	if (lseek (fd,131072L,SEEK_SET) < 0)
	{
		log_printf (LOG_ERROR,"While seeking to start of %s: %m\n",filename);
		exit (EXIT_FAILURE);
	}
}

/******************************************************************************/
static int dev_fd = -1;

static int region_erase(int Fd, int start, int count, int unlock, int regcount)
{
	int i, j;
	region_info_t * reginfo;

	reginfo = calloc(regcount, sizeof(region_info_t));

	for(i = 0; i < regcount; i++)
	{
		reginfo[i].regionindex = i;
		if(ioctl(Fd,MEMGETREGIONINFO,&(reginfo[i])) != 0)
			return 8;
		else
			printf("Region %d is at %d of %d sector and with sector "
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
		printf("Starting offset %x not within chip.\n", start);
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
				perror("\nMTD Unlock failure");
				close(Fd);
				return 8;
			}
		}
	
		fflush(stdout);
		if(ioctl(Fd, MEMERASE, &erase) != 0)
		{
			perror("\nMTD Erase failure");
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

static int non_region_erase(int Fd, int start, int count, int unlock)
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
					perror("\nMTD Unlock failure");
					close(Fd);
					return 8;
				}
			}

			if (ioctl(Fd,MEMERASE,&erase) != 0)
			{      
				perror("\nMTD Erase failure");
				close(Fd);
				return 8;
			}
			erase.start += meminfo.erasesize;
		}
	}
	return 0;
}


int as_read_serial_no(unsigned char *read_serial_no)
{
	int fd;
	int i;
	char dest[SERIAL_BUFSIZE];
	struct mtd_info_user mtd;
	
	fd = safe_open (dev_write_serial_no,O_SYNC | O_RDWR);
   	if (ioctl (fd,MEMGETINFO,&mtd) < 0)
	{
		printf ("This doesn't seem to be a valid MTD flash device\n");
		return (EXIT_FAILURE);
	}

	if (lseek (fd,131072L,SEEK_SET) < 0)
	{
		printf ("While seeking to start address of serialno failure\n");
		return (EXIT_FAILURE);
	}

	/* read from device */
	ssize_t result;
   	result = read (fd,dest,SERIAL_BUFSIZE);
   	if (SERIAL_BUFSIZE != result)
	{
		if (result < 0)
		{
			printf ("While reading data from serialno failure\n");
			return (EXIT_FAILURE);
		}
		printf ("Short read count returned while reading from serialno failure\n");
		return (EXIT_FAILURE);
	}
	for(i=0;i<SERIAL_BUFSIZE;i++)
	{
		*(read_serial_no+i)=toascii(dest[i]);
	}
	return 0;
}

int as_write_serial_no (unsigned char *serNO)
{
   	const char *device = NULL;
   	int i,flags = FLAG_NONE;
   	size_t result,size,written;
   	struct mtd_info_user mtd;
   	struct stat filestat;
   	struct mtd_info_user mtdInfo;
  	struct erase_info_user mtdLockInfo;
   	unsigned char dest[BUFSIZE];
	
	if(serNO==NULL)
	{
		printf("error:  please enter 422 serial NO as (#write_serial_no ********)\n");
		return (EXIT_FAILURE);
	}
	
	for(i=0;i<20;i++)
	{
		if(*(serNO+i) == '\0')
		{
			printf("error:  you enter 422 serial NO under the 20B\n");
			return (EXIT_FAILURE);
		}
	}

   	/* get some info about the flash device */
   	dev_fd = safe_open (dev_write_serial_no,O_SYNC | O_RDWR);
   	if (ioctl (dev_fd,MEMGETINFO,&mtd) < 0)
	{
		DEBUG("ioctl(): %m\n");
		log_printf (LOG_ERROR,"This doesn't seem to be a valid MTD flash device!\n");
		return (EXIT_FAILURE);
	}


   	/********************************
	unlock flash block1 at 0x20000-0x2ffff
	**********************************/

  	if(ioctl(dev_fd, MEMGETINFO, &mtdInfo))
  	{
    		fprintf(stderr, "Could not get MTD device info from \n");
    		close(dev_fd);
    		return (EXIT_FAILURE);
  	}


  	mtdLockInfo.start = 0;
  	mtdLockInfo.length = mtdInfo.size;
  	if(ioctl(dev_fd, MEMUNLOCK, &mtdLockInfo))
  	{
    		fprintf(stderr, "Could not unlock MTD device: %s\n", dev_write_serial_no);
    		close(dev_fd);
    		return(EXIT_FAILURE);
  	}

   	/*****************************************************
	* erase enough blocks so that we can write the file *
	*****************************************************/
	int regcount;
	if (ioctl(dev_fd,MEMGETREGIONCOUNT,&regcount) == 0)
	{
		if(regcount == 0)
		{
			i = non_region_erase(dev_fd, erase_start, 1, 0);
		}
		else
		{
			i = region_erase(dev_fd, erase_start, 1, 0, regcount);
		}
	}

   	/**********************************
	* write the entire file to flash *
	**********************************/
	
   	safe_rewind(dev_fd,device);

   	if (flags & FLAG_VERBOSE) log_printf (LOG_NORMAL,"Writing data: 0k/%luk (0%%)",KB (filestat.st_size));
   	size = SERIAL_BUFSIZE;
   	i = BUFSIZE;
   	written = 0;

   	while (size)
	{
		if (flags & FLAG_VERBOSE)
		  	log_printf (LOG_NORMAL,"\rWriting data: %dk/%luk (%lu%%)",
				KB (written + SERIAL_BUFSIZE),
				  	KB (filestat.st_size),
				  		PERCENTAGE (written + i,filestat.st_size));

		/* write to device */
		

		result = write (dev_fd,serNO,SERIAL_BUFSIZE);
		if (SERIAL_BUFSIZE != result)
		{
			if (flags & FLAG_VERBOSE) log_printf (LOG_NORMAL,"\n");
			if (result < 0)
			{
				log_printf (LOG_ERROR,
					"While writing data to 0x%.8x-0x%.8x on %s: %m\n",
						written,
							written + SERIAL_BUFSIZE,
								device);
				return (EXIT_FAILURE);
			}
			log_printf (LOG_ERROR,
				"Short write count returned while writing to x%.8x-0x%.8x on %s: %d/%lu bytes written to flash\n",
					written,
						written + SERIAL_BUFSIZE,
							device,
								written + result,
									filestat.st_size);
			return (EXIT_FAILURE);
		}

		written += SERIAL_BUFSIZE;
		size -= SERIAL_BUFSIZE;
	}
   	if (flags & FLAG_VERBOSE)
		log_printf (LOG_NORMAL,
			"\rWriting data: %luk/%luk (100%%)\n",
				KB (filestat.st_size),
					KB (filestat.st_size));
   	DEBUG("Wrote %d / %luk bytes\n",written,filestat.st_size);

   	/**********************************
	* verify that flash == file data *
	**********************************/

   	safe_rewind (dev_fd,device);
   	size = SERIAL_BUFSIZE;
   	i = SERIAL_BUFSIZE;
   	written = 0;
   	if (flags & FLAG_VERBOSE) log_printf (LOG_NORMAL,"Verifying data: 0k/%luk (0%%)",KB (filestat.st_size));
   	while (size)
	{
		if (flags & FLAG_VERBOSE)
			log_printf (LOG_NORMAL,
				"\rVerifying data: %dk/%luk (%lu%%)",
					KB (written + SERIAL_BUFSIZE),
						KB (filestat.st_size),
					  		PERCENTAGE (written +SERIAL_BUFSIZE,filestat.st_size));

		/* read from device */

		safe_read (dev_fd,device,dest,SERIAL_BUFSIZE,flags & FLAG_VERBOSE);

		/* compare buffers */

		if (memcmp (serNO,dest,SERIAL_BUFSIZE))
		{
			log_printf (LOG_ERROR,
				"File does not seem to match flash data. First mismatch at 0x%.8x-0x%.8x\n",
					written,
						written + SERIAL_BUFSIZE);
			return (EXIT_FAILURE);
		}

		written += SERIAL_BUFSIZE;
		size -= SERIAL_BUFSIZE;
	}
   	if (flags & FLAG_VERBOSE)
	log_printf (LOG_NORMAL,
		"\rVerifying data: %luk/%luk (100%%)\n",
			KB (filestat.st_size),
				KB (filestat.st_size));
   	DEBUG("Verified %d / %luk bytes\n",written,filestat.st_size);
	/***************************
	*  lock
	***************************/
  	int num_sectors;
  	int ofs;

  	if(ioctl(dev_fd, MEMGETINFO, &mtdInfo))
  	{
    		fprintf(stderr, "Could not get MTD device info \n");
    		close(dev_fd);
    		return (EXIT_FAILURE);
  	}

	ofs = LOCK_START;
	num_sectors = LOCK_SECTORS;
	
  	if(ofs > mtdInfo.size - mtdInfo.erasesize)
  	{
    		fprintf(stderr, "%x is beyond device size %x\n",ofs,(unsigned int)(mtdInfo.size - mtdInfo.erasesize));
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
	    		fprintf(stderr, "%d are too many sectors, device only has %d\n",num_sectors,(int)(mtdInfo.size/mtdInfo.erasesize));
	    		return (EXIT_FAILURE);
	  	}
  	}

 	mtdLockInfo.start = ofs;
  	mtdLockInfo.length = num_sectors * mtdInfo.erasesize;
  	if(ioctl(dev_fd, MEMLOCK, &mtdLockInfo))
  	{
    		fprintf(stderr, "Could not lock MTD device\n");
    		close(dev_fd);
    		return (EXIT_FAILURE);
  	}
	
   	return EXIT_SUCCESS;
}

