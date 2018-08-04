/*
 * $Id: as_disk_info.c,v 1.6 2007/03/13 20:15:01 lizhijie Exp $
 */
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "cgi_lib.h"
#include "as_disk_info.h"

static part_info_t	__flashdisk_parts[] =
{
	{
		PART_SIZE_PERCENT,
		100,								/* 100% for USB Flash Disk */
		DISK_FILESYSTEM_EXT3,
		HDDISK_PART_MOUNT_POINT_1_ROOT,	
		""
	},
	{
		PART_SIZE_UNKWNOW,
		-1,
		NULL,
		NULL,
		NULL
	}
};

static part_info_t	__cfcard_disk_parts[] =
{
	{
		PART_SIZE_PERCENT,
		100,								/* 100% for USB Flash Disk */
		DISK_FILESYSTEM_REISERFS,
		HDDISK_PART_MOUNT_POINT_1_ROOT,	
		""
	},
	{
		PART_SIZE_UNKWNOW,
		-1,
		NULL,
		NULL,
		NULL
	}
};


static part_info_t	__hddisk_parts[] =
{
	{
		PART_SIZE_ABSOLUTE,
		512,								/* about 512 MB */
		DISK_FILESYSTEM_REISERFS,
		HDDISK_PART_MOUNT_POINT_1_ROOT,	
		""
	},
	{/* partition for file server */
		PART_SIZE_PERCENT,
		30,
		DISK_FILESYSTEM_REISERFS,
		HDDISK_PART_MOUNT_POINT_2_SPOOL,	
		SUBDIRS_OF_SPOOL,
	},
	{/* partition for other servers such as printer, mail and www */
		PART_SIZE_PERCENT,
		100,								/* Last Partition used up all disk space */
		DISK_FILESYSTEM_REISERFS,
		HDDISK_PART_MOUNT_POINT_3_FILESERVER,	
		SUBDIRS_OF_FILE_SERVER,
	},
	{
		PART_SIZE_UNKWNOW,
		-1,
		NULL,
		NULL,
		NULL
	}
};

unsigned char  *as_cgi_disk_info(as_disk_t *disk)
{
	unsigned char info[1024];
	int pos = 0;

	pos += CGI_SPRINTF(info,pos, "<TR><TD align=\"center\" colspan=3 bgcolor=\"#cccccc\"><strong>%s : %s</strong></TD></TR>\n", 
		gettext("Disk Info"), disk->descrition);
	pos += CGI_SPRINTF(info,pos, "<TR><TD align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD><TD align=\"center\">%d %s</TD><TD align=\"center\">%s bytes</TD></TR>\n", 
		gettext("Capacity"), disk->capacity, disk->unit, disk->capacityB);
	
	return strdup(info);
}


int	as_cgi_partition_check_mount(as_partition_t *part)
{
	FILE			*fp;
	char			line[LINE_MAX_LENGTH];
	char 		t1[128],t2[128],t3[128],t4[128],t5[128],t6[128];

	if ((fp = fopen(DISK_MOUNT_TABLE, "r") ) == (FILE *)NULL) 
	{
		cgidebug("open %s file failed\n", DISK_MOUNT_TABLE);
		return -1;
	}

	while( fgets(line, LINE_MAX_LENGTH, fp) != NULL )
	{
		if( sscanf(line,"%s %s %s %s %s %s", t1, t2, t3, t4, t5,t6) == 6)
		{
			if( strstr(t1, part->name) )
			{
				part->status = AS_PART_MOUNTED;
				CGI_SPRINTF(part->mountPoint,0, "%s", t2);
				CGI_SPRINTF(part->fileSystemType,0, "%s", t3);
				if(!strcasecmp(part->mountPoint, "/" ) )
				{
					part->isRootFs = 1;
					part->disk->hasRootFs = 1;
				}

				fclose(fp);

				return 1;
			}
		}	
	}

	fclose(fp);
	
	return 0;
}


static void __as_cgi_get_partitions(as_disk_t *disk)
{
	FILE			*fp;
	char	 		cmd[256];
	char			line[1024];
	char			t1[32], t2[32],t3[32],t4[32],t5[32],t6[32];
	int 			index = 0;
	as_partition_t	*tmp = NULL, *pre = NULL;

	/* command such "echo -e "p\nq" | fdisk /dev/sda | grep /dev/sda " */
	CGI_SPRINTF(cmd, 0, "echo -e \"p\nq\" | fdisk %s |grep %s > %s",disk->name, disk->name,  DISK_TEMP_FILE);
	system(cmd);
	cgidebug("get parition command is '%s'\n", cmd);
	
	if ((fp = fopen(DISK_TEMP_FILE, "r") ) == (FILE *)NULL) 
	{
		cgidebug("open %s file failed when detect disk detail\n", DISK_TEMP_FILE);
		return;
	}

	while( fgets(line,LINE_MAX_LENGTH, fp) != NULL )
	{
		if( strlen(line)<1 )
			continue;
		
		if( !strncmp(line, disk->name, strlen(disk->name) ) )
		{
			index ++;
			tmp = (as_partition_t *)malloc(sizeof(as_partition_t) );
			if(tmp==NULL)
			{
				return;
			}
			memset(tmp, 0, sizeof(as_partition_t) );
			
			sscanf(line,"%s %s %s %s %s %s", t1, t2, t3, t4, t5,t6);
			tmp->index = index;
			CGI_SPRINTF(tmp->name,0, "%s", t1);
			tmp->startCylinder = atoi(t2);
			tmp->endCylinder = atoi(t3);
			CGI_SPRINTF(tmp->blocks,0, "%s", t4);
			tmp->id = atoi(t5);
			CGI_SPRINTF(tmp->system,0, "%s", t6);
			
			tmp->status = AS_PART_UNFORMAT;
			tmp->disk = disk;
			
			as_cgi_partition_check_mount(tmp);
			cgidebug("add part %s\n", tmp->name );
			if(disk->parts== NULL)
			{
				disk->parts = tmp;
				cgidebug("disk %s add part %s\n", disk->name, tmp->name );
			}

			if(pre )
			{
				pre->next = tmp;
			}
			pre = tmp;
			
		}
		else
		{
			continue;
		}

	}

	fclose(fp);
	remove(DISK_TEMP_FILE);
	
	return;
}


as_disk_t *as_cgi_get_disks()
{
	FILE			*fp;
	char	 		cmd[128];
	char			line[1024];
	as_disk_t		*_disks = NULL, *tmp =NULL,*pre= NULL;
	char			t1[32], t2[32],t3[32],t4[32],t5[32],t6[32];
	int 			index = 0;
	
	CGI_SPRINTF(cmd, 0, "/sbin/fdisk -l|grep Disk > %s", DISK_TEMP_FILE);
	system(cmd);
	
	if ((fp = fopen(DISK_TEMP_FILE, "r") ) == (FILE *)NULL) 
	{
		cgidebug("open %s file failed\n", DISK_TEMP_FILE);
		return NULL;
	}

	while( fgets(line,LINE_MAX_LENGTH, fp) != NULL )
	{
		if( sscanf(line,"%s %s %s %s %s %s", t1, t2, t3, t4, t5,t6) == 6)
		{
			tmp = (as_disk_t *)malloc(sizeof(as_disk_t) );
			if(tmp== NULL)
			{
				return NULL;
			}
			memset(tmp, 0, sizeof(as_disk_t) );
			
			t2[strlen(t2)-1] = 0;
			t4[strlen(t4)-1] = 0;
			if( strstr(t2, DEVICE_HARD_DISK) ) //strncasecmp(t2, DEVICE_HARD_DISK, sizeof(DEVICE_HARD_DISK) ) )
			{
				if(strstr(t4, "GB") )
				{
					tmp->status = AS_DISK_HARD_DISK;
					tmp->partConfig = __hddisk_parts;
					CGI_SPRINTF(tmp->descrition, 0, "%s(%s)", gettext("Hard Disk"), t2);
				}
				else
				{
					tmp->status = AS_DISK_CF_CARD_DISK;
					tmp->partConfig = __cfcard_disk_parts;
					CGI_SPRINTF(tmp->descrition, 0, "%s(%s)", gettext("CF Disk"), t2);
				}
			}
			else if( strstr(t2, DEVICE_USB_DISK) )// !strncasecmp(t2, DEVICE_USB_DISK, sizeof(DEVICE_USB_DISK) ) )
			{
				tmp->status = AS_DISK_USB_DISK;
				tmp->partConfig = __flashdisk_parts;
				CGI_SPRINTF(tmp->descrition,0, "%s(%s)", gettext("USB Disk"), t2);
			}
			else
			{
#if 0			
				tmp->status = AS_DISK_NOT_SUPPORT;
				CGI_SPRINTF(tmp->descrition, 0, "%s(%s)", gettext("ERROR Disk"), t2);
#else
				free(tmp);
				tmp = NULL;
				continue;
#endif
			}

			index++;
			tmp->index = index;
			CGI_SPRINTF(tmp->name,0, "%s", t2);
			tmp->capacity = atoi(t3);
			CGI_SPRINTF(tmp->unit, 0, "%s" , t4);
			CGI_SPRINTF(tmp->capacityB,0, "%s", t5);

			__as_cgi_get_partitions(tmp);
			
			if(_disks== NULL)
			{
				_disks = tmp;
			}

			if(pre )
			{
				pre->next = tmp;
			}
			pre = tmp;
		}	

		memset(line, 0 , sizeof(line) );
	}

	fclose(fp);
	remove(DISK_TEMP_FILE);
	
	return _disks;
}

#if 0
static int is_ide_cdrom_or_disk(const char *device)
{
	FILE *procf;
	char buf[100];
	struct stat statbuf;
	int is_ide = 0;

	/* No device was given explicitly, and we are trying some
	   likely things.  But opening /dev/hdc may produce errors like
	   "hdc: tray open or drive not ready"
	   if it happens to be a CD-ROM drive. It even happens that
	   the process hangs on the attempt to read a music CD.
	   So try to be careful. This only works since 2.1.73. */

	if (strncmp("/dev/hd", device, 7))
		return 0;

	snprintf(buf, sizeof(buf), "/proc/ide/%s/media", device+5);
	procf = fopen(buf, "r");
	if (procf != NULL && fgets(buf, sizeof(buf), procf) )
		is_ide = (!strncmp(buf, "cdrom", 5) || !strncmp(buf, "disk", 4));
	else
		/* Now when this proc file does not exist, skip the
		   device when it is read-only. */
		if (stat(device, &statbuf) == 0)
			is_ide = ((statbuf.st_mode & 0222) == 0);

	if (procf)
		fclose(procf);
	
	return is_ide;
}

static void try(const char *device, int user_specified)
{
	int gb;

	disk_device = device;
	if (setjmp(listingbuf))
		return;
	if (!user_specified)
		if (is_ide_cdrom_or_tape(device))
			return;
	if ((fd = open(disk_device, type_open)) >= 0) {
		gb = get_boot(try_only);
		if (gb > 0) {   /* I/O error */
			close(fd);
		} else if (gb < 0) { /* no DOS signature */
			list_disk_geometry();
			if (aix_label)
				return;
				fprintf(stderr,"Disk %s doesn't contain a valid partition table\n", device);
			close(fd);
		} else {
			close(fd);
			list_table(0);
		}
	} else {
		/* Ignore other errors, since we try IDE
		   and SCSI hard disks which may not be
		   installed on the system. */
		if (errno == EACCES) {
			fprintf(stderr, "Cannot open %s\n" , device);
			return;
		}
	}
}


static void tryprocpt(void)
{
	FILE *procpt;
	char line[100], ptname[100], devname[120], *s;
	int ma, mi, sz;

	procpt = fopen(DISK_PARTITIONS, "r");

	while(fgets(line, sizeof(line), procpt))
	{
		if (sscanf (line, " %d %d %d %[^\n ]",
			    &ma, &mi, &sz, ptname) != 4)
			continue;
		for (s = ptname; *s; s++);
		if (isdigit(s[-1]))
			continue;
		sprintf(devname, "/dev/%s", ptname);
		try(devname, 0);
	}

	fclose(procpt);

}
#endif


as_disk_t *as_cgi_disk_find_disk(as_disk_t *_disks,  char *name)
{
	as_disk_t *tmp = _disks;

	while(tmp)
	{
		if( !strcasecmp(tmp->name, name) ) // == index )
			return tmp;
		tmp = tmp->next;
	}
	return NULL;
}

as_partition_t *as_cgi_disk_find_part(as_disk_t *_disk,  int index)
{
	as_partition_t *tmp = _disk->parts;

	while(tmp)
	{
		if(tmp->index == index )
			return tmp;
		tmp = tmp->next;
	}
	return NULL;
}

char *as_cgi_disk_parts_info(as_disk_t *disk)
{
	as_partition_t  *current = disk->parts;
	char 	buf[4096];
	int 		length = 0;
	char 	target[128];
	if(!current)
	{
		CGI_SPRINTF(buf,0, "'%s' %s", disk->descrition, gettext("has no partitions, so fdisk it first!") );
		return strdup(buf);
	}
	
	while (current) 
	{
		char msg[256];

		CGI_SPRINTF(target,0, "%s?%s=%s&%s=%s&%s=%d", WEB_URL_DISK_MGMT, CGI_KEYWORD_OPERATOR, DISK_OP_PART_FORMAT, 
			DISK_KEYWORD_DISK, disk->name, DISK_KEYWORD_PART, current->index );
		length += CGI_SPRINTF(buf,length, "<TR><TD align=\"center\" bgcolor=\"#cccccc\" colspan=3><strong>%s--%d (%s)</strong></TD></TR>\n", 
			gettext(CGI_STR_PARTITION_INFO), current->index, current->name);

		if( strcasecmp(current->mountPoint, "/") )
		{
			CGI_SPRINTF(msg,0, "%s '%s' %s. %s", gettext("In order to format"), current->name, gettext("all data in this partition will be deleted"), gettext(CGI_STR_ARE_YOU_SURE) );

			length += CGI_SPRINTF(buf,length,  "<TR><TD align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD><TD align=\"center\">%d</TD><TD align=\"center\" rowspan=7>%s</TD></TR>\n", 
				gettext(CGI_STR_START_CYLINDER), current->startCylinder, cgi_button_alert( gettext("Format"), target, msg ) );
		}
		else
		{
			length += CGI_SPRINTF(buf,length, "<TR><TD align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD><TD align=\"center\">%d</TD><TD align=\"center\" rowspan=7></TD></TR>\n", 
				gettext(CGI_STR_START_CYLINDER), current->startCylinder );
		}
		length += CGI_SPRINTF(buf,length,  "<TR><TD align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD><TD align=\"center\">%d</TD></TR>\n", 
			gettext(CGI_STR_END_CYLINDER), current->endCylinder);
		length += CGI_SPRINTF(buf,length,  "<TR><TD align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD><TD align=\"center\">%s</TD></TR>\n", 
			gettext("Blocks"), current->blocks);
		length += CGI_SPRINTF(buf,length, "<TR><TD align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD><TD align=\"center\">%d</TD></TR>\n", 
			gettext(CGI_STR_ID), current->id);
		length += CGI_SPRINTF(buf,length,  "<TR><TD align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD><TD align=\"center\">%s</TD></TR>\n", 
			gettext("System"), current->system );
		length += CGI_SPRINTF(buf,length,  "<TR><TD align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD><TD align=\"center\">%s</TD></TR>\n", 
			gettext(CGI_STR_MOUNT_POINT), current->mountPoint );
		length += CGI_SPRINTF(buf,length,  "<TR><TD align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD><TD align=\"center\">%s</TD></TR>\n", 
			gettext("File System"), current->fileSystemType  );
		current = current->next;
	}

	return strdup(buf);
}


char *as_cgi_disk_disks_info(as_disk_t *disks, char *url , char *op)
{
	as_disk_t  *current = disks;
	char 	buf[4096];
	int 		length = 0;
	char 	target[128];

	while (current)
	{
		length += CGI_SPRINTF(buf,length, "%s", as_cgi_disk_info(current) );
		CGI_SPRINTF(target, 0, "%s?%s=%s&%s=%s", url, CGI_KEYWORD_OPERATOR, 
			op,DISK_KEYWORD_DISK, current->name );
		length += CGI_SPRINTF(buf,length,  "<TR><TD align=\"center\" colspan=3>%s</TD></TR><TR></TR>", cgi_button(gettext(CGI_STR_PARTITION_INFO), target)  );	
		current = current->next;
	}
	return strdup(buf);
}


