/*
* $Id: disk_install.c,v 1.3 2007/02/07 19:04:17 lizhijie Exp $
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

#define	FORM_NAME		"fdiskForm"

int	sys_disk_install(as_disk_t *disks)
{
	char			button[2048];
	char			target[256], msg[256];
	char			title[256];
	int			len = 0;
	as_disk_t		*disk = NULL;
	as_disk_t 	*tmp = disks;

	while( tmp)
	{
		if(tmp->status == AS_DISK_HARD_DISK && tmp->hasRootFs )
		{
//			if(! disk_check_root_disk(tmp) )
			{
				disk = tmp;
				break;
			}
		}
		tmp = tmp->next;
	}

	if(! disk )
	{
		cgi_error_page(NULL, gettext("Hard Disk Fail"), gettext("No Hard Disk can be used as data disk") );
		return 0;
	}

	len += CGI_SPRINTF(button, len, "<form name=\"%s\" method=\"post\" action=\"%s\">\n",FORM_NAME, WEB_URL_DISK_MGMT);
	len += CGI_SPRINTF(button, len,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_KEYWORD_OPERATOR, DISK_OP_DISK_FDISK);
	len += CGI_SPRINTF(button, len, "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", DISK_KEYWORD_DISK, disk->name);
	len += CGI_SPRINTF(button, len,  "<input type=\"hidden\" name=\"%s\" value=\"%d\">\n", DISK_KEYWORD_HD_NUM, 0 );
	len += CGI_SPRINTF(button, len,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", DISK_KEYWORD_HD_STATUS, DISK_OP_DISK_FDISK );

	if(disk->parts)
	{
		CGI_SPRINTF(msg, 0, "%s %s %s. %s", gettext("In order to install"), disk->descrition, gettext("all partitions will be deleted"), gettext(CGI_STR_ARE_YOU_SURE) );

		len += CGI_SPRINTF(button,len, "<TR><TD align=\"center\">%s\n", cgi_submit_button_with_alert(gettext(CGI_STR_NEXT),msg, FORM_NAME) );

		CGI_SPRINTF(target, 0, "%s?%s=%s&%s=%s", WEB_URL_DISK_MGMT, CGI_KEYWORD_OPERATOR, 
				DISK_OP_PART_INFO,DISK_KEYWORD_DISK, disk->name );
		len += CGI_SPRINTF(button, len, "%s</TD></TR></FORM>", cgi_button(gettext(CGI_STR_PARTITION_INFO), target)  );	
		CGI_SPRINTF(title,0,  "%s : %s", gettext("Hard Disk Install"), gettext("Remove exist partitions") );
	}
	else
	{
		len += CGI_SPRINTF(button,len, "<TR><TD align=\"center\">%s</TD></TR></FORM>\n", cgi_submit_button(gettext(CGI_STR_NEXT) ) );
		CGI_SPRINTF(title,0,  "%s : %s", gettext("Hard Disk Install"), gettext("Select Disk") );
	}

	
	cgi_info_page(title , as_cgi_disk_info(disk), button);

	return 0;
}

