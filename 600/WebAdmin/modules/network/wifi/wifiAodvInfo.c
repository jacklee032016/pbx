/*
* $Id: wifiAodvInfo.c,v 1.5 2007/09/06 09:19:59 lizhijie Exp $
*/

#include "cgi_lib.h"
#include "cgi_wifi.h"

typedef char *(*aodv_read_func_t)(FILE *fp);

static char *__wifi_aodv_info_route( FILE *fp)
{
	char 	buf[8192*5];
	int 		length = 0;
	char 	line[1024];
	int		count;
	char		dest[64],seq[64],hopCount[64],nextHop[64],route[64],type[64], t1[64],t2[64];

	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"right\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\n"
		"<TD align=\"right\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\n"
		"<TD align=\"right\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\n"
		"<TD align=\"right\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\n"
		"<TD align=\"right\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\n"
		"<TD align=\"right\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\n</TR>\r\n", 
		gettext("Destionation"), gettext("Sequence"), gettext("Hop Count"), gettext("Next Hop"), gettext("Route Type"), gettext(CGI_STR_TYPE));

	while(fgets(line,1024,fp))
	{
		count = sscanf(line,"%s %s %s %s %s %s %s %s", dest, seq, hopCount, nextHop, route, type, t1, t2);
		if(count !=6)
			continue;
		length += CGI_SPRINTF(buf,length, "<TR><TD align=\"right\">%s</TD>\n"
			"<TD align=\"right\">%s</TD>"
			"<TD align=\"right\">%s</TD>"
			"<TD align=\"right\">%s</TD>"
			"<TD align=\"right\">%s</TD>"\
			"<TD align=\"right\">%s</TD></TR>\r\n", 
		dest, seq, hopCount, nextHop, route, type);

	}
	
	return strdup(buf);
}


static char *__wifi_aodv_info_neigh( FILE *fp)
{
	char 	buf[8192*5];
	int 		length = 0;
	char 	line[1024];
	int		count;
	char		dest[64], link[64],valid[64],lifetime[64], t1[64],t2[64];

	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"right\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\n"
		"<TD align=\"right\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\n"
		"<TD align=\"right\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\n"
		"<TD align=\"right\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\n</TR>\r\n", 
		gettext("Address"), gettext("Link"), gettext("Valid"), gettext("Lifetime") );

	while(fgets(line,1024,fp))
	{
		count = sscanf(line,"%s %s %s %s %s %s", dest, link, valid, lifetime, t1, t2);
		if(count !=4)
			continue;
		length += CGI_SPRINTF(buf,length, "<TR><TD align=\"right\">%s</TD>\n"
			"<TD align=\"right\">%s</TD>"
			"<TD align=\"right\">%s</TD>"
			"<TD align=\"right\">%s</TD></TR>\r\n", 
		dest, link, valid, lifetime );

	}
	
	return strdup(buf);
}

static char *__wifi_aodv_info_timer( FILE *fp)
{
	char 	buf[8192*5];
	int 		length = 0;
	char 	line[1024];
	int		count;
	char		id[64], type[64], duration[64], retries[64], t1[64],t2[64];

	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"right\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\n"
		"<TD align=\"right\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\n"
		"<TD align=\"right\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\n"
		"<TD align=\"right\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\n</TR>\r\n", 
		gettext(CGI_STR_ID), gettext(CGI_STR_TYPE), gettext("Second/msecond"), gettext("Retries") );

	while(fgets(line,1024,fp))
	{
		count = sscanf(line,"%s %s %s %s %s %s", id, type, duration, retries, t1, t2);
		if(count !=4)
			continue;
		length += CGI_SPRINTF(buf,length, "<TR><TD align=\"right\">%s</TD>\n"
			"<TD align=\"right\">%s</TD>"
			"<TD align=\"right\">%s</TD>"
			"<TD align=\"right\">%s</TD></TR>\r\n", 
		id, type, duration, retries);

	}
	
	return strdup(buf);
}

static char *__wifi_aodv_info_flood( FILE *fp)
{
	char 	buf[8192*5];
	int 		length = 0;
	char 	line[1024];
	int		count;
	char		src[64], dest[64], id[64], duration[64], t1[64],t2[64];

	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"right\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\n"
		"<TD align=\"right\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\n"
		"<TD align=\"right\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\n"
		"<TD align=\"right\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\n</TR>\r\n",
		gettext(CGI_STR_SOURCE_ADDRESS), gettext(CGI_STR_DESTIN_ADDRESS), gettext("Flood ID"), gettext("Second/msecond"));

	while(fgets(line,1024,fp))
	{
		count = sscanf(line,"%s %s %s %s %s %s", src, dest, id, duration, t1, t2);
		if(count !=4)
			continue;
		length += CGI_SPRINTF(buf,length, "<TR><TD align=\"right\">%s</TD>\n"
			"<TD align=\"right\">%s</TD>"
			"<TD align=\"right\">%s</TD>"
			"<TD align=\"right\">%s</TD></TR>\r\n", 
		src, dest, id, duration);

	}
	
	return strdup(buf);
}

int	wifi_Aodv_Info(WIFI_INFO *info)
{
	char 	*cmd=NULL;
	char		*title;
	char		*msg;
	FILE		*fp;
	aodv_read_func_t readFunc;

	cmd = GET_VALUE( &info->cgiVariables, CGI_KEYWORD_OPERATOR);
	if( !cmd || !strlen(cmd) )
	{
		cgi_invalidate_page();
		return 0;
	}
	
	if(!strcasecmp(cmd, CGI_WIFI_OP_AODV_FLOOD) )
	{
		fp=fopen(WIFI_AODV_TBL_FLOODS, "r");
		title = "Flood Information";
		readFunc = __wifi_aodv_info_flood;
	}
	else if(!strcasecmp(cmd, CGI_WIFI_OP_AODV_TIMER) )
	{
		fp=fopen(WIFI_AODV_TBL_TIMERS, "r");
		title = "Timer Information";
		readFunc = __wifi_aodv_info_timer;
	}
	else if(!strcasecmp(cmd, CGI_WIFI_OP_AODV_NEIGH) )
	{
		fp=fopen(WIFI_AODV_TBL_NEIGHBORS, "r");
		title = "Neighbors Information";
		readFunc = __wifi_aodv_info_neigh;
	}
	else //if(!strcasecmp(cmd, CGI_WIFI_OP_AODV_ROUTE) )
	{
		fp=fopen(WIFI_AODV_TBL_ROUTES, "r");
		title = "Routes Information";
		readFunc = __wifi_aodv_info_route;
	}
	
	if(!fp)
	{
		cgi_error_page(NULL, gettext(CGI_STR_ERROR), title);
	}
	msg = (*readFunc)(fp);
	
	cgi_info_page(gettext("Status Information of Wireless AODV"), title, msg );

	fclose(fp);
	
	return 0;
}

