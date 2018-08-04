/*
* $Id: wizardFdisk.c,v 1.4 2007/09/06 09:19:59 lizhijie Exp $
*/

#include "wizard.h"

int	__wizard_remove_parts(as_disk_t *disk, WIZARD_INFO *info)
{
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
				return -1;
			}

			sprintf(msg, "'%s' of '%s' %s.....", part->name, disk->descrition, gettext("is umounting") );
			CGI_INSTANCE_INFO(wizard_page_status(msg,  info), CGI_COLOR_BLUE );
			
			CGI_SPRINTF(command, 0, "umount %s > /dev/null  2>&1", part->mountPoint );
			system(command);
		}
		part = part->next;
	}

	if( disk->parts )
	{/* delete old parts */
		part = disk->parts;

		length = 0;
		length += CGI_SPRINTF(command,length, "echo -e \"" );
		while(part)
		{
			length += CGI_SPRINTF(command,length, "d\\n%d\\n", part->index );

			sprintf(msg, "'%s' of '%s' %s.....", part->name, disk->descrition, gettext("is deleting") );
			CGI_INSTANCE_INFO(wizard_page_status(msg,  info), CGI_COLOR_BLUE);
			
			part = part->next;	
		}

		length += CGI_SPRINTF(command,length, "w \" | fdisk %s  > /dev/null  2>&1", disk->name );
		system(command);

//		cgidebug("fdisk command : %s\n", command );
		
	}

	return 0;
}


int	__wizard_refdisk_parts(as_disk_t *disk, WIZARD_INFO *info)
{
	char		msg[4096];
	char		command[1024];
	int 		length = 0;
//	as_partition_t *part = disk->parts;
	part_info_t	*partCfg = disk->partConfig;
	int		i = 1;

	length += CGI_SPRINTF(command,length, "echo -e \""); 	/* first partition */

	sprintf(msg, "'%s' %s.....", disk->descrition, gettext("is creating a new partition") );
	CGI_INSTANCE_INFO(wizard_page_status(msg,  info), CGI_COLOR_BLUE);
	while(partCfg->ratio != -1)
	{
		char		size[32];
		int		len = 0;
	
		len += CGI_SPRINTF(msg,len,  "%s '%s%d' : ",gettext(CGI_STR_PARTITION),  disk->name, i);
		
		if(partCfg->sizeType==PART_SIZE_ABSOLUTE)
		{
			sprintf(size, "+%dM", partCfg->ratio );
			len += CGI_SPRINTF(msg,len,  "%s %dM", gettext("about"), partCfg->ratio);
		}
		else
		{
			if(partCfg->ratio == 100)
			{/* used up left space on disk */
				sprintf(size, "%s", "\\n");
				len += CGI_SPRINTF(msg,len, " %s", gettext("Left Space"));
			}
			else
			{
				int cap = disk->capacity;
				if(strchr(disk->unit, 'G' ) )
				{
					cap = cap*1024;
				}
				
				sprintf(size, "+%dM", partCfg->ratio*cap /100);
				len += CGI_SPRINTF(msg,len, "%s %dM", gettext("about"), partCfg->ratio*cap /100);
			}
		}

		CGI_INSTANCE_INFO(wizard_page_status(msg,  info), CGI_COLOR_BLUE);

		/* n : new ; p : primary; 1 : first; \n : CR, default(First Cylinder);  */
		length += CGI_SPRINTF(command,length, "\nn\\np\\n%d\\n\\n%s\\n", i, size ); 	/* first partition */

		i++;
		partCfg++;
	}
	length += CGI_SPRINTF(command,length, "\\nw\\n\" | fdisk %s  > /dev/null  2>&1", disk->name ); 		/* write and quit fdisk */

	cgidebug("create new partition command : %s\n", command );
	system(command);

	return 0;
}

int  wizard_cgi_hd_partition(WIZARD_INFO *info )
{
	char 	status[2048];
	char 	alert[2048];
	char 	leftButton[2048];
	char 	rightButton[2048];

	int 		length = 0;
	char		*diskname;
	as_disk_t  *disk = NULL;

	diskname = GET_VALUE(&info->cgiVariables, DISK_KEYWORD_DISK) ;
	if( !strlen(diskname) )
	{
		return wizard_cgi_hardware_check(info );
	}

	disk = as_cgi_disk_find_disk(info->disks, diskname);
	if(!disk)
	{
		length += CGI_SPRINTF(status, length, "'%s' %s\n" , diskname, gettext("is not found") );
		return cgi_error_page(NULL, CGI_STR_ERROR, status);
	}

	printf("%s", wizard_page_begin(gettext(WIZARD_STR_PARTITION), info) );
//	printf("%s", wizard_page_status_begin( info));
	fflush(stdout);

	if(!__wizard_remove_parts(disk, info) )
	{
		__wizard_refdisk_parts(disk,  info);
	}
	else
	{
	}

	length = 0;	
	length += CGI_SPRINTF(alert, length, "<table height=\"21\" align=\"center\" bgColor=#f7f3f7>\n");
	length += CGI_SPRINTF(alert, length, "<tr><td>%s.\n", gettext("The HD Disk has been repartitioned now"));
	length += CGI_SPRINTF(alert, length, "<tr><td>%s, %s.\n", gettext("In order to install new system"), gettext("it must be format by WIZARD in next step") );
	length += CGI_SPRINTF(alert, length, "</td></tr>\n" );
	length += CGI_SPRINTF(alert, length, "</table>\n" );
	printf("%s", wizard_page_alert(alert, info) );

	printf( "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", DISK_KEYWORD_DISK, disk->name );
	
	length = 0;
	length += CGI_SPRINTF(rightButton,length,"%s", cgi_submit_button(gettext(WIZARD_STR_FORMAT) ) );

	length = 0;
	length += CGI_SPRINTF(leftButton,length, "%s", cgi_button(gettext(WIZARD_STR_PARTITION), WEB_URL_WIZARD"?"CGI_KEYWORD_OPERATOR"="WIZARD_OP_HARDWARE) );
	printf("%s", wizard_page_end(leftButton, rightButton,  info) );
	
	return 0;
}


