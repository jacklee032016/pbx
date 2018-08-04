/*
* $Id: syscfg_hostname.c,v 1.3 2007/09/06 09:19:58 lizhijie Exp $
*/

#include "sysconf.h"

#define	CGI_HOSTNAME			"hostname"
#define	CGI_DOMAINNAME		"domainname"

static char *__cgi_output_hostname()
{
	FILE * fp;
	char line[1024];
	char * ret=NULL;
	char hostname[128];
	char fqdn[128];
	char ip[20];
	char msg[4096];
	int length = 0;

	if( (fp=fopen(HOSTNAME_CONF,"r") )==NULL)
	{
		cgidebug("open %s file error\n" , HOSTNAME_CONF);
		return NULL;
	}

#if 0
	length += sprintf(msg+length, "<tr><td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></td><td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></td><td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></td>",
		gettext("Hostname"), gettext("FQDN"), gettext(CGI_STR_IP_ADDRESS) );
#else
	length += sprintf(msg+length, "<tr><td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></td><td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></td><td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></td>",
		gettext("Hostname"), gettext("FQDN"), gettext(CGI_STR_IP_ADDRESS) );
#endif

	while( (ret=fgets(line,sizeof(line)-1,fp) )!=NULL)
	{
		if( (line[0]=='#') || (line[0]=='\0')|| (line[0]=='\r') || (line[0]=='\n') )
			continue;
		
		memset(hostname, 0, sizeof(hostname));
		memset(ip,0,sizeof(ip));
		if(sscanf(line,"%s %s %s",ip, hostname, fqdn )!= 3)
		{
			sscanf(line,"%s %s",ip, fqdn);
			hostname[0] = '\0';
		}
		
		if( strstr(hostname,"localhost")  || !strcmp(ip,"127.0.0.1") )
			continue ;
#if 0		
		length += sprintf(msg+length, "<tr><td align=\"center\"><font size=2>%s&nbsp </font></td>",hostname);
//		printf("<td align=\"center\"><font size=2>%s&nbsp </font></td>" ,fqdn);
		length += sprintf(msg+length, "<td align=\"center\"><font size=2>%s.%s&nbsp </font></td>" , hostname, cgi_domain_name_read() );
		length += sprintf(msg+length, "<td align=\"center\"><font size=2>%s&nbsp </font></td></tr>" ,ip); 
#else
		if(strlen(hostname) )
			length += sprintf(msg+length, "<tr><td align=\"center\"><font size=2>%s&nbsp </font></td>",hostname);
		else
			length += sprintf(msg+length, "<tr><td align=\"center\"><font size=2>%s&nbsp </font></td>",cgi_host_name_read() );

		length += sprintf(msg+length, "<td align=\"center\"><font size=2>%s </font></td>" , fqdn );
		length += sprintf(msg+length, "<td align=\"center\"><font size=2>%s&nbsp </font></td></tr>" ,ip); 
#endif
		memset(line,0,sizeof(line));
	}
	
	fclose(fp);

	return strdup(msg);
	
}

static char *__cgi_host_add_buttons()
{
	char buf[2048];
	int 	length = 0;

	length += sprintf(buf+length, "<form name=\"hostnameForm\" method=\"post\" action=\"%s\">\n", WEB_URL_SYSCONFIG );
	length += sprintf(buf+length, "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_KEYWORD_OPERATOR, CGI_SYSCFG_OP_HOSTNAME );
	length += sprintf(buf+length, "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_ACTION, CGI_ACTION_ACTIVATE );

	length += sprintf(buf+length, "<TR><TD  align=\"right\"><strong>%s</strong></TD>", 
		gettext("Host Name") );
	length += sprintf(buf+length, "<TD  align=\"left\"><input name=\"%s\" type=\"text\" size=\"15\" value=\"%s\" maxlength=\"50\"></TD></TR>\r\n",
		CGI_HOSTNAME, cgi_host_name_read() );

	length += sprintf(buf+length, "<TR><TD  align=\"right\"><strong>%s</strong></TD>", 
		gettext("Domain Name") );
	length += sprintf(buf+length, "<TD  align=\"left\"><input name=\"%s\" type=\"text\" size=\"15\" value=\"%s\" maxlength=\"50\"></TD></TR>\r\n", 
		CGI_DOMAINNAME, cgi_domain_name_read() );
	
	length += sprintf(buf+length, "<TR><TD align=\"right\">%s</TD><TD  align=\"left\">%s</TD></TR></TABLE>\r\n\r\n",
		cgi_submit_button(gettext(CGI_STR_ACTIVATE) ), cgi_help_button(CGI_HELP_MSG_HOST_NAME ));

	length += CGI_SPRINTF(buf,length, "<table width=\"85%%\" border=\"0\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\" bgcolor=\"#FFFFFF\" >\n");
	length += CGI_SPRINTF(buf,length, "<TR><TD><strong>%s</strong><br>* %s, %s!</TD></TR>\r\n",
		gettext(CGI_STR_NOTE), gettext("When DDNS such as Peant Hull is activating"), gettext("FQDN input here will be used by our DDNS program") );

	return strdup(buf);
}


int sysconfig_hostname(SYSCFG_INFO *info)
{
	char *host=NULL;
	char *domain = NULL;
	char cmd[128];
	char *action;
#if 0	
	eth_info eth;
	eth_info * peth;
	peth=&eth;
	memset(&eth,0,sizeof(eth) );
#endif

	action = GET_VALUE(&info->cgiVariables, CGI_ACTION );
	if(!strcasecmp(action, CGI_ACTION_ACTIVATE) )
	{
		host=GET_VALUE(&info->cgiVariables,CGI_HOSTNAME );
		if(host != NULL && strlen(host)!= 0)
		{
			cgi_host_name_write( host);
		}
		else 
			return cgi_error_page(NULL, gettext("Not Found"), gettext("Field of hostname is not found") );

		domain = GET_VALUE( &info->cgiVariables, CGI_DOMAINNAME);
		if(domain!= NULL && strlen(domain)!= 0)
		{
			cgi_domain_name_write(domain);
		}
		else
			return cgi_error_page(NULL, gettext("Not Found"), gettext("Field of domainname is not found") );
		
		sprintf(cmd,"/bin/hostname %s",host);
		system(cmd);

		cgi_syscfg_update_host_conf();
	}

	cgi_info_page(gettext("Host and Domain Configuration"), __cgi_output_hostname(), __cgi_host_add_buttons() );
	
	return 0;
}

