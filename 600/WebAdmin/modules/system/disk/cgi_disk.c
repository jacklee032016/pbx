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

/* return 0 : not root disk ; others : root disk */
int	disk_check_root_disk(as_disk_t *disk)
{
	as_partition_t *part = disk->parts;
	int isRoot = 0;

	while(part)
	{
		if(!strcasecmp(part->mountPoint, "/") )
		{
			isRoot = 1;
			break;
		}
		part = part->next;
	}

	return isRoot;
}

int disk_write_config_file(as_disk_t *disk)
{

	if(!disk_check_root_disk(disk) )
	{
	}

	return 0;
}

char *cgi_disk_disks_info(as_disk_t  *disk)
{
	char 	buf[4096];
	int 		length = 0;
	char 	target[128];
	
//	length += CGI_SPRINTF(buf+length,sizeof(buf), "%s", as_cgi_disk_info(current) );
	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"center\" colspan=3 bgcolor=\"#cccccc\"><strong>%s : %s</strong></TD></TR>\n", 
		gettext("Disk Info"), disk->descrition);
	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD><TD align=\"center\">%d %s</TD><TD align=\"center\">%s bytes</TD></TR>\n", 
		gettext("Capacity"), disk->capacity, disk->unit, disk->capacityB);
	
	sprintf(target, "%s?%s=%s&%s=%s", WEB_URL_DISK_MGMT, CGI_KEYWORD_OPERATOR, DISK_OP_PART_INFO, 
		DISK_KEYWORD_DISK, disk->name );
	length += CGI_SPRINTF(buf, length, "<TR><TD align=\"center\" colspan=3>%s</TD></TR>", 
		cgi_button(gettext(CGI_STR_PARTITION_INFO), target)  );
	
	return strdup(buf);
}

