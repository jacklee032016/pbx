/*
* $Id: wizardFormat.c,v 1.4 2007/09/06 09:19:59 lizhijie Exp $
*/

#include "wizard.h"

int	__wizard_format_disk(as_disk_t *disk, WIZARD_INFO *info)
{
	char		command[1024];
	char		msg[1024];

	as_partition_t *part = disk->parts;
	part_info_t	*partCfg = disk->partConfig;

	while(part && partCfg )
	{
		if(!strcasecmp(partCfg->filesystem, DISK_FILESYSTEM_EXT3 ) )
			CGI_SPRINTF(command, 0, DISK_BACKEND_E3_FORMAT, part->name, part->name );
		else
			CGI_SPRINTF(command, 0, DISK_BACKEND_REISERFS_FORMAT, part->name );
		
		sprintf(msg, "'%s' of '%s' %s %s.....",part->name, disk->descrition, gettext("is formatting as"), partCfg->filesystem );
		CGI_INSTANCE_INFO(wizard_page_status(msg,  info), CGI_COLOR_BLUE);
			
		system(command);
		
		part = part->next;
		partCfg ++;
	}

	part = disk->parts;
	partCfg = disk->partConfig;
	while(part && partCfg )
	{
		char	mountPoint[128];
		
		sprintf(mountPoint, "%s%s", WIZARD_FS_MOUNT_POINT, partCfg->mountpoint );
		CGI_SPRINTF(command, 0, "mkdir -p %s", mountPoint);
		system(command);
		
		sprintf(msg, "'%s' of '%s' %s %s.....",part->name, disk->descrition, gettext("is mounting on"), mountPoint );
		CGI_INSTANCE_INFO(wizard_page_status(msg,  info), CGI_COLOR_BLUE);
				
		sprintf(part->fileSystemType, "%s", partCfg->filesystem);
		sprintf(part->mountPoint, "%s", mountPoint );
		CGI_SPRINTF(command, 0, "mount -t %s %s %s > /dev/null  2>&1", part->fileSystemType, part->name, mountPoint );
		
		system(command);
		
		part = part->next;
		partCfg ++;
	}

	sprintf(command, "mount -t tmpfs -o size=48M -o remount tmpfs /tmp" );
	system(command);
	
	return 0;
}

char *__wizard_upload_box(WIZARD_INFO *info)
{
	char buf[2048];
	int length = 0;
	
	length += sprintf(buf+length, "<TR><TD><input name=\"%s\" onChange=\"checkIsNotNULL(%s, '%s')\" type=\"file\" value=\"\">\r\n",
		WIZARD_KEY_UPLOAD_FILE, WIZARD_KEY_UPLOAD_FILE, gettext("File can not be NULL") );

//	length += sprintf(buf+length, cgi_submit_button( gettext(CGI_STR_UPLOAD) ) );
	length += sprintf(buf+length, "</TD></TR>\r\n");

	return strdup(buf);
}

int  wizard_cgi_hd_format(WIZARD_INFO *info )
{
	char 	alert[2048];
	char 	leftButton[2048];
	char 	rightButton[2048];
	char		target[256];

	int 		length = 0;
	char		*diskname;
	as_disk_t  *disk = NULL;
	as_partition_t *part;

	diskname = GET_VALUE(&info->cgiVariables, DISK_KEYWORD_DISK) ;
	if( !strlen(diskname) )
	{
		return wizard_cgi_hardware_check(info );
	}
	disk = as_cgi_disk_find_disk(info->disks, diskname);
	if(!disk)
	{
		length += CGI_SPRINTF(alert, length, "'%s' %s\n" , diskname, gettext("is not found") );
		return cgi_error_page(NULL, CGI_STR_ERROR, alert);
	}
	part = disk->parts;

	printf("%s", wizard_page_begin(gettext(WIZARD_STR_FORMAT), info) );
//	printf("%s", wizard_page_status_begin( info));
	fflush(stdout);

	__wizard_format_disk(disk, info);

	length = 0;	
	length += CGI_SPRINTF(alert, length, "<table height=\"21\" align=\"center\" bgColor=#f7f3f7>\n");
	length += CGI_SPRINTF(alert, length, "<tr><td>%s %s\n</td></tr>", disk->descrition, gettext("Partition and Format Result") );
	length += CGI_SPRINTF(alert, length, "<table width=\"100%%\" border=\"1\" align=\"center\" cellpadding=\"1\" cellspacing=\"1\" bgcolor=\"#FFFFFF\" >\n" );
	while(part)
	{
		length += CGI_SPRINTF(alert, length, "<tr><td align=\"center\" bgcolor=\"#cccccc\">%s</td><td align=\"center\" bgcolor=\"#cccccc\">%s</td><td align=\"center\" bgcolor=\"#cccccc\">%s</td></tr>", part->name, part->fileSystemType, part->mountPoint );
		part = part->next;
	}
	length += CGI_SPRINTF(alert, length, "</table>\n" );
	
	length += CGI_SPRINTF(alert, length, "<tr><td>%s<br>\n", gettext("Now, it is time for upload File System image with Browser." ));

	length += CGI_SPRINTF(alert, length, "%s</td></tr>\n</table>\n", __wizard_upload_box(info) );
	length += CGI_SPRINTF(alert, length, "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", DISK_KEYWORD_DISK, disk->name );
	printf("%s", wizard_page_alert(alert, info) );
	
	length = 0;
	length += CGI_SPRINTF(rightButton,length,"%s", cgi_submit_button(gettext(CGI_STR_UPLOAD) ) );

	sprintf(target, "%s?%s=%s&%s=%s", WEB_URL_WIZARD, CGI_KEYWORD_OPERATOR, WIZARD_OP_PARTITION,
		DISK_KEYWORD_DISK, disk->name );
	
	length = 0;
	length += CGI_SPRINTF(leftButton,length, "%s", cgi_button(gettext(WIZARD_STR_FORMAT), target) );
	printf("%s", wizard_page_end(leftButton, rightButton,  info) );
	
	return 0;
}


