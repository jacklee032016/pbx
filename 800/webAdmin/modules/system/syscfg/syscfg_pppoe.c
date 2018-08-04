/*$Id: syscfg_pppoe.c,v 1.7 2007/09/21 21:22:12 lizhijie Exp $*/

#include "sysconf.h"

#define	ADSL_DEVICE			"ADSL_DEVICE"
#define	ADSL_LOCAL				"ADSL_LocalIp"
#define	ADSL_REMOTE			"ADSL_RemoteIp"
#define	ADSL_DNS1				"ADSL_DNS1"
#define	ADSL_DNS2				"ADSL_DNS2"
#define	ADSL_BYTE_SEND		"ADSL_BYTES_SEND"
#define	ADSL_BYTE_RECV		"ADSL_BYTES_RCVD"
#define	ADSL_LENGTH			"ADSL_CONNECT_TIME"

static char *__syscfg_pppoe_status_info(void)
{
	llist 		runtime;
	char		*value;
	char		buf[4096];
	int		length = 0;
	
	if (readconfig(VPN_LOG_FILE, ITEM_EQU, NULL, &runtime) != 0)
	{
//		cgi_error_page(NULL, gettext(CGI_STR_ERROR), gettext("No Config of VPN is found") );
	}
	value = GET_VALUE(&runtime, ADSL_DEVICE);

	length += CGI_SPRINTF(buf, length, "<tr><td align=\"right\" bgcolor=\"#cccccc\"><strong>%s</strong></td><td>%s</td></tr>", 
		gettext("ADSL Device"), value );
	if(strcasecmp(value, "down") )
	{
		length += CGI_SPRINTF(buf, length, "<tr><td align=\"right\" bgcolor=\"#cccccc\"><strong>%s</strong></td><td>%s</td></tr>", 
			gettext("Local IP Address"), GET_VALUE(&runtime, ADSL_LOCAL) );
		length += CGI_SPRINTF(buf, length, "<tr><td align=\"right\" bgcolor=\"#cccccc\"><strong>%s</strong></td><td>%s</td></tr>", 
			gettext("Peer IP Address"), GET_VALUE(&runtime, ADSL_REMOTE) );
		length += CGI_SPRINTF(buf, length, "<tr><td align=\"right\" bgcolor=\"#cccccc\"><strong>%s</strong></td><td>%s</td></tr>", 
			gettext("Primary DNS Server"), GET_VALUE(&runtime, ADSL_DNS1) );
		length += CGI_SPRINTF(buf, length, "<tr><td align=\"right\" bgcolor=\"#cccccc\"><strong>%s</strong></td><td>%s</td></tr>", 
			gettext("Secondary DNS Server"), GET_VALUE(&runtime, ADSL_DNS2) );
	}
	else
	{
		length += CGI_SPRINTF(buf, length, "<tr><td align=\"right\" bgcolor=\"#cccccc\"><strong>%s</strong></td><td>%s</td></tr>", 
			gettext("Connection(Seconds)"), GET_VALUE(&runtime, ADSL_LENGTH) );
		length += CGI_SPRINTF(buf, length, "<tr><td align=\"right\" bgcolor=\"#cccccc\"><strong>%s</strong></td><td>%s</td></tr>", 
			gettext("Send(Bytes)"), GET_VALUE(&runtime, ADSL_BYTE_RECV) );
		length += CGI_SPRINTF(buf, length, "<tr><td align=\"right\" bgcolor=\"#cccccc\"><strong>%s</strong></td><td>%s</td></tr>", 
			gettext("Rcvd(Bytes)"), GET_VALUE(&runtime, ADSL_BYTE_SEND) );
	}

	return strdup(buf);
}

char *syscfg_pppoe_checkup()
{
	char		r_buff[128];
	FILE		*fp = (FILE *)NULL;
	int		isError = 2;
	char 	*msg = NULL;
	char 	buf[4048];
	int 		length = 0;

	if ((fp = fopen(PPPOE_LOG, "r") ) == (FILE *)NULL)
	{
		cgi_error_page(NULL, gettext(CGI_STR_ERROR), gettext("User/Password error") );
		return NULL;
	}
			
	length += sprintf(buf+length, "<tr><td colspan=2 align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></td></tr>", gettext("Current PPPOE Configuration") );
	while(fgets(r_buff, sizeof(r_buff), fp) != NULL)
	{
		if (strstr(r_buff, PPPOE_MSG_OK) )
		{
#if 1
			msg = __syscfg_pppoe_status_info();
			isError = 0;
#else
			char t1[32],t2[32], t3[32], ip[32];
			fscanf(fp, "%s %s %s %s", t1, t2, t3, ip);
			length += sprintf(buf+length, "<tr><td bgcolor=\"#cccccc\"><strong>%s</strong></td><td>%s</td></tr>", gettext("Local IP Address"), ip );
			fscanf(fp, "%s %s %s %s", t1, t2, t3, ip);
			length += sprintf(buf+length, "<tr><td bgcolor=\"#cccccc\"><strong>%s</strong></td><td>%s</td></tr>", gettext("Peer IP Address"), ip );
			fscanf(fp, "%s %s %s %s", t1, t2, t3, ip);
			length += sprintf(buf+length, "<tr><td bgcolor=\"#cccccc\"><strong>%s</strong></td><td>%s</td></tr>", gettext("Primary DNS Server"), ip );
//			cgi_info_page("PPPOE Success", buf, NULL);
			msg = strdup(buf);	
			isError = 0;
#if 0			
			break;
#endif
#endif
		}	
		else
		{
			if (strstr(r_buff, PPPOE_MSG_ERR_NONET)  )
			{
				char info[256];
				int len = 0;
				isError = 1;
				len += sprintf(info+len, "%s<p>", gettext("Timeout") );
				len += sprintf(info+len, "%s<br>", gettext("Maybe your network cable is not connected") );
				len += sprintf(info+len, "%s", gettext("Or PPPOE Server is not available") );

				msg = strdup(info);
				
#if 0			
				break;
#endif
			}
			else if(strstr(r_buff, PPPOE_MSG_ERR_REFUSE)  )
			{
				isError = 1;
				msg = gettext("User/Password error");
#if 0			
				break;
#endif
			}
			else if(strstr(r_buff, PPPOE_MSG_ERR_FAIL)  )
			{
				isError = 1;
				msg = gettext("Authentication Failed") ;
#if 0			
				break;
#endif
			}			
		}
	}
	
	if( isError)
	{
		if(isError == 1)
		{
			system("killall -9 pppoe-connect");
			system("killall -9 pppd");
			length += sprintf(buf+length, "<tr><td colspan=2><strong><font color=\"#ff0000\">%s</font></strong><br><p>", gettext(CGI_STR_FAIL) );
			length += sprintf(buf+length, "<font color=\"#ff0000\">%s</font></td></tr>", msg);
			msg = strdup(buf);
		}
		else
		{
			char target[256];
			length += sprintf(buf+length, "<tr><td colspan=2><strong><font color=\"#ff0000\">%s</font></strong><br><p>", gettext(CGI_STR_WARN) );
			length += sprintf(buf+length, "<font color=\"#ff0000\">%s.<br></font>", gettext("PPPOE is not startup until now") );
			sprintf(target, "<A href=\"%s?%s=%s&%s=%s&%s=%s\">%s</A>", WEB_URL_SYSCONFIG, NIC_KEYWORD_DEVICE, ETH0, 
				CGI_KEYWORD_OPERATOR, CGI_SYSCFG_OP_WAN, CGI_ACTION, CGI_ACTION_MODIFY, gettext(CGI_STR_ETH_CONFIG));
			length += sprintf(buf+length, "%s %s", gettext("Please wait a moment and recheck it with"), target );
			length += sprintf(buf+length, "</td></tr>" );
			msg = strdup(buf);
		}
	}
	
	fclose(fp);
	
	return msg;
}

static char *__cgi_pppoe_add_buttons(SYSCFG_INFO *info)
{
	char buf[4096];
	int length = 0;

	length += sprintf(buf+length, "<form name=\"pppoeForm\" method=\"post\" action=\"%s\">\n", WEB_URL_SYSCONFIG );
	length += sprintf(buf+length, "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n",  CGI_KEYWORD_OPERATOR, CGI_SYSCFG_OP_WAN );
	length += sprintf(buf+length, "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n",  CGI_ACTION, CGI_ACTION_ACTIVATE );

	length += sprintf(buf+length, "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", NIC_KEYWORD_DEVICE, ETH0 );
	length += sprintf(buf+length, "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", NIC_KEYWORD_PROTOCOL, NIC_KEYWORD_PROTOCOL_PPPOE);

	/* server name */
	length += sprintf(buf+length, "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", PPPOE_KEYWORD_SERVER_NAME, PPPOE_KEYWORD_SERVER_NAME_ASTERIC);
	/* User/Passwd */
	length += sprintf(buf+length, "<TR><TD align=\"right\"><strong>%s:</strong></TD>\n", 
		gettext(CGI_STR_USER_NAME) );
	length += sprintf(buf+length, "<TD><input name=\"%s\"  type=\"text\" size=\"15\" value=\"%s\" maxlength=\"50\"></TD></TR>\r\n", 
		PPPOE_KEYWORD_USER, GET_VALUE(&info->pppoeCfgs, PPPOE_KEYWORD_USER) );

	length += sprintf(buf+length, "<TR><TD align=\"right\"><strong>%s:</strong></TD>\n", 
		gettext(CGI_STR_PASSWORD) );
//	length += sprintf(buf+length, "<TD ><input name=\"%s\" onKeyPress=\"checkNumber(this)\" onChange=\"checkIP(NETMASK.value,this)\"  type=\"password\" size=\"15\" value=\"%s\" maxlength=\"50\"></TD></TR>\r\n", 
	length += sprintf(buf+length, "<TD ><input name=\"%s\" type=\"password\" size=\"15\" value=\"%s\" maxlength=\"50\"></TD></TR>\r\n", 
		PPPOE_KEYWORD_PASSWORD, GET_VALUE(&info->pppoeCfgs,PPPOE_KEYWORD_PASSWORD) );

	/* ISP */
	length += sprintf(buf+length, "<TR><TD align=\"right\"><strong>%s:</strong></TD>\n", 
		gettext("ISP Server") );
	length += sprintf(buf+length, "<TD><input name=\"%s\" onKeyPress=\"checkNumber(this)\" onChange=\"checkIP(%s.value,this)\"  type=\"text\" size=\"15\" value=\"%s\" maxlength=\"50\"></TD></TR>\r\n", 
		PPPOE_KEYWORD_ACNAME, PPPOE_KEYWORD_ACNAME, GET_VALUE(&info->pppoeCfgs, PPPOE_KEYWORD_ACNAME ) );

	length += sprintf(buf+length, "<TR><TD  align=\"right\"><strong>%s:</strong></TD>\n", 
		gettext("ISP Service Name") );
	length += sprintf(buf+length, "<TD><input name=\"%s\" onKeyPress=\"checkNumber(this)\" onChange=\"checkIP(%s.value,this)\"  type=\"text\" size=\"15\" value=\"%s\" maxlength=\"50\"></TD></TR>\r\n", 
		PPPOE_KEYWORD_SERVICENAME, PPPOE_KEYWORD_SERVICENAME, GET_VALUE(&info->pppoeCfgs, PPPOE_KEYWORD_SERVICENAME ) );

	length += sprintf(buf+length, "<TR><TD colspan=2><br><br></strong></TD></TR>\n");


	/* DNS */
	length += sprintf(buf+length, "<TR><TD align=\"right\"><strong>%s:</strong></TD>\n", 
		gettext("DNS Mode") );
	length += sprintf(buf+length, "<TD><select name=\"%s\"><option value=\"SPECIFY\" %s>%s<option value=\"SERVER\" %s>%s</select></td></TR>\r\n", 
		PPPOE_KEYWORD_DNSTYPE, ( strcasecmp(PPPOE_KEYWORD_DNSTYPE_USER, GET_VALUE(&info->pppoeCfgs, PPPOE_KEYWORD_DNSTYPE)) )?"":"selected" ,
		gettext("Set By User"), 
		( strcasecmp(PPPOE_KEYWORD_DNSTYPE_USER, GET_VALUE(&info->pppoeCfgs, PPPOE_KEYWORD_DNSTYPE)) )?"selected":"" ,
		gettext("Set By Server") );
	
	length += sprintf(buf+length, "<TR><TD align=\"right\"><strong>%s:</strong></TD>\n", 
		gettext(CGI_STR_FIRST_DNS_SERVER) );
	length += sprintf(buf+length, "<TD><input name=\"%s\" onKeyPress=\"checkNumber(this)\" onChange=\"checkIP(%s.value,this)\"  type=\"text\" size=\"15\" value=\"%s\" maxlength=\"50\"></TD></TR>\r\n", 
		PPPOE_KEYWORD_DNS1, PPPOE_KEYWORD_DNS1, GET_VALUE(&info->pppoeCfgs, PPPOE_KEYWORD_DNS1 ) );

	length += sprintf(buf+length, "<TR><TD  align=\"right\"><strong>%s:</strong></TD>\n", 
		gettext(CGI_STR_SECOND_DNS_SERVER) );
	length += sprintf(buf+length, "<TD><input name=\"%s\" onKeyPress=\"checkNumber(this)\" onChange=\"checkIP(%s.value,this)\"  type=\"text\" size=\"15\" value=\"%s\" maxlength=\"50\"></TD></TR>\r\n", 
		PPPOE_KEYWORD_DNS2, PPPOE_KEYWORD_DNS2, GET_VALUE(&info->pppoeCfgs, PPPOE_KEYWORD_DNS2 ) );


	length += sprintf(buf+length, "<TR><TD align=\"right\">\r\n");
	length += sprintf(buf+length, cgi_submit_button(gettext(CGI_STR_SAVE) ) );
	length += sprintf(buf+length, "</TD>\r\n");
#if 0
	length += sprintf(buf+length, "<TR><TD align=\"center\">\r\n");
	length += sprintf(buf+length, cgi_reset_button(gettext(CGI_STR_RESET) ) );
	length += sprintf(buf+length, "</TD>\r\n");
#endif

	length += sprintf(buf+length, "<TD>\r\n");
	length += sprintf(buf+length, cgi_help_button(CGI_HELP_MSG_PPPOE_CONFIG ) );
	length += sprintf(buf+length, "</TD></TR>\r\n");

	return strdup(buf);
}


int sysconfig_pppoe( SYSCFG_INFO *info)
{
	char cmd[128];
	char	*action = GET_CGI_ACTION(&info->cgiVariables );
	
	if( !strcasecmp(action, CGI_ACTION_ADD) )
	{/* PPPOE paramters after select protocol */
		cgi_info_page( gettext("PPPOE Configuration"), gettext("Set all parameters need for PPPOE connection in this page"), 
			__cgi_pppoe_add_buttons(info) );

		return 0;
	}
	else if( !strcasecmp(action, CGI_ACTION_ACTIVATE) )
	{/* activate PPPOE after parameters setup */
		cgi_syscfg_wan_activate_pppoe( &info->cgiVariables);

		sprintf(cmd, "%s?%s=%s&%s=%s&%s=%s",WEB_URL_SYSCONFIG, 
			NIC_KEYWORD_DEVICE,ETH0,  CGI_KEYWORD_OPERATOR, CGI_SYSCFG_OP_WAN, CGI_ACTION, CGI_ACTION_MODIFY);
		cgi_refresh_page(10, cmd, gettext(CGI_STR_WAIT), gettext("PPPOE is connecting to server.....") );

		return 0;
	}
	else if(!strcasecmp(action, CGI_ACTION_MODIFY) )
	{/* view PPPOE status */
		char *msg = syscfg_pppoe_checkup();
		cgi_info_page(gettext("PPPOE Configuration"), msg, "");

		return 0;
	}

	cgi_invalidate_page();
	
	return 0;
}

