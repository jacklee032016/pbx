/*
* $Id: help_msg.c,v 1.3 2007/05/31 20:38:05 lizhijie Exp $
*/

#include <stdio.h>
#include <string.h>
#include "cgiLib.h"

char *cgi_help_getLocaleHeader(cgi_help_type_t type)
{
	char msg[2048];
	int length = 0;
	
	switch (type)
	{
		case CGI_HELP_MSG_UPLOAD_ADMIN:
		{
			length += sprintf(msg +length, "%s,", gettext("With upload function" ));
			length += sprintf(msg +length, "%s,", gettext("the crupted system can be restored or patched packages"));
			length += sprintf(msg +length, "%s.", gettext("can be upgrade system" ));
			return strdup(msg);
		}
		case CGI_HELP_MSG_SERVICES:
		{
			length += sprintf(msg +length, "%s",gettext( "All Services in our system are list here"));
			return strdup(msg);
		}
              case CGI_HELP_MSG_SYSTEM_USER:
		{
			length += sprintf(msg +length, "%s",gettext( "System user admin"));
			return strdup(msg);
		}
              case CGI_HELP_MSG_HOST_NAME:
		{
			length += sprintf(msg +length, "%s",gettext( "hostname and Domain admin"));
			return strdup(msg);
		}

              case CGI_HELP_MSG_WAN_CONFIG:
		{
			length += sprintf(msg +length, "%s",gettext( "config the WAN IP"));
			return strdup(msg);
		}
		 case CGI_HELP_MSG_STATIC_WAN_CONFIG:
		{
			length += sprintf(msg +length, "%s",gettext( "WAN static IP config"));
			return strdup(msg);
		}	  
              case CGI_HELP_MSG_PPPOE_CONFIG:
		{
			length += sprintf(msg +length, "%s",gettext( "config the PPPOE"));
			return strdup(msg);
		}

		case CGI_HELP_MSG_INVALIDATE:
		{
			length += sprintf(msg +length, "%s",gettext(CGI_STR_HELP_NOT_DEFINED));
			return strdup(msg);
		}

		default:
			return NULL;
	}

	return NULL;
}

char *cgi_help_getLocaleMsg(cgi_help_type_t type)
{
	char msg[4096];
	int length = 0;
	
	switch (type)
	{
		case CGI_HELP_MSG_UPLOAD_ADMIN:
		{
			length += sprintf(msg +length, "%s:<br>", gettext("Update/Restore involved 3 steps") );
			length += sprintf(msg +length, "1 : %s,", gettext("Login in our web site with your browser(or contact with us)"));
			length += sprintf(msg +length, "%s.<br>", gettext("and get the package you need"));
			length += sprintf(msg +length, "2 : %s.<br>", gettext("Select the menu item of 'Upgrade Software'"));
			length += sprintf(msg +length, "3 : %s.<br>", gettext("After update, you should reboot system"));
			return strdup(msg);
		}

		case CGI_HELP_MSG_SERVICES:
		{
			length += sprintf(msg +length, "* : %s,", gettext("In order to config a server yuo must know the state of services"));
			length += sprintf(msg +length, " %s.<br>", gettext("in there we can look over all state of services in our system"));
		       length += sprintf(msg +length, "* : %s.<br>", gettext("you can stop or runing every services in our system"));
                     length += sprintf(msg +length, "* : %s.<br>", gettext("you can choose upload or no services when the system startup"));
			return strdup(msg);
		}
		 case CGI_HELP_MSG_SYSTEM_USER:
		{
                     length += sprintf(msg +length, "* : %s%s.<br>", gettext(CGI_STR_HELP_IN_THIS_PAGE),gettext("the usename and password in webadmin"));
		       length += sprintf(msg +length, "* : %s.<br>", gettext("Username: generally used numeral,Letter and special characte"));
                     length += sprintf(msg +length, "* : %s.<br>", gettext("Password: composed by the numeral and letter"));
                     length += sprintf(msg +length, "* : %s.<br>", gettext("Note: it is differ to capital letter and small letter"));
			return strdup(msg);		
		}
              case CGI_HELP_MSG_HOST_NAME:
		{
			length += sprintf(msg +length, "* : %s%s.<br>",gettext( CGI_STR_HELP_IN_THIS_PAGE),gettext("the hostname and domain"));
			length += sprintf(msg +length, "* : %s.<br>",gettext( "Homename and WAN IP address are correspond mutually"));
                     length += sprintf(msg +length, "* : %s.<br>", gettext("Hostname and domain generally used numeral,letter and special characte"));
                     length += sprintf(msg +length, "* : %s,", gettext("If you want to used WWW and MAIL services you must used this domain"));
                     length += sprintf(msg +length, " %s.<br>", gettext("and hostname which you already applied and registed"));
			length += sprintf(msg +length, "* : %s.", gettext("Note: it is differ to capital letter and small letter"));
			length += sprintf(msg +length, " %s.<br>", gettext("In order to FQDN can use, must config the DNS server correctly"));
			return strdup(msg);
		}

              case CGI_HELP_MSG_WAN_CONFIG:
		{
			length += sprintf(msg +length, "* : %s%s.<br>", gettext(CGI_STR_HELP_IN_THIS_PAGE),gettext("the way to connect the Internat"));
		       length += sprintf(msg +length, "* : %s.<br>", gettext("we have 3 way to connect to internet:static IP,DHCP,PPPOE"));
                     length += sprintf(msg +length, "* : %s.<br>", gettext("you choice anyone you want"));
                     length += sprintf(msg +length, "* : %s.<br>", gettext("click 'ok' to start config"));
			length += sprintf(msg +length, "* : %s.<br>", gettext("if you choice DHCP may obtain IP automatically from ISP"));		 
			return strdup(msg);
		}
		 case CGI_HELP_MSG_STATIC_WAN_CONFIG:
		{
			length += sprintf(msg +length, "* : %s%s.<br>", gettext(CGI_STR_HELP_IN_THIS_PAGE),gettext("the static wan IP"));
		       length += sprintf(msg +length, "* : %s.<br>", gettext("WAN IP provides by ISP"));
                     length += sprintf(msg +length, "* : %s.<br>", gettext("WAN netmask generally used 255.255.255.0"));
                     length += sprintf(msg +length, "* : %s.<br>", gettext("Gateway IP namely goal address"));
		       length += sprintf(msg +length, "* : %s.<br>", gettext("First DNS Server provides by ISP"));
		       length += sprintf(msg +length, "* : %s.<br>", gettext("Second DNS Server provides by ISP also may not fill in"));   
			return strdup(msg);
		}	  
              case CGI_HELP_MSG_PPPOE_CONFIG:
		{
			length += sprintf(msg +length, "* : %s%s.<br>", gettext(CGI_STR_HELP_IN_THIS_PAGE),gettext("the PPPOE"));
		       length += sprintf(msg +length, "* : %s.<br>", gettext("PPPOE is ADSL"));
                     length += sprintf(msg +length, "* : %s,%s.<br>", gettext("usedname and password is provides by ISP"),gettext("if you don't know can ask ISP"));
                     length += sprintf(msg +length, "* : %s,%s.<br>", gettext("ISP Server and ISP server name is provides by ISP"),gettext("if you don't know can ask ISP"));
			length += sprintf(msg +length, "* : %s,%s.<br>", gettext("you have two way to get DNS server"),gettext("Set by User and Set by server"));
                     length += sprintf(msg +length, "* : %s,%s.<br>", gettext("if you choice set by user"),gettext("you must set First DNS Server and Second DNS Server"));		 
		       length += sprintf(msg +length, "* : %s.<br>", gettext("if you choice set by server you can not set that"));
			length += sprintf(msg +length, "* : %s.<br>", gettext("First DNS Server provides by ISP"));   
		       length += sprintf(msg +length, "* : %s.<br>", gettext("Second DNS Server provides by ISP also may not fill in"));   
                     length += sprintf(msg +length, "* : %s,", gettext("Note: ISP can alloca IP address dynamocly for ADSL"));
			length += sprintf(msg +length, " %s.<br>", gettext("so you can not set ISP server and ISP server name"));
			return strdup(msg);
		}
			  
		case CGI_HELP_MSG_INVALIDATE:
		{
			length += sprintf(msg +length, "%s",gettext(CGI_STR_HELP_NOT_DEFINED));
			return strdup(msg);
		}

		default:
			return NULL;
	}

	return NULL;
}


