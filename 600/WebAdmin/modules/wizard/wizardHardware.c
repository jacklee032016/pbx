/*
* $Id: wizardHardware.c,v 1.7 2007/09/06 09:19:59 lizhijie Exp $
*/

#include "wizard.h"

char *__wizard_disk_parts_info(as_disk_t *disk)
{
	as_partition_t  *current = disk->parts;
	char 	buf[4096*5];
	int 		length = 0;
	
	if(!current)
	{
		CGI_SPRINTF(buf,0, "<TR><TD align=\"center\" colspan=4>'%s' %s</TD></TR>\n", disk->descrition, gettext("has no partitions, so fdisk it first!") );
		return strdup(buf);
	}

	cgidebug("disk %s part info %s\n", disk->name, current->name );
	while (current) 
	{

		length += CGI_SPRINTF(buf,length, "<TR><TD align=\"center\" bgcolor=\"#cccccc\" colspan=4><strong>%s--%d (%s)</strong></TD></TR>\n", 
			gettext(CGI_STR_PARTITION_INFO), current->index, current->name);

		length += CGI_SPRINTF(buf,length, "<TR><TD align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD><TD align=\"center\">%d</TD>\n", 
			gettext(CGI_STR_START_CYLINDER), current->startCylinder );
		length += CGI_SPRINTF(buf,length,  "<TD align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD><TD align=\"center\">%d</TD></TR>\n", 
			gettext(CGI_STR_END_CYLINDER), current->endCylinder);
			
		length += CGI_SPRINTF(buf,length,  "<TR><TD align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD><TD align=\"center\">%s</TD>\n", 
			gettext("Blocks"), current->blocks);
		length += CGI_SPRINTF(buf,length, "<TD align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD><TD align=\"center\">%d</TD></TR>\n", 
			gettext(CGI_STR_ID), current->id);

		length += CGI_SPRINTF(buf,length,  "<TR><TD align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD><TD align=\"center\">%s</TD>\n", 
			gettext("System"), current->system );
		length += CGI_SPRINTF(buf,length,  "<TD align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD><TD align=\"center\">%s</TD></TR>\n", 
			gettext(CGI_STR_MOUNT_POINT), current->mountPoint );
		
		length += CGI_SPRINTF(buf,length,  "<TR><TD colspan=2 align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD><TD colspan=2 align=\"center\">%s</TD></TR>\n", 
			gettext("File System"), current->fileSystemType  );

		current = current->next;
	}

	return strdup(buf);
}


char *__wizard_disk_disks_info(as_disk_t *disks)
{
	as_disk_t  *current = disks;
	char 	buf[4096*16];
	int 		length = 0;

	while (current)
	{
		length += CGI_SPRINTF(buf,length, "<table width=\"100%%\" border=\"2\" align=\"center\" cellpadding=\"1\" cellspacing=\"1\" bgcolor=\"#FFFFFF\" >\n");
		
		length += CGI_SPRINTF(buf,length, "<TR><TD align=\"center\" bgcolor=\"#cccccc\"><strong>%s </strong></TD><TD align=\"center\">%s</TD>\n", 
			gettext("Disk Info"), current->descrition);
		if(current->hasRootFs)
		{
			length += CGI_SPRINTF(buf,length, "<TD align=\"center\" colspan=2><strong>%s, %s</strong></TD></TR>\n", 
				gettext("Device of Current Root File System"), gettext("which can not used as install device") );
		}
		else
		{
			length += CGI_SPRINTF(buf,length, "<TD align=\"center\" bgcolor=\"#cccccc\"><strong>%s </strong></TD><TD align=\"center\"><input name=\"%s\" type=\"Radio\" value=\"%s\">%s</TD></TR>\n", 
				gettext("Restore On This Device"), DISK_KEYWORD_DISK, current->name, CGI_STR_YES );
		}
		
		length += CGI_SPRINTF(buf,length, "<TR><TD align=\"center\" colspan=2 bgcolor=\"#cccccc\"><strong>%s</strong></TD><TD align=\"center\">%d %s</TD><TD align=\"center\">%s bytes</TD></TR>\n", 
			gettext("Capacity"), current->capacity, current->unit, current->capacityB);

		length += CGI_SPRINTF(buf,length, "%s", __wizard_disk_parts_info(current) );

		length += CGI_SPRINTF(buf, length, "</table>\n" );
		
		current = current->next;
	}
	
	return strdup(buf);
}


int  wizard_cgi_hardware_check(WIZARD_INFO *info )
{
	char 	status[2048*10];
	char 	alert[2048*10];
	char 	leftButton[2048];
	char 	rightButton[2048];

	int 		length = 0;
	as_disk_t  *disk =info->disks;

	length += CGI_SPRINTF(status,length, "<table width=\"100%%\" border=\"1\" align=\"center\" cellpadding=\"1\" cellspacing=\"1\" bgcolor=\"#FFFFFF\" >\n");

	length += CGI_SPRINTF(status, length, "%s", __wizard_disk_disks_info(disk) );
	length += CGI_SPRINTF(status, length, "</table>\n" );

	length = 0;	
	length += CGI_SPRINTF(alert, length, "<table height=\"21\" align=\"center\" bgColor=#f7f3f7>\n");
	length += CGI_SPRINTF(alert, length, "<tr><td>%s.\n", gettext("Mass Storage Disk has been detected"));
	length += CGI_SPRINTF(alert, length, "<tr><td>%s, %s.\n", gettext("In order to restore system"), gettext("you must select the device which Wizard will use") );
	length += CGI_SPRINTF(alert, length, "<tr><td><strong>%s</strong> : %s!\n", gettext(CGI_STR_NOTE), gettext("The device will be repartitioned and format as our requirement" ));
	length += CGI_SPRINTF(alert, length, "</td></tr>\n" );
	length += CGI_SPRINTF(alert, length, "</table>\n" );

	length = 0;
	length += CGI_SPRINTF(rightButton,length,"%s", cgi_submit_button(gettext(WIZARD_STR_PARTITION) ) );
	length = 0;
	length += CGI_SPRINTF(leftButton,length, "%s", cgi_button(gettext(WIZARD_STR_HARDWARE), WEB_URL_WIZARD) );
	
	return wizard_page( gettext(WIZARD_STR_HARDWARE), status, alert, leftButton, rightButton, info);
}



as_disk_t  *wizard_find_disk(WIZARD_INFO *info)
{
	int		length = 0;
	char		alert[1024];
	char		*diskname;
	as_disk_t  *disk = NULL;

	diskname = GET_VALUE(&info->cgiVariables, DISK_KEYWORD_DISK) ;
	if( !strlen(diskname) )
	{
		wizard_cgi_hardware_check(info );
		return NULL;
	}
	disk = as_cgi_disk_find_disk(info->disks, diskname);
	if(!disk)
	{
		length += CGI_SPRINTF(alert, length, "'%s' %s\n" , diskname, gettext("is not found") );
		cgi_error_page(NULL, gettext(CGI_STR_ERROR), alert);
		return NULL;
	}

	return disk;
}

