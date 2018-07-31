/*
* $Id: wifiAcl.c,v 1.9 2007/09/06 09:19:59 lizhijie Exp $
*/

#include "cgiLib.h"
#include "cgi_wifi.h"
#include "cgi_syscfg.h"

#define	WIFI_ACL_PROC					"/proc/sys/net/"

struct	acl_policy_cmd
{
	int			type;
	char			*description;
};

struct acl_policy_cmd _aclPolicies[] =
{
	{
		CGI_WIFI_ACL_OPEN, 
		CGI_WIFI_ACL_OPEN_STR
	},
	{
		CGI_WIFI_ACL_ALLOW, 
		CGI_WIFI_ACL_ALLOW_STR
	},
	{
		CGI_WIFI_ACL_DENY, 
		CGI_WIFI_ACL_DENY_STR
	},
	{
		CGI_WIFI_ACL_FLUSH, 
		CGI_WIFI_ACL_FLUSH_STR
	},
	{
		CGI_WIFI_ACL_DEATTACH, 
		CGI_WIFI_ACL_DEATTACH_STR
	},
	{
		-1, 
		NULL
	},
};

#define	__ACL_POLICY_COMMAND		"/sbin/wifipriv %s setparam 17 %d"
#define	__ACL_ADDRESS_COMMAND	"/sbin/wifipriv %s %s %s"

#define	__ACL_ADD	1
#define	__ACL_DEL	0

static int	__acl_command_policy(WIFI_INFO *info)
{
	wifi_acl_info_t *acl = &info->aclInfo;
	struct acl_policy_cmd *_policy = _aclPolicies;
	int policy = 0;
	char	cmd[128], *value;
	int i;
	
	value = GET_VALUE( &info->cgiVariables, WIFI_KEYWORD_ACL_POLICY );
	list_replace_value_by_key( &acl->macCfgs, WIFI_KEYWORD_ACL_POLICY, value);

	if(writeconfig(WIFI_CONFIG_ACL_FILE, ITEM_EQU, NULL, &acl->macCfgs) )
	{
		cgi_error_page(NULL, gettext("Wireless Operation Failed"), gettext( CGI_STR_SOME_ERROR ) );
		return -1;
	}

	for(i=0; i< CGI_WIFI_ACL_FLUSH; i++)
	{
		if(!strcasecmp(_policy->description, value) )
		{
			policy = _policy->type;
		}
		_policy++;
	}
	
//	sprintf(cmd, __ACL_POLICY_COMMAND, info->wifiInfo.b.name, policy);
	sprintf(cmd, __ACL_POLICY_COMMAND, GET_VALUE(&info->nicCfgs, NIC_KEYWORD_DEVICE), policy);
	system(cmd);

	return 0;
}

static int	__acl_command_mac_address(int action/*1 : add; 0 : delete */, WIFI_INFO *info)
{
	wifi_acl_info_t *acl = &info->aclInfo;
	char	cmd[128], *value, *t;
	int	i;
#if 0	
	int 	count = acl->macCfgs;
#else
	int 	count = atoi(GET_VALUE(&acl->macCfgs, WIFI_KEYWORD_ACL_MAC_NUM) );
#endif

	value = GET_VALUE( &info->cgiVariables, WIFI_KEYWORD_ACL_MAC);

	for(i=0; i< count; i++)
	{
		sprintf(cmd, WIFI_KEYWORD_ACL_MAC"_%d", i);
		t = GET_VALUE(&acl->macCfgs, cmd);
//		fprintf(stderr, "raw = '%s', new is '%s'\n", t, value );
		if( !strncasecmp(t, value, 17) )
		{
			if(action== __ACL_ADD)
			{/* added : has exist */
				cgi_error_page(NULL, gettext("Has Exist when add"), gettext(CGI_STR_ERROR ) );
				return -1;
			}
			else
			{/* delete : found */
				break;
			}
		}
	}
	
	if(action==__ACL_DEL && i== count)
	{
		cgi_error_page(NULL, gettext("Not exist when delete"), gettext(CGI_STR_ERROR) );
		return -1;
	}

	if(action==__ACL_ADD)
	{/* added */
		sprintf(cmd, WIFI_KEYWORD_ACL_MAC"_%d", i);
		list_append_raw(&acl->macCfgs, cmd, value, NULL, NULL);
	}
	else
	{
		list_delete_by_name(&acl->macCfgs, cmd);
	}
	sprintf(cmd, "%d", (action==__ACL_ADD)? (count+1):(count-1) );
	list_replace_value_by_key(&acl->macCfgs, WIFI_KEYWORD_ACL_MAC_NUM, cmd);

	if(writeconfig(WIFI_CONFIG_ACL_FILE, ITEM_EQU, NULL, &acl->macCfgs) )
	{
		cgi_error_page(NULL, gettext("Wireless Operation Failed"), gettext( CGI_STR_SOME_ERROR ) );
		return -1;
	}

//	sprintf(cmd, __ACL_ADDRESS_COMMAND, info->wifiInfo.b.name, (action==__ACL_ADD)?"addmac":"delmac",  value);
	sprintf(cmd, __ACL_ADDRESS_COMMAND, GET_VALUE(&info->nicCfgs, NIC_KEYWORD_DEVICE), (action==__ACL_ADD)?"addmac":"delmac",  value);
	system(cmd);

	return 0;
}

int	__acl_init(WIFI_INFO *info)
{
#define	TMP_FILE	"/tmp/acl.txt"

	wifi_acl_info_t	 *acl = &info->aclInfo;
	char		line[PARSE_FILE_MAX_LENGTH];
	FILE  	*fp;
	char		t1[32];
	int		i = 0;
	char		filename[128];
	char		cmd[256];
	
	if (readconfig(WIFI_CONFIG_ACL_FILE, ITEM_EQU, NULL, &acl->macCfgs) )
	{
		cgi_error_page(NULL, gettext("Wireless Operation Failed"), gettext( CGI_STR_SOME_ERROR ) );
		return -1;
	}

//	sprintf(filename, WIFI_ACL_PROC"%s/ACL/policy", info->wifiInfo.b.name);
	sprintf(filename, WIFI_ACL_PROC"%s/ACL/policy", GET_VALUE(&info->nicCfgs, NIC_KEYWORD_DEVICE));
	sprintf(cmd, "cat %s > "TMP_FILE, filename );
	system(cmd);
//	if((fp=fopen(filename, "r"))==NULL)
	if((fp=fopen(TMP_FILE, "r"))==NULL)
	{
		perror( "Open WIFI ACL POLICY proc file failure!" );
		return -1;
	}

	fgets(line,PARSE_FILE_MAX_LENGTH,fp);
	int __count = sscanf(line, "%s", t1);

	if(__count == 1)
	{
		acl->policy = atoi(t1);
	}
	if(acl->policy< CGI_WIFI_ACL_OPEN || acl->policy > CGI_WIFI_ACL_OPEN)
		acl->policy = CGI_WIFI_ACL_OPEN;
	fclose(fp);


//	sprintf(filename, WIFI_ACL_PROC"%s/ACL/macs", info->wifiInfo.b.name);
	sprintf(filename, WIFI_ACL_PROC"%s/ACL/macs", GET_VALUE(&info->nicCfgs, NIC_KEYWORD_DEVICE));
	sprintf(cmd, "cat %s > "TMP_FILE, filename );
	system(cmd);
//	if((fp=fopen(filename, "r"))==NULL)
	if((fp=fopen(TMP_FILE, "r"))==NULL)
	{
		perror( "Open WIFI ACL MACs proc file failure!" );
		return -1;
	}

	i = 0;
	while( fgets(line,PARSE_FILE_MAX_LENGTH,fp) )
	{
		char		key[128];
		__count = sscanf(line, "%s", t1);
		
		sprintf(key, WIFI_KEYWORD_ACL_MAC"_%d", i );
		i++;
		
		list_append_raw(&acl->macRunning, key, t1, NULL, NULL);
	}
	fclose(fp);

//	acl->countMacs = i;
	return 0;
}

char *__wifi_acl_status(WIFI_INFO *info)
{
	wifi_acl_info_t		*acl = &info->aclInfo;
	char 	buf[8192*5];
	int 		length = 0;
	int 		count, i;

//	length += CGI_SPRINTF(buf,length, "<TR></TD><table width=\"65%%\" border=\"2\" align=\"center\" cellpadding=\"1\" cellspacing=\"1\" bgcolor=\"#FFFFFF\" >\n");
	length += CGI_SPRINTF(buf,length, "<TR><TD ><strong>%s</strong></TD><TD>%s</TD></TR></table>\n", 
		gettext("MAC Filter Policy"), gettext(GET_VALUE(&acl->macCfgs, WIFI_KEYWORD_ACL_POLICY) ));

	count = atoi(GET_VALUE(&acl->macCfgs, WIFI_KEYWORD_ACL_MAC_NUM ) );
	if(count<= 0 )
		return strdup(buf);
	
//	length += CGI_SPRINTF(buf,length, "<TR><TD colspan=2><strong>%s</strong></TD>\n", gettext("MAC Filter List") );
	length += CGI_SPRINTF(buf,length, "<table width=\"65%%\" border=\"2\" align=\"center\" cellpadding=\"1\" cellspacing=\"1\" bgcolor=\"#FFFFFF\" >\n");
	length += CGI_SPRINTF(buf, length, "\t<TR><TD align=\"center\"><strong>%s</strong></TD><TD align=\"center\"><strong>%s</strong></TD></TR>\n", 
		gettext(CGI_STR_MAC_ADDRESS), gettext(CGI_STR_MANAGEMENT) );


	for(i=0; i < count; i++)
	{
		char	key[128], target[256];
		char	*macAddress;
		
		sprintf(key, WIFI_KEYWORD_ACL_MAC"_%d", i);
		macAddress = GET_VALUE( &acl->macCfgs, key);
		if(macAddress && strlen(macAddress) )
		{
			sprintf(target, WEB_URL_WLAN"?"CGI_KEYWORD_OPERATOR"="CGI_WIFI_OP_ACL"&"CGI_ACTION"=%s&"WIFI_KEYWORD_ACL_MAC"=%s",
				CGI_ACTION_DEL, macAddress);

			length += CGI_SPRINTF(buf,length, "<TR><TD align=\"center\">%s</TD>\r\n", macAddress );
			length += CGI_SPRINTF(buf,length, "\t\t<TD align=\"center\">%s</TD></TR>\r\n" , cgi_button(CGI_STR_DELETE, target) );
		}
	}
//	length += CGI_SPRINTF(buf, length, "</table></TD></TR>\n" );

	return strdup(buf);
}

char *__wifi_acl_button(WIFI_INFO *info)
{
	char 	buf[8192*5];
	int 		length = 0;
	int 		j;
	char		*value;
	struct	acl_policy_cmd	*policy = _aclPolicies;
	wifi_acl_info_t				*acl = &info->aclInfo;

	/* policy configuration */
	length += CGI_SPRINTF(buf,length, "<TR><TD><table width=\"100%%\" border=\"0\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\" bgcolor=\"#FFFFFF\" >\n");

	length += CGI_SPRINTF(buf, length, "<form name=\"wifiForm\" method=\"post\" action=\"%s\">\n", WEB_URL_WLAN );
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_KEYWORD_OPERATOR, CGI_WIFI_OP_ACL);
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_ACTION, CGI_ACTION_DEFAULT);

//	length += CGI_SPRINTF(buf,length, "<TR><TD colspan=2><strong>%s</strong><p></TD></TR>\n", gettext("MAC Filter Policy") );
	value = GET_VALUE( &acl->macCfgs, WIFI_KEYWORD_ACL_POLICY);
	length += CGI_SPRINTF(buf, length, "\t<TR><TD align=\"right\"><strong>%s:</strong></TD><TD>\n\t<select name=\"%s\" style='width:120px;'>\n", 
		gettext("MAC Filter Policy"), WIFI_KEYWORD_ACL_POLICY );
	for(j=0; j<CGI_WIFI_ACL_FLUSH; j++)
	{
		length += CGI_SPRINTF(buf,length, "\t\t<option value=\"%s\" %s>%s</option>\n", 
			policy->description, (!strcasecmp(policy->description, value) ) ?"selected":"",  gettext(policy->description));
		policy++;
	}
	length += CGI_SPRINTF(buf,length, "</TD><TD colspan=2 align=\"center\">%s</TD></TR>\r\n",cgi_submit_button( gettext(CGI_STR_SAVE) ) );

	length += CGI_SPRINTF(buf, length, "</FORM>\n" );
	

	/* MAC address add form */
//	length += CGI_SPRINTF(buf,length, "<table width=\"100%%\" border=\"2\" align=\"center\" cellpadding=\"1\" cellspacing=\"1\" bgcolor=\"#FFFFFF\" >\n");

	length += CGI_SPRINTF(buf, length, "<form name=\"wifiForm\" method=\"post\" action=\"%s\">\n", WEB_URL_WLAN );
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_KEYWORD_OPERATOR, CGI_WIFI_OP_ACL);
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_ACTION, CGI_ACTION_ADD);

//	length += CGI_SPRINTF(buf,length, "<TR><TD colspan=2><strong>%s</strong><p></TD></TR>\n", gettext("MAC Address Add") );
	length += sprintf(buf+length, "<TR><TD align=\"right\"><strong>%s:</strong></TD>", 
		gettext(CGI_STR_MAC_ADDRESS) );
	length += sprintf(buf+length, "<TD><input name=\"%s\" type=\"text\" size=\"15\" value=\"\" maxlength=\"32\"></TD>\r\n",
		WIFI_KEYWORD_ACL_MAC );
	length += CGI_SPRINTF(buf,length, "<TD align=\"center\">%s</TD></TR>\r\n",cgi_submit_button( gettext(CGI_STR_ADD) ) );

//	length += CGI_SPRINTF(buf, length, "</FORM></table></TD></TR>\n" );
	length += CGI_SPRINTF(buf, length, "</FORM></table></TD></TR>\n" );

	return strdup(buf);
}


int	wifi_Acl(WIFI_INFO *info)
{
	char *action;
	char	target[256];
	sprintf(target, WEB_URL_WLAN"?"CGI_KEYWORD_OPERATOR"="CGI_WIFI_OP_ACL);
	
	__acl_init(info);

	action = GET_VALUE( &info->cgiVariables, CGI_ACTION);
	if(!action || !strlen(action) )
	{
		cgi_info_page(gettext("MAC Filter Configuration of WiFi"), __wifi_acl_status(info), __wifi_acl_button(info));
		return 0;
	}

	if(!strcasecmp(action, CGI_ACTION_DEFAULT) )
	{/* save policy */
		if(__acl_command_policy( info) )
			return 0;
	}

	if(!strcasecmp(action, CGI_ACTION_ADD) )
	{/* add new MAC address */
		if(__acl_command_mac_address(__ACL_ADD, info) )
			return 0;
	}

	if(!strcasecmp(action, CGI_ACTION_DEL) )
	{/* delete MAC address */
		if( __acl_command_mac_address(__ACL_DEL, info) )
			return 0;
	}

	return cgi_refresh_page(3, target,gettext(CGI_STR_WAIT), gettext("ACL Filter is saving") );
}

