/*
 * $Id: disk_management.c,v 1.3 2007/09/06 09:19:59 lizhijie Exp $
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

#define	DISK_MGMT_TYPE_MOUNT		0
#define	DISK_MGMT_TYPE_UNMOUNT		1

#define	FORM_NAME		"formatForm"
static char *__cgi_unmount_buttons(as_disk_t *disk, llist *diskConfig,  int type)
{
	char 	buf[4096];
	char		msg[256];
	int 		length = 0;
	int		i = 0;
	as_partition_t *part = disk->parts;

	length += CGI_SPRINTF(buf, length,"<form name=\"%s\" method=\"post\" action=\"%s\">\n", FORM_NAME, WEB_URL_DISK_MGMT);
	length += CGI_SPRINTF(buf, length, "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_KEYWORD_OPERATOR, 
		(type==DISK_MGMT_TYPE_MOUNT)?DISK_OP_MGMT_MOUNT: DISK_OP_MGMT_UNMOUNT );
	length += CGI_SPRINTF(buf, length, "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", DISK_KEYWORD_DISK, disk->name);
	length += CGI_SPRINTF(buf, length, "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", DISK_KEYWORD_HD_STATUS, 
		(type==DISK_MGMT_TYPE_MOUNT)?DISK_OP_DISK_UNMOUNT:DISK_OP_DISK_MOUNT );

	length += CGI_SPRINTF(buf, length, "<tr><td colspan=2 align=\"center\" bgcolor=\"#cccccc\"><strong>'%s' %s</strong></td></tr>\n", disk->descrition, 
		(type==DISK_MGMT_TYPE_MOUNT)?gettext("Mount Point Defination"):gettext("Mount Status") );
	length += CGI_SPRINTF(buf,length, "<tr><td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></td><td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></td></tr>\n",
		gettext(CGI_STR_PARTITION) , gettext(CGI_STR_MOUNT_POINT) );

	while(part )
	{
		char cgiName[128];
		length += CGI_SPRINTF(buf,length, "<TR><TD align=\"center\">%s-%d</TD>\r\n", 	gettext(CGI_STR_PARTITION), i );

		CGI_SPRINTF(cgiName, 0, "%s_%d_%s", DISK_KEYWORD_PART_DEV, i, DISK_KEYWORD_PART_MOUNTPOINT );
		length += CGI_SPRINTF(buf,length, "<TD align=\"center\">%s</TD></TR>\r\n", GET_VALUE(diskConfig, cgiName)  );

		i ++;
		part = part->next;
	}
	
	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"center\" colspan=2>\r\n");
	
	if(type==DISK_MGMT_TYPE_MOUNT)
		sprintf(msg, "%s %s", gettext("Disk will mount"), gettext("when next reboot"));
	else	
		sprintf(msg, "%s %s", gettext("Disk will not mount"), gettext("when next reboot"));

	length += CGI_SPRINTF(buf,length, cgi_submit_button_with_alert(
		(type==DISK_MGMT_TYPE_MOUNT)?gettext("Remount"):gettext("Unmount"),msg, FORM_NAME ) );

	length += CGI_SPRINTF(buf,length, cgi_help_button(CGI_HELP_MSG_DISK_MGMT) );
	length += CGI_SPRINTF(buf,length, "</TD></TR></FORM>\r\n");

	return strdup(buf);
}

as_disk_t *__check_hd_disk(as_disk_t *disks)
{
	as_disk_t *tmp = disks;
	char		title[256];
	
	while( tmp)
	{
		if(tmp->status == AS_DISK_HARD_DISK )
		{
			if(! disk_check_root_disk(tmp) )
			{
				return tmp;
			}
		}
		tmp = tmp->next;
	}

	sprintf(title, "%s : %s", gettext(CGI_STR_DISK_MANAGE), gettext(CGI_STR_ERROR) );
	cgi_error_page(NULL, title, gettext("No IDE Hard Disk is found") );
	
	return NULL;
}

int sys_disk_management(as_disk_t *disks, llist *diskConfig)
{
	char			msg[4096];
	int 			len = 0;
	as_partition_t *part ;
	int 			isMount 	= 0;
	as_disk_t		*disk = __check_hd_disk(disks);

	if(!disk )
		return -1;

	part = disk->parts;
	if(!part)
	{
		len += CGI_SPRINTF(msg, len, "%s, %s", gettext("No partitions is allocated"), gettext("Please install it first") );
		cgi_error_page(NULL, gettext(CGI_STR_DISK_MANAGE),  msg );
		return -1;
	}

	if( part)
	{

		while( part )
		{
			if(strlen( part->mountPoint) )
			{
				isMount = 1;
				break;
			}
			part = part->next;
		}

		if(isMount)
		{
			sprintf(msg, "%s : %s", gettext(CGI_STR_DISK_MANAGE), gettext("Unmount") );
			return cgi_info_page(msg , "", __cgi_unmount_buttons(disk, diskConfig, DISK_MGMT_TYPE_UNMOUNT) );
		}
	}	

	if(strcasecmp(DISK_OP_DISK_UNMOUNT, GET_VALUE(diskConfig, DISK_KEYWORD_HD_STATUS) )  )
	{
		len += CGI_SPRINTF(msg, len, "%s, %s", gettext("This Disk is not unmount"), gettext("Please install it first") );
		cgi_error_page(NULL, gettext(CGI_STR_DISK_MANAGE),  msg );
		return -1;
	}

	sprintf(msg, "%s : %s", gettext(CGI_STR_DISK_MANAGE), gettext("Remount") );
	return cgi_info_page(msg , "", __cgi_unmount_buttons(disk, diskConfig, DISK_MGMT_TYPE_MOUNT) );
}


int sys_disk_mgmt_remount(as_disk_t *disks, llist *diskConfig)
{
	char			title[256];
	char 		msg[2048];
	int 			len = 0;
	as_disk_t		*disk = __check_hd_disk(disks);

	if(!disk )
		return -1;
	
	if(strcasecmp(DISK_OP_DISK_UNMOUNT, GET_VALUE(diskConfig, DISK_KEYWORD_HD_STATUS) )  )
	{
		len += CGI_SPRINTF(msg, len, "%s, %s", gettext("This Disk is not unmount"), gettext("Please install it first") );
		cgi_error_page(NULL, gettext(CGI_STR_DISK_MANAGE),  msg );
		return -1;
	}

	config_file_replace_field(HD_DISK_CONFIG_FILE, ITEM_EQU, DISK_KEYWORD_HD_STATUS, DISK_OP_DISK_MOUNT);

	sprintf(title, "%s : %s", gettext(CGI_STR_DISK_MANAGE), gettext("Result") );
	sprintf(msg, "'%s' %s %s",disk->descrition, gettext("will be remount"), gettext("when next reboot") );
	cgi_info_page(title, "", msg);
	
	return 0;
}

int sys_disk_mgmt_unmount(as_disk_t *disks, llist *diskConfig)
{
	char			title[256];
	char 		msg[2048];
	int 			len = 0;
	as_disk_t		*disk = __check_hd_disk(disks);

	if(!disk )
		return -1;
	
	if(strcasecmp(DISK_OP_DISK_MOUNT, GET_VALUE(diskConfig, DISK_KEYWORD_HD_STATUS) )  )
	{
		len += CGI_SPRINTF(msg, len, "%s, %s", gettext("This Disk is not unmount"), gettext("Please install it first") );
		cgi_error_page(NULL, gettext(CGI_STR_DISK_MANAGE),  msg );
		return -1;
	}

	config_file_replace_field(HD_DISK_CONFIG_FILE, ITEM_EQU, DISK_KEYWORD_HD_STATUS, DISK_OP_DISK_UNMOUNT);

	sprintf(title, "%s : %s", gettext(CGI_STR_DISK_MANAGE), gettext("Result") );
	sprintf(msg, "'%s' %s %s",disk->descrition, gettext("will be unmount"), gettext("when next reboot") );
	cgi_info_page(title, "", msg);
	
	return 0;
}


