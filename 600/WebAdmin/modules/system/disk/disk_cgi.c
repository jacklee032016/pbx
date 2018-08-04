/*
 * $Id: disk_cgi.c,v 1.5 2007/09/06 09:19:59 lizhijie Exp $
*/
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "cgi_lib.h"
#include "as_version.h"
#include "as_disk_info.h"

llist			diskCgiVariables;
as_disk_t 	*disks;
llist 			hdDiskCfgs;


static int __disk_cgi_init()
{
	as_disk_t *disk;
	cgi_html_header();
	list_create(&diskCgiVariables);
	read_cgi_input(&diskCgiVariables, NULL, NULL);

	list_create(&hdDiskCfgs);
	if(readconfig(HD_DISK_CONFIG_FILE, ITEM_EQU,NULL, &hdDiskCfgs))
	{
		cgi_error_page(NULL, gettext(CGI_STR_ERROR), gettext(CGI_STR_WITHOUT_CONF_FILE));
		return -1;
	}
	
	disks = as_cgi_get_disks();
	if(!disks )
	{
		cgi_error_page(NULL, gettext(CGI_STR_ERROR), gettext("No Disk is check in this system") );
		return -1;
	}

	disk = disks;
	while(disk)
	{
		if( (disk->hasRootFs && strstr(disk->name, DEVICE_HARD_DISK) )
			|| (disk->hasRootFs && !strstr(disk->name, DEVICE_USB_DISK) )  )
		{
			char		msg[1024];
			int		len = 0;
			len += CGI_SPRINTF(msg,len, "%s %s!", gettext("Hard Disk format can only be used"), gettext("when USB Disk as Root and HD Disk as odd mass storage device") );
			if( disk->hasRootFs && strstr(disk->name, DEVICE_HARD_DISK) )
				len += CGI_SPRINTF(msg,len, "<br>%s!", gettext("Now Hard Disk is used as Root Device") );
			if( disk->hasRootFs && !strstr(disk->name, DEVICE_USB_DISK)  )
				len += CGI_SPRINTF(msg,len, "<br>%s!", gettext("Now USB Disk is not used as Root Device") );

			cgi_error_page(NULL, gettext(CGI_STR_ERROR), msg );
			return -1;
		}

		disk =disk->next;
	}
	
	return 0;
}


static char *__disk_add_buttons(as_disk_t *disk)
{
	char buf[4096];
	int length = 0;
	char target[128];

	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"center\">\r\n");

	CGI_SPRINTF(target,0, "%s?%s=%s&%s=%s", WEB_URL_DISK_MGMT, CGI_KEYWORD_OPERATOR, DISK_OP_DISK_FDISK, 
		DISK_KEYWORD_DISK, disk->name);
	if(disk->parts)
	{
		as_partition_t *part = disk->parts;
		int isRoot = 0;
		char msg[256];

		while(part)
		{
			if(!strcasecmp(part->mountPoint, "/") )
			{
				isRoot = 1;
				break;
			}
			part = part->next;
		}
		
		if(! isRoot )
		{
			CGI_SPRINTF(msg,0, "%s %s %s. %s", gettext("In order to fdisk"), disk->descrition, gettext("all partitions will be deleted"), gettext(CGI_STR_ARE_YOU_SURE) );
			length += CGI_SPRINTF(buf,length, cgi_button_alert(gettext("Fdisk"), target, msg ) );
		}	
		
	}	
	else
		length += CGI_SPRINTF(buf,length, cgi_button(	gettext("Fdisk"), target) );

	length += CGI_SPRINTF(buf,length, "\r\n");

	length += CGI_SPRINTF(buf,length,  cgi_help_button(CGI_HELP_MSG_DISK_MGMT) );

	length += CGI_SPRINTF(buf,length,  "</TD></TR></form>\r\n");

	return strdup(buf);
}


int main(int argc, char *argv[])
{
	char 			*cmd;
	char 			*_index;
	as_disk_t 		*disk = NULL;
	char 			*_partIndex;
	
	if(__disk_cgi_init() )
		return 0;
	
	cmd = GET_VALUE( &diskCgiVariables,  CGI_KEYWORD_OPERATOR);
	_index = GET_VALUE( &diskCgiVariables,  DISK_KEYWORD_DISK );
	_partIndex = GET_VALUE( &diskCgiVariables,  DISK_KEYWORD_PART );
	
//	cgidebug("disk command is %s\n", cmd);
		
	if( !cmd || !strlen(cmd) || !strcasecmp(cmd, DISK_OP_DISK_LOOKUP) )
	{
		
		cgi_info_page(gettext("Disk Info"), as_cgi_disk_disks_info(disks, WEB_URL_DISK_MGMT, DISK_OP_PART_INFO), "");
	}
	else if(!strcasecmp(cmd, DISK_OP_PART_INFO ) || 
		!strcasecmp(cmd, DISK_OP_PART_FORMAT ) ||!strcasecmp(cmd, DISK_OP_PART_FORMATING ) ||
		!strcasecmp(cmd, DISK_OP_DISK_FDISK)||!strcasecmp(cmd, DISK_OP_DISK_FDISKING)  )
	{
		char title[128];
		if(_index)
			disk = as_cgi_disk_find_disk(disks, _index );
		if(!_index || !disk)
		{
			cgi_error_page(NULL, gettext(CGI_STR_FAIL), gettext( "Hard Disk for Partition check is not found" ) );
			return -1;
		}

		if(!strcasecmp(cmd, DISK_OP_PART_INFO ) )
		{	
			CGI_SPRINTF(title,0, "'%s' %s", disk->descrition, gettext(CGI_STR_PARTITION_INFO));
			cgi_info_page(title, as_cgi_disk_parts_info(disk), __disk_add_buttons(disk) );
			return 0;
		}
		else if(!strcasecmp(cmd, DISK_OP_DISK_FDISK) ||!strcasecmp(cmd, DISK_OP_DISK_FDISKING))
		{	
			return sys_disk_fdisk(disk, cmd);
		}
#if 1
		return sys_disk_format( disk, cmd);
#else		
		if(_partIndex )
			part = as_cgi_disk_find_part(disk, atoi(_partIndex) );
		if(!_partIndex || !part)
		{
			cgi_error_page(NULL, gettext(CGI_STR_FAIL), gettext( "Partition for format is not found" ) );
			return -1;
		}

		return sys_partition_format( part);
#endif

	}	
	else if(!strcasecmp(cmd, DISK_OP_DISK_INSTALL) )
	{
		return sys_disk_install(disks);
	}
	else if(!strcasecmp(cmd, DISK_OP_MGMT) )
	{
		return sys_disk_management(disks, &hdDiskCfgs);
	}
	else if(!strcasecmp(cmd, DISK_OP_MGMT_MOUNT) )
	{
		return sys_disk_mgmt_remount(disks, &hdDiskCfgs);
	}
	else if(!strcasecmp(cmd, DISK_OP_MGMT_UNMOUNT) )
	{
		return sys_disk_mgmt_unmount(disks, &hdDiskCfgs);
	}
	else
	{
		cgi_error_page(NULL, gettext(CGI_STR_FAIL), gettext( "No command for Hard Disk check" ) );
		return -1;
	}

	return 0;
}


