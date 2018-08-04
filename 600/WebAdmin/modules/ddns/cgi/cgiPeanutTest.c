/*
* $Id: cgiPeanutTest.c,v 1.1 2007/05/31 20:39:35 lizhijie Exp $
*/

#include "cgi_lib.h"
#include "cgiDdns.h"
#include "cgi_services.h"

static char *__ddns_test_status(PEANUT_INFO *info)
{
	char buf[4096];
	int length = 0;
	network_info	*netInfo;
	char	*ip;
	
	length += CGI_SPRINTF(buf,length, "<table width=\"85%%\" border=\"1\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\" bgcolor=\"#FFFFFF\" >\n");
	length += CGI_SPRINTF(buf, length, "<tr><td align=\"center\" bgcolor=\"#cccccc\" colspan=2><strong>%s</strong></td></tr>", 
		gettext(CGI_STR_TEST_RESULT));

	netInfo = cgi_get_network_info( 0);
	length += CGI_SPRINTF(buf, length, "<tr><td align=\"right\" bgcolor=\"#cccccc\"><strong>%s %s</strong></td><td >%s</td></tr>", 
		gettext(CGI_STR_WAN_PORT), gettext(CGI_STR_IP_ADDRESS), netInfo->wanInfo.addr);
	free(netInfo);

	ip = PeanutTestRegisterResult(info->fqdn);
	length += CGI_SPRINTF(buf, length, "<tr><td align=\"right\" bgcolor=\"#cccccc\"><strong>%s</strong></td><td >%s</td></tr>", 
		gettext("Registered FQDN"), info->fqdn);

	length += CGI_SPRINTF(buf, length, "<tr><td align=\"right\" bgcolor=\"#cccccc\"><strong>%s</strong></td><td >%s</td></tr>", 
		gettext("Resolved IP Address"), (ip==NULL)?"None":ip );

	if(ip==NULL)
	{
		length += CGI_SPRINTF(buf, length, "<tr><td colspan=2><font color=#ff0000>%s!%s!</font></td></tr>", 
			gettext("Resolved FQDN Failed"), gettext("Please check your configuration carefully") );
	}
	else
	{
		length += CGI_SPRINTF(buf, length, "<tr><td colspan=2>'%s' %s '%s'!</td></tr>", 
			info->fqdn, gettext("has been resolved as IP address"), ip );
	}
	
	return strdup(buf);
}


int cgiPeanutTest(PEANUT_INFO *info)
{
	int	pid;
	char		buf[1024];

	pid = cgi_get_program_pid_with_param(SERVICE_NAME_DDNSD, NULL);
	if(pid <= 0)
	{
		CGI_SPRINTF(buf, 0, "%s %s %s!", gettext(CGI_STR_PEANUT_NAME), gettext(CGI_STR_DAEMON), gettext("is not running now") );
		return cgi_error_page(NULL, gettext(CGI_STR_WARN), buf);
	}
	
	CGI_SPRINTF(buf, 0, "%s DDNS %s", gettext(CGI_STR_PEANUT_NAME), gettext(CGI_STR_TEST_RESULT) );
	cgi_info_page(buf, "", __ddns_test_status(info));
		
	return 0;	
}

