/*
* $Id: fwConnTrack.c,v 1.4 2007/02/07 19:04:16 lizhijie Exp $
*/

#include "cgi_lib.h"
#include "cgi_iptables.h"

#define		CONN_TRACK_FILE	"/proc/net/ip_conntrack"

struct _conn_track
{
	char		protocol[32];
	char		timeout[32];
	char		status[32];

	char		srcAddress[32];
	char		srcPort[32];
	
	char		destAddress[32];
	char		destPort[32];

	struct _conn_track *next;
};

typedef struct _conn_track 		conn_track_t;

conn_track_t	*_currentConntions;


static conn_track_t *__connTrack_read()
{
	FILE * fp;
	char line[1024];
	char	protoId[32];
	char value[5][32];
	char *t;
	int i=0;
	char *linelen;
	conn_track_t *header= NULL, *p, *last=NULL;

	if( ( fp=fopen(CONN_TRACK_FILE,"r") )==NULL )
	{
		fprintf(stderr,"open file %s error", CONN_TRACK_FILE);
		return NULL;
	}

	while ( (linelen = fgets(line, PARSE_FILE_MAX_LENGTH, fp) ) )
	{
		p = (conn_track_t *)malloc(sizeof(conn_track_t) );
		if(!p)
			break;
		memset(p, 0, sizeof(conn_track_t) );
		
		sscanf(line,"%s %s %s %s %s %s %s %s",
			p->protocol, protoId, p->timeout, value[0], value[1], value[2], value[3],value[4]);
		
		if(!strcasecmp( p->protocol, IPTABLES_PROTOCOL_NAME_TCP) )
		{
			sprintf(p->status, "%s", value[0]);
			i = 1;
		}
		else if(!strcasecmp(p->protocol, IPTABLES_PROTOCOL_NAME_TCP) )
		{
			sprintf(p->status, "%s", "");
			i = 0;
		}
		else
		{
			break;
		}
		
		t = strchr( value[i], '=');
		if(t)
			sprintf(p->srcAddress, ++t );
		i++;
		t = strchr( value[i], '=');
		if(t)
			sprintf(p->destAddress, ++t );
		
		i++;
		t = strchr( value[i], '=');
		if(t)
			sprintf(p->srcPort, ++t );
		i++;
		t = strchr( value[i], '=');
		if(t)
			sprintf(p->destPort, ++t );

		if(header==NULL)
			header = p;
		if(last)
			last->next = p;

		last = p;
	}
	fclose(fp);

	return header;
}

static char *__connTrack_status_list( )
{
	char buf[4096*100];
	int 	length = 0;
	char	target[256];
	conn_track_t *con;

	if(!_currentConntions)
	{
		length += CGI_SPRINTF(buf,length, "<tr><td bgcolor=\"#cccccc\"><strong>%s:</strong><BR>\n", 
			gettext(CGI_STR_NOTE) );
		length += CGI_SPRINTF(buf,length, "%s!<br>%s, %s</td></TR>\n",
			gettext("No Active connection currently"), gettext("Maybe your network has some problem"), gettext("Please check it") );
	}
	else
	{
		length += CGI_SPRINTF(buf,length, "<tr><td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD><td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD><td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD><td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD><td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD><td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD><td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\n</TR>\n", 
			gettext(CGI_STR_PROTOCOL),gettext("Timeout"),gettext(CGI_STR_STATUS),gettext(CGI_STR_SOURCE_ADDRESS), gettext("Source Port"),gettext("Dest Address"), gettext("Dest Port") );

		con = _currentConntions;
		while(con)
		{
cgitrace;
			length += CGI_SPRINTF(buf,length, "<TR><td align=\"center\">%s</td><td align=\"center\">%s</td><td align=\"center\">%s</td><td align=\"center\">%s</td><td align=\"center\">%s</td><td align=\"center\">%s</td><td align=\"center\">%s</td></TR>\n",
				con->protocol, con->timeout, con->status, con->srcAddress, con->srcPort, con->destAddress, con->destPort);
			con = con->next;
		}
	}

	sprintf(target, "%s?%s=%s", WEB_URL_FIREWALL, CGI_KEYWORD_OPERATOR, FIREWALL_OP_TRACK);
	length += CGI_SPRINTF(buf,length,"\n<script language=\"JavaScript\">\n");
	length += CGI_SPRINTF(buf,length,"<!--\nsetTimeout('window.location.replace(\"%s\")', %d)\n", 
		target, 60*1000);
	length += CGI_SPRINTF(buf,length,"//-->\n</script>\n");
cgitrace;

	return strdup(buf);
}


int cgiFw_ConnTrack(FW_INFO *fwInfo)
{
	_currentConntions = __connTrack_read();

	cgi_info_page(gettext("Connections Track"), "",  __connTrack_status_list() );

	return 0;	
}

