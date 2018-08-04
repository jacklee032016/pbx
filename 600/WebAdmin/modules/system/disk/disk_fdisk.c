/*
 * $Id: disk_fdisk.c,v 1.6 2007/09/06 09:19:59 lizhijie Exp $
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

static char *__cgi_fdisk_add_buttons(as_disk_t *disk)
{
	char buf[4096];
	int 	length = 0;
	int 	i;
	part_info_t *info = disk->partConfig;//__default_parts;

#define	SELECTED_VALUE(info, value)		(( strcasecmp(info->filesystem, value) )?"": "selected" )

	length += CGI_SPRINTF(buf, length, "<form name=\"fdiskForm\" method=\"post\" action=\"%s\">\n", WEB_URL_DISK_MGMT);
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_KEYWORD_OPERATOR, DISK_OP_DISK_FDISKING);
	length += CGI_SPRINTF(buf, length, "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", DISK_KEYWORD_DISK, disk->name);
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%d\">\n", DISK_KEYWORD_HD_NUM, 3 );
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", DISK_KEYWORD_HD_STATUS, DISK_OP_DISK_FDISKING);

	length += CGI_SPRINTF(buf, length,  "<tr><td colspan=5 align=\"center\" bgcolor=\"#cccccc\"><strong>'%s' %s</strong></td></tr>\n",
		disk->descrition, gettext(CGI_STR_PARTITION_INFO) );
	length += CGI_SPRINTF(buf, length,  "<tr><td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></td><td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></td><td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></td><td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></td><td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></td></tr>\n",
		gettext(CGI_STR_PARTITION) , gettext("Ratio"), gettext("Capacity"), gettext("File System Type"), gettext(CGI_STR_MOUNT_POINT) );

	while(info->ratio != -1 )
	{
		length += CGI_SPRINTF(buf, length,  "<TR><TD align=\"center\">%s-%d</TD>\r\n", 	gettext(CGI_STR_PARTITION), i );

		length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s_%d\" value=\"%s%d\">\n", DISK_KEYWORD_PART_DEV, i, disk->name,  i +1 );
		length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s_%d_%s\" value=\"%s\">\n", DISK_KEYWORD_PART_DEV, i, DISK_KEYWORD_PART_STATUS, DISK_OP_DISK_FDISKING);
		length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s_%d_%s\" value=\"%s\">\n", DISK_KEYWORD_PART_DEV, i, DISK_KEYWORD_PART_MOUNTPOINT,  info->mountpoint);
		length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s_%d_%s\" value=\"%s\">\n", DISK_KEYWORD_PART_DEV, i, DISK_KEYWORD_PART_MT_SUBDIRS,  info->subdirs);

		length += CGI_SPRINTF(buf, length,  "<TD align=\"center\"><input name=\"%s_%d_%s\" onKeyPress=\"checkNumber(this)\" type=\"text\" size=\"5\" value=\"%d\" maxlength=\"50\"> %% </TD>\r\n", 
			DISK_KEYWORD_PART_DEV,i, DISK_KEYWORD_SIZE, info->ratio );

		length += CGI_SPRINTF(buf, length,  "<TD>%d %s</TD>\r\n", 
			(int)(disk->capacity*info->ratio/100), disk->unit);

		length += CGI_SPRINTF(buf, length, "<TD align=\"center\"><select name=\"%s_%d_%s\"  maxlength=\"30\">\n", DISK_KEYWORD_PART_DEV, i, DISK_KEYWORD_FS_TYPE);

		length += CGI_SPRINTF(buf, length,  "<option value=\"%s\" %s>%s</option>", DISK_FILESYSTEM_EXT2,  SELECTED_VALUE(info, DISK_FILESYSTEM_EXT2), DISK_FILESYSTEM_EXT2);
		length += CGI_SPRINTF(buf, length,  "<option value=\"%s\" %s>%s</option>", DISK_FILESYSTEM_EXT3, SELECTED_VALUE(info, DISK_FILESYSTEM_EXT3), DISK_FILESYSTEM_EXT3 );
		length += CGI_SPRINTF(buf, length,  "<option value=\"%s\" %s>%s</option></SELECT></TD>", DISK_FILESYSTEM_REISERFS, SELECTED_VALUE(info,DISK_FILESYSTEM_REISERFS), DISK_FILESYSTEM_REISERFS );

		length += CGI_SPRINTF(buf, length,  "<TD>%s</TD></TR>\r\n", info->mountpoint );

		cgidebug("mount %s\n", info->mountpoint);
		
		info ++;

	}
	
	length += CGI_SPRINTF(buf, length, "<TR><TD align=\"center\" colspan=5>\r\n");
	length += CGI_SPRINTF(buf, length,  cgi_submit_button(gettext(CGI_STR_NEXT) ) );

	length += CGI_SPRINTF(buf, length,  cgi_help_button(CGI_HELP_MSG_DISK_MGMT) );
	length += CGI_SPRINTF(buf, length,  "</TD></TR></FORM>\r\n");

	return strdup(buf);
}


int	__cgi_fdisk_disk(as_disk_t *disk)
{
	int		i;
	int 		ratio;
	int		size;
	char 	target[128];
	char		msg[4096];
	char		command[1024];
	int 		length = 0;
	char		*num = GET_VALUE( &diskCgiVariables, DISK_KEYWORD_HD_NUM);
	int 		part_num =(num)?atoi(num): 0;
	
	if(part_num== 0)
	{
		cgi_error_page(NULL, gettext(CGI_STR_ERROR), gettext("No Partition number is defined for this disk") );
		return -1;
	}

	length += CGI_SPRINTF(command,length, "echo -e \""); 	/* first partition */
	for(i = 0; i< part_num-1; i++)
	{
		char cgiName[256];
		CGI_SPRINTF(cgiName,0, "%s_%d_%s", DISK_KEYWORD_PART_DEV,i, DISK_KEYWORD_SIZE );
//			ratio = atoi(cgi_value( diskCgiVariables, DISK_KEYWORD_PART1_SIZE) );
		ratio = atoi( GET_VALUE(&diskCgiVariables, cgiName ) );

		size = ratio *disk->capacity;
		if(!strcasecmp(disk->unit, "GB") )
		{
			size = size*1024;
		}
		size = size/100;
		
		length += CGI_SPRINTF(command,length, "\nn\\np\\n%d\\n\\n+%dM", i+1, size ); 	/* first partition */
	}
	
	length += CGI_SPRINTF(command,length, "\\nn\\np\\n%d\\n\\n", i+1 ); 					/* first partition : all space left */
	length += CGI_SPRINTF(command,length, "\\nw\\n\" | fdisk %s >> /dev/null", disk->name ); 		/* write and quit fdisk */
	cgidebug("fdisk command : %s\n", command );
	
#if 1
		CGI_SPRINTF(target, 0,"%s?%s=%s&%s=%s", WEB_URL_DISK_MGMT, CGI_KEYWORD_OPERATOR, DISK_OP_PART_FORMAT, 
			DISK_KEYWORD_DISK, disk->name );
#else
		CGI_SPRINTF(target, "%s?%s=%s&%s=%d", WEB_URL_DISK_MGMT, CGI_KEYWORD_OPERATOR, DISK_OP_PART_INFO, 
			DISK_KEYWORD_DISK, disk->index );
#endif
		CGI_SPRINTF(msg,0, "'%s' %s", disk->descrition, gettext("is fdisking, please waiting ....."));
		cgi_refresh_page(5, target, gettext("Fdisking now"), msg );
		
		system(command);

		writeconfig(HD_DISK_CONFIG_FILE, ITEM_EQU, NULL, &diskCgiVariables);
		return 0;
}


int	sys_disk_fdisk(as_disk_t *disk, char *cmd)
{
	char 	title[128];
	char		msg[4096];
	char		command[1024];
	int 		length = 0;

	as_partition_t *part = disk->parts;

	while(part)
	{
		if(strlen(part->mountPoint) )
		{
			if(!strcasecmp(part->mountPoint, "/" ) )
			{
				CGI_SPRINTF(msg, 0, "%s, %s '%s' %s!", gettext("root partition is on this disk"),gettext("Disk"),disk->descrition, gettext("can not be fdisk") );
				cgi_error_page(NULL, gettext(CGI_STR_ERROR), msg);
				return 0;
			}
			
			CGI_SPRINTF(command, 0, "umount %s", part->mountPoint );
			system(command);
		}
		part = part->next;
	}

	if( disk->parts )
	{/* delete old parts */
		as_partition_t *part = disk->parts;
		length += CGI_SPRINTF(command,length, "echo -e \"" );
		while(part)
		{
			length += CGI_SPRINTF(command,length, "d\\n%d\\n", part->index );
			part = part->next;	
		}

		length += CGI_SPRINTF(command,length, "w \" | fdisk %s >> /dev/null", disk->name );
		cgidebug("fdisk command : %s\n", command );
		system(command);

	}

	if(!strcasecmp(cmd, DISK_OP_DISK_FDISK) )
	{
		writeconfig(HD_DISK_CONFIG_FILE, ITEM_EQU, NULL, &diskCgiVariables);
		CGI_SPRINTF(title, 0, "%s : %s '%s'",gettext("Hard Disk Install"), gettext("Fdisk"), disk->descrition );
		return cgi_info_page(title, as_cgi_disk_info(disk), __cgi_fdisk_add_buttons(disk) );
	}
	else
	{
		return __cgi_fdisk_disk(disk);
	}
	
	return 0;
}

