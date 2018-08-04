/*
 * $Id: disk_format.c,v 1.4 2007/09/06 09:19:59 lizhijie Exp $
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

static char *__cgi_format_add_buttons(as_disk_t *disk)
{
	char 	buf[4096];
	int 		length = 0;
	int		i = 0;
	as_partition_t *part = disk->parts;

	length += CGI_SPRINTF(buf, length,"<form name=\"formatForm\" method=\"post\" action=\"%s\">\n", WEB_URL_DISK_MGMT);
	length += CGI_SPRINTF(buf, length, "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_KEYWORD_OPERATOR, DISK_OP_PART_FORMATING );
	length += CGI_SPRINTF(buf, length, "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", DISK_KEYWORD_DISK, disk->name);
	length += CGI_SPRINTF(buf, length, "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", DISK_KEYWORD_HD_STATUS, DISK_OP_DISK_FDISKING);

	length += CGI_SPRINTF(buf, length, "<tr><td colspan=6 align=\"center\" bgcolor=\"#cccccc\"><strong>'%s' %s</strong></td></tr>\n",
		disk->descrition, gettext(CGI_STR_PARTITION_INFO) );
	length += CGI_SPRINTF(buf,length, "<tr><td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></td><td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></td><td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></td><td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></td><td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></td><td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></td></tr>\n",
		gettext(CGI_STR_PARTITION) , gettext(CGI_STR_START_CYLINDER), gettext(CGI_STR_END_CYLINDER), gettext(CGI_STR_ID),gettext("FileSystem"), gettext(CGI_STR_MOUNT_POINT) );

	while(part)
	{
		char cgiName[128];
		i ++;
		length += CGI_SPRINTF(buf,length, "<TR><TD align=\"center\">%s-%d</TD>\r\n", 	gettext(CGI_STR_PARTITION), i );

		length += CGI_SPRINTF(buf,length, "<input type=\"hidden\" name=\"%s_%d\" value=\"%s%d\">\n", DISK_KEYWORD_PART_DEV,i, disk->name,  i);

		length += CGI_SPRINTF(buf,length, "<TD align=\"center\">%d</TD>\r\n", 	part->startCylinder );
		length += CGI_SPRINTF(buf,length,  "<TD align=\"center\">%d</TD>\r\n", part->endCylinder );
		length += CGI_SPRINTF(buf,length,  "<TD align=\"center\">%d</TD>\r\n", part->id );

		CGI_SPRINTF(cgiName, 0, "%s_%d_%s", DISK_KEYWORD_PART_DEV, i, DISK_KEYWORD_FS_TYPE);
		length += CGI_SPRINTF(buf,length,  "<TD align=\"center\">%s</TD>\r\n", GET_VALUE(&diskCgiVariables, cgiName) );

		CGI_SPRINTF(cgiName, 0, "%s_%d_%s", DISK_KEYWORD_PART_DEV, i, DISK_KEYWORD_PART_MOUNTPOINT );
		length += CGI_SPRINTF(buf,length, "<TD>%s</TD></TR>\r\n", GET_VALUE(&diskCgiVariables, cgiName)  );

		part = part->next;
	}
	
	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"center\" colspan=6>\r\n");
	length += CGI_SPRINTF(buf,length, cgi_submit_button(gettext(CGI_STR_NEXT) ) );

	length += CGI_SPRINTF(buf,length, cgi_help_button(CGI_HELP_MSG_DISK_MGMT) );
	length += CGI_SPRINTF(buf,length, "</TD></TR></FORM>\r\n");

	return strdup(buf);
}

int	sys_partition_format(as_partition_t *part)
{
	char 	target[128];
	char		msg[1024];
	char		command[128];
	char		*fsType;

	if(strlen(part->mountPoint) )
	{
		if(!strcasecmp(part->mountPoint, "/" ) )
		{
			cgi_error_page(NULL, gettext(CGI_STR_ERROR), gettext("root partition can not be format") );
			return 0;
		}

		CGI_SPRINTF(target,0, "%s?%s=%s&%s=%s&%s=%d", WEB_URL_DISK_MGMT, CGI_KEYWORD_OPERATOR, DISK_OP_PART_FORMAT, 
			DISK_KEYWORD_DISK, part->disk->name, DISK_KEYWORD_PART, part->index );
		CGI_SPRINTF(msg, 0, "'%s' %s '%s'. %s, %s", part->name, gettext("mount on the"), part->mountPoint, gettext("In order to format this partition"), gettext("it must be unmount from kernel file system firstly") );
		cgi_refresh_page(5, target, gettext("Umount Partition"), msg );

		CGI_SPRINTF(command, 0, "umount %s", part->mountPoint );
		system(command);

		return 0;
	}
	
//	CGI_SPRINTF(target,0,  "%s_%d_%s", DISK_KEYWORD_PART_DEV, part->index, DISK_KEYWORD_FS_TYPE);
	CGI_SPRINTF(target,0,  "%s_%d_%s", DISK_KEYWORD_PART_DEV, part->index-1, DISK_KEYWORD_FS_TYPE);
	fsType = GET_VALUE(&hdDiskCfgs, target);

	if(!fsType)
	{
		cgi_error_page(NULL, gettext(CGI_STR_ERROR), gettext("No File System Type os assigned"));
		return -1;
	}
	if( !strcasecmp(fsType, DISK_FILESYSTEM_EXT2) )
	{
		CGI_SPRINTF(command,0, DISK_BACKEND_E2_FORMAT, part->name );
	}	
	else if(!strcasecmp(fsType, DISK_FILESYSTEM_EXT3) )
	{
		CGI_SPRINTF(command, 0, DISK_BACKEND_E3_FORMAT, part->name, part->name );
	}	
	else
	{
		CGI_SPRINTF(command, 0, DISK_BACKEND_REISERFS_FORMAT, part->name );
	}
#if 0	
	fork_cmd(command);
#else
	system(command);
#endif

	CGI_SPRINTF(target,0,  "%s_%d_%s", DISK_KEYWORD_PART_DEV, part->index -1, DISK_KEYWORD_PART_STATUS);
	config_file_replace_field(HD_DISK_CONFIG_FILE, ITEM_EQU, target, DISK_OP_DISK_MOUNT );
	
	return 0;
}


int sys_disk_format(as_disk_t *disk, char *cmd)
{
	char			target[256];
	char			msg[4096];
	int 			len = 0;
	as_partition_t *part = disk->parts;
	int			i=0;

	if(!part)
	{
		cgi_error_page(NULL, gettext("Format Error"), gettext("No partitions is allocated in disk when format disk") );
		return -1;
	}

	if(!strcasecmp(cmd, DISK_OP_PART_FORMAT) )
	{
		char title[128];
		CGI_SPRINTF(title,0, "%s : %s '%s'", gettext("Hard Disk Install"), gettext("Format"), disk->descrition );
		cgi_info_page(title, as_cgi_disk_info(disk), __cgi_format_add_buttons(disk) );

		return 0;
	}
	else
	{
	
		CGI_SPRINTF(target,0, "%s?%s=%s&%s=%s", WEB_URL_DISK_MGMT, CGI_KEYWORD_OPERATOR, DISK_OP_PART_INFO, 
				DISK_KEYWORD_DISK, part->disk->name );
		len += CGI_SPRINTF(msg,len, "'%s' %s,", part->disk->descrition, gettext("is formatting") );
		len += CGI_SPRINTF(msg,len, "%s.", gettext("it may cost some miniutes to finish this work") );
		len += CGI_SPRINTF(msg,len, "%s", gettext("How long is determined by the capacity of this partition") );
		cgi_refresh_page(DISK_FORMAT_REFRESH_TIMEOUT, target, gettext("Formatting now"), msg );
			
		while(part)
		{
			len = 0;
			len += CGI_SPRINTF(msg,len, "%s-%d ('%s') %s.....", gettext(CGI_STR_PARTITION), ++i , part->name, gettext("is formatting"));
			printf("<br>%s", msg);

			fflush(stdout);
			if( sys_partition_format(part)< 0)
			{
				len = 0;
				len += CGI_SPRINTF(msg,len, "'%s' %s", part->name, gettext("can not be formatted") );
				cgi_error_page(NULL, gettext(CGI_STR_ERROR), msg);

				return -1;
			}
			part = part->next;
		}
		
		printf("<p>%s %d %s %s!",gettext(CGI_STR_ALL), i, gettext(CGI_STR_PARTITION), gettext("are formatted"));
		fflush(stdout);

		config_file_replace_field(HD_DISK_CONFIG_FILE, ITEM_EQU, DISK_KEYWORD_HD_STATUS, DISK_OP_DISK_MOUNT);
	}

	return 0;
}


