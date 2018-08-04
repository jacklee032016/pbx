/*
* $Id: help_msg.c,v 1.3 2007/05/31 20:38:05 lizhijie Exp $
*/

#include <stdio.h>
#include <string.h>
#include "cgi_lib.h"

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
		case CGI_HELP_MSG_UPLOAD_WWW:
		{
			length += sprintf(msg +length, "%s,", gettext("With this function"));
			length += sprintf(msg +length, "%s,", gettext("you can construct your Internet WWW site and"));
			length += sprintf(msg +length, "%s.", gettext("update it dynamically"));
			return strdup(msg);
		}
		case CGI_HELP_MSG_MAIL_ALIAS:
		{
			length += sprintf(msg +length, "%s", gettext("Aliases are another Mail name for system user"));
			return strdup(msg);
		}
		case CGI_HELP_MSG_SMTP:
		{
			length += sprintf(msg +length, "%s", gettext("SMTP(Simple Mail Transfer Protocol)Server"));
			return strdup(msg);
		}
		case CGI_HELP_MSG_POP_IMAP:
		{
			length += sprintf(msg +length, "%s", gettext("POP(Post Office Protocol)/IMAP(Internet Mail Access Protocol)" ));
			return strdup(msg);
		}
		case CGI_HELP_MSG_WWW:
		{
			length += sprintf(msg +length, "%s", gettext("WWW" ));
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
               case CGI_HELP_MSG_LAN_CONFIG:
		{
			length += sprintf(msg +length, "%s",gettext( "config the LAN IP"));
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
              case CGI_HELP_MSG_DISK_MGMT:
		{
			length += sprintf(msg +length, "%s",gettext(CGI_STR_HELP_NOT_DEFINED));
			return strdup(msg);
		}
              case CGI_HELP_MSG_QUICK_INSTALL:
		{
			length += sprintf(msg +length, "%s",gettext( "config the guide of quick install WAN"));
			return strdup(msg);
		}
		case CGI_HELP_MSG_QUICK_INSTALL_LAN:
		{
			length += sprintf(msg +length, "%s",gettext( "config the guide of quick install LAN"));
			return strdup(msg);
		}	  
               case CGI_HELP_MSG_LOCALE:
		{
			length += sprintf(msg +length, "%s",gettext(CGI_STR_HELP_NOT_DEFINED));
			return strdup(msg);
		}
             case CGI_HELP_MSG_SYSTIME:
		{
			length += sprintf(msg +length, "%s",gettext(CGI_STR_HELP_NOT_DEFINED));
			return strdup(msg);
		}
             case CGI_HELP_MSG_SERVICES_DHCP:
		{
			length += sprintf(msg +length, "%s",gettext(CGI_STR_HELP_NOT_DEFINED));
			return strdup(msg);
		}
	      case CGI_HELP_MSG_SERVICES_PPTP:
		{
			length += sprintf(msg +length, "%s",gettext(CGI_STR_HELP_NOT_DEFINED));
			return strdup(msg);
		}
	      case CGI_HELP_MSG_SERVICES_QOS:
		{
			length += sprintf(msg +length, "%s",gettext(CGI_STR_HELP_NOT_DEFINED));
			return strdup(msg);
		}
	      case CGI_HELP_MSG_SERVICES_UPNP:
		{
			length += sprintf(msg +length, "%s",gettext( CGI_STR_HELP_NOT_DEFINED));
			return strdup(msg);
		}
	      case CGI_HELP_MSG_SERVICES_RIP:
		{
			length += sprintf(msg +length, "%s",gettext(CGI_STR_HELP_NOT_DEFINED));
			return strdup(msg);
		}
	      case CGI_HELP_MSG_RECORD:
		{
			length += sprintf(msg +length, "%s",gettext( CGI_STR_HELP_NOT_DEFINED));
			return strdup(msg);
		}
		case CGI_HELP_MSG_SIPUA_PROTOCOL:
		{
			length += sprintf(msg +length, "%s",gettext(CGI_STR_HELP_NOT_DEFINED));
			return strdup(msg);
		}
		 case CGI_HELP_MSG_SIPUA_DIAL_RULE:
		{
			length += sprintf(msg +length, "%s",gettext(CGI_STR_HELP_NOT_DEFINED));
			return strdup(msg);
		}
		case CGI_HELP_MSG_BLACKLIST_MGMT:
		{
			length += sprintf(msg +length, "%s",gettext(CGI_STR_HELP_NOT_DEFINED));
			return strdup(msg);
		}
		case CGI_HELP_MSG_ROUTE:
		{
			length += sprintf(msg +length, "%s",gettext(CGI_STR_HELP_NOT_DEFINED));
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
		case CGI_HELP_MSG_UPLOAD_WWW:
		{
			length += sprintf(msg +length, "%s:<br>", gettext("Update WWW file involved 3 steps"));
			length += sprintf(msg +length, "1 :%s,", gettext("You should package your Web pages in the directory"));
			length += sprintf(msg +length, "%s.<br>", gettext("with index page of 'index.html'"));
			length += sprintf(msg +length, "2 :%s.<br>", gettext("You can package your Web pages with software of 'winrar' or 'tar' "));
			length += sprintf(msg +length, "3 :%s.<br>", gettext("After update, you should browsing it with your browser"));
			length += sprintf(msg +length, "%s<br>", gettext("Note : Domain Name(FQDN) for your WAN IP address is suggestted"));
			return strdup(msg);
		}
		case CGI_HELP_MSG_MAIL_ALIAS:
		{
			length += sprintf(msg +length, "* : %s.<br>", gettext("Aliases is very facilitye for system user to admin"));
		       length += sprintf(msg +length, "* : %s,%s.<br>", gettext("there are two kind for alias"), gettext("personal alias and alias group"));
                     length += sprintf(msg +length, "* : %s.<br>", gettext("alias is creater for system user"));
			length += sprintf(msg +length, "* : %s.<br>", gettext("one system user can used more aliases"));
			length += sprintf(msg +length, "* : %s,%s.<br>", gettext("group alias is places more system user in the same group"), gettext("in order to easy used"));
		       length += sprintf(msg +length, "* : %s.<br>", gettext("for example all employee of suppors can belong to the group of suppors"));

			return strdup(msg);
		}
		case CGI_HELP_MSG_SMTP:
		{
			length += sprintf(msg +length, "* : %s,%s,", gettext("SMTP server is used to transmits and accepts the mail"), gettext("in order to transmits and accepts the mail successful"));
		       length += sprintf(msg +length, "%s,%s<br>", gettext("you shoud apply domain name before used"), gettext("and assure the domain and MX (Mail eXtension) can analyze correctly"));
			length += sprintf(msg +length, "* : %s,", gettext("Generally your DNS is recorded www.abc.com, and MX is abc.com"));
			length += sprintf(msg +length, "%s.<br>", gettext("Must guarantee two names both can analyze the WAN address of your AS600"));
			length += sprintf(msg +length, "* : %s,%s.<br>", gettext("Generally your should set SMTP port nember to 25"), gettext("in order to guarantee other mails can transmit correctly to yours host"));
			return strdup(msg);
		}
		case CGI_HELP_MSG_POP_IMAP:
		{
		            length += sprintf(msg +length, "* : %s.<br>", gettext("POP3/IMAP is the common mail protocol"));
		            length += sprintf(msg +length, "*: %s,%s.<br>", gettext("According to yours custom and your mail software"), gettext("you can startup two services in sametime"));
			     length += sprintf(msg +length, "*: %s,%s.<br>", gettext("Generally for security of your mail"), gettext("you can modify the port nember of two srevers"));
		            return strdup(msg);
		}
		case CGI_HELP_MSG_WWW:
		{
		            length += sprintf(msg +length, "* : %s.<br>", gettext("POP3/IMAP is the common mail protocol"));
		            length += sprintf(msg +length, "*: %s,%s.<br>", gettext("According to yours custom and your mail software"), gettext("you can startup two services in sametime"));
			     length += sprintf(msg +length, "*: %s,%s.<br>", gettext("Generally for security of your mail"), gettext("you can modify the port nember of two srevers"));
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
               case CGI_HELP_MSG_LAN_CONFIG:
		{
			length += sprintf(msg +length, "* : %s.<br>", gettext("LAN IP:default 192.168.3.112"));
		       length += sprintf(msg +length, "* : %s", gettext("you can changes this IP, if you changed you must use new IP"));
                     length += sprintf(msg +length, "%s.<br>", gettext("to registe this AS600"));
                     length += sprintf(msg +length, "* : %s%s.<br>", gettext("LAN Netmask:default 255.255.255.0 "),gettext("in LAN other computer must to same as here"));
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
              case CGI_HELP_MSG_DISK_MGMT:
		{
			length += sprintf(msg +length, "%s",gettext( "All Services in our system are list here"));
			return strdup(msg);
		}
              case CGI_HELP_MSG_QUICK_INSTALL:
		{
			length += sprintf(msg +length, "* : %s%s.<br>", gettext(CGI_STR_HELP_IN_THIS_PAGE),gettext("the guide of quick install WAN"));
		       length += sprintf(msg +length, "* : %s.<br>", gettext("we have 3 way to connect to internet:static IP,DHCP,PPPOE"));
                     length += sprintf(msg +length, "* : %s.<br>", gettext("you choice anyone you want"));
                     length += sprintf(msg +length, "* : %s.<br>", gettext("click 'next' to set IP of WAN"));
			length += sprintf(msg +length, "* : %s.<br>", gettext("if you choice DHCP may obtain IP automatically from ISP"));
			length += sprintf(msg +length, "* : %s.<br>", gettext("click 'skip' skip goto LAN"));
			length += sprintf(msg +length, "* : %s.<br>", gettext("click 'finsh' finsh quick install"));
		       return strdup(msg);
		}
		case CGI_HELP_MSG_QUICK_INSTALL_LAN:
		{
			length += sprintf(msg +length, "* : %s%s.<br>", gettext(CGI_STR_HELP_IN_THIS_PAGE),gettext("the guide of quick install LAN"));
		       length += sprintf(msg +length, "* : %s.<br>", gettext("you can choice define or Redefine LAN Address"));
                     length += sprintf(msg +length, "* : %s.<br>", gettext("if you choicd define you must set server IP to define your AS600"));
			length += sprintf(msg +length, "* : %s.<br>", gettext("click 'finsh' finsh quick install")); 
			return strdup(msg);
		}	  
               case CGI_HELP_MSG_LOCALE:
		{
			length += sprintf(msg +length, "%s",gettext(CGI_STR_HELP_NOT_DEFINED));
			return strdup(msg);
		}
             case CGI_HELP_MSG_SYSTIME:
		{
			length += sprintf(msg +length, "%s",gettext(CGI_STR_HELP_NOT_DEFINED));
			return strdup(msg);
		}
             case CGI_HELP_MSG_SERVICES_DHCP:
		{
			length += sprintf(msg +length, "%s",gettext(CGI_STR_HELP_NOT_DEFINED));
			return strdup(msg);
		}
	      case CGI_HELP_MSG_SERVICES_PPTP:
		{
			length += sprintf(msg +length, "%s",gettext(CGI_STR_HELP_NOT_DEFINED));
			return strdup(msg);
		}
	      case CGI_HELP_MSG_SERVICES_QOS:
		{
			length += sprintf(msg +length, "%s",gettext(CGI_STR_HELP_NOT_DEFINED));
			return strdup(msg);
		}
	      case CGI_HELP_MSG_SERVICES_UPNP:
		{
			length += sprintf(msg +length, "%s",gettext(CGI_STR_HELP_NOT_DEFINED));
			return strdup(msg);
		}
	      case CGI_HELP_MSG_SERVICES_RIP:
		{
			length += sprintf(msg +length, "%s",gettext(CGI_STR_HELP_NOT_DEFINED));
			return strdup(msg);
		}
	      case CGI_HELP_MSG_RECORD:
		{
			length += sprintf(msg +length, "%s",gettext(CGI_STR_HELP_NOT_DEFINED));
			return strdup(msg);
		}
		case CGI_HELP_MSG_SIPUA_PROTOCOL:
		{
			length += sprintf(msg +length, "%s",gettext(CGI_STR_HELP_NOT_DEFINED));
			return strdup(msg);
		}
		 case CGI_HELP_MSG_SIPUA_DIAL_RULE:
		{
			length += sprintf(msg +length, "%s",gettext(CGI_STR_HELP_NOT_DEFINED));
			return strdup(msg);
		}
		case CGI_HELP_MSG_BLACKLIST_MGMT:
		{
			length += sprintf(msg +length, "%s",gettext(CGI_STR_HELP_NOT_DEFINED));
			return strdup(msg);
		}
		case CGI_HELP_MSG_ROUTE:
		{
			length += sprintf(msg +length, "%s",gettext(CGI_STR_HELP_NOT_DEFINED));
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


