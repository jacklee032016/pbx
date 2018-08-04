/*
* $Id: netQos.c,v 1.2 2007/09/06 09:19:59 lizhijie Exp $
*/

#include "netQos.h"

static qos_bandwidth_t	_bandwidths[] = 
{
		{
			QOS_BW_128K,
			QOS_BW_NAME_128K
		},
		{
			QOS_BW_256K,
			QOS_BW_NAME_256K
		},
		{
			QOS_BW_512K,
			QOS_BW_NAME_512K
		},
		{
			QOS_BW_1M,
			QOS_BW_NAME_1M
		},
		{
			QOS_BW_2M,
			QOS_BW_NAME_2M
		},
		{
			QOS_BW_5M,
			QOS_BW_NAME_5M
		},
		{
			QOS_BW_10M,
			QOS_BW_NAME_10M
		},
};

#if 0
static llist 	qosCfg;
#endif

static char *__QoS_buttons(FW_INFO *info )
{
	char 		buf[8192*2];
	int 			length = 0;
	int			i;
	
	length += CGI_SPRINTF(buf,length, "\n<TR><TD  colspan=2><strong>%s:<p></strong></TD></TR>\n", 
		gettext("Please Select the bandwidth allocated to voice traffic") );


	length += CGI_SPRINTF(buf, length, "<form name=\"qosForm\" method=\"post\" action=\"%s\">\n", WEB_URL_FIREWALL);
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_KEYWORD_OPERATOR, FIREWALL_OP_QOS);
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_ACTION, CGI_ACTION_ADD );
	
	length += CGI_SPRINTF(buf,length, "\n<TR><TD align=\"right\"><strong>%s:</strong></TD>\n\t<TD><select name=\"%s\" style='width:90px;'>\n", 
		gettext("Bandwidth"), QOS_CFG_BANDWIDTH );
	for(i=0; i<7; i++)
	{
		length += CGI_SPRINTF(buf,length, "\t\t<option value=\"%s\">%s</option>\n",_bandwidths[i].bandwidth, _bandwidths[i].description);
	}
	
	length += CGI_SPRINTF(buf,length, "</SELECT><p></TD></TR>\n<TR><TD align=\"center\" colspan=2>%s  \n", cgi_submit_button( gettext(CGI_STR_SAVE) ) );

	length += CGI_SPRINTF(buf,length, "%s</TD></TR>\r\n</FORM>\r\n", cgi_help_button(CGI_HELP_MSG_QOS) );

	return strdup(buf);
}


static char *__QoS_select_buttons(FW_INFO *info )
{
	char 		buf[8192*2];
	char			targetActivate[256], targetDeactivate[256];
	int 			length = 0;

	length += CGI_SPRINTF(buf,length, "\n<TR><TD><strong>%s:</strong><p></TD></TR>\n", 
		gettext("Please Select the QoS Mode") );

	sprintf(targetActivate, "%s?%s=%s&%s=%s", WEB_URL_FIREWALL,CGI_KEYWORD_OPERATOR, FIREWALL_OP_QOS, CGI_ACTION, CGI_ACTION_ACTIVATE );
	sprintf(targetDeactivate,"%s?%s=%s&%s=%s",  WEB_URL_FIREWALL, CGI_KEYWORD_OPERATOR, FIREWALL_OP_QOS, CGI_ACTION, CGI_ACTION_DEL );
	
	length += CGI_SPRINTF(buf,length, "</TD></TR>\n<TR><TD align=\"center\" colspan=4>%s %s %s  <p></TD></TR>\r\n</FORM>\r\n\n", 
		cgi_button( gettext(CGI_ACTION_ACTIVATE), targetActivate),cgi_button( gettext(CGI_STR_DEACTIVATE), targetDeactivate), cgi_help_button(CGI_HELP_MSG_QOS )   );

	return strdup(buf);
}


static int __QoS_init()
{
#if 0
	list_create(&qosCfg);
	if (readconfig(IPTABLES_VPN_CONFIG, ITEM_TRI, NULL, &qosCfg) != 0)
	{
		cgi_error_page(NULL, gettext(CGI_STR_ERROR), gettext("No Config of VPN is found") );
		return -1;
	}
#endif	
	return 0;
}

static int __QoS_activate(FW_INFO *info)
{
#if 0
	char		msg[256];
	char		*gw, *type, *ip;
	node	*current;
	char 	*action;

	action = GET_VALUE(&info->cgiVariables, CGI_ACTION );
	
	gw = GET_VALUE(&info->cgiVariables, VPN_CFG_GATEWAY);
	type = GET_VALUE(&info->cgiVariables, VPN_CFG_TYPE );
	ip = GET_VALUE(&info->cgiVariables, VPN_CFG_LOCAL_IP );
	
	if(!gw ||!strlen(gw)||!type||!strlen(type) || !ip ||!strlen(ip)  )
	{
		sprintf(msg, "%s", gettext("Error Fields exist when update VPN configuration") );
		cgi_error_page(NULL, gettext(CGI_STR_ERROR), msg);
		return -1;	
	}
	
	current = qosCfg.head;

	while(current)
	{
		if(!strcasecmp(current->entry.name, gw) &&
			!strcasecmp(current->entry.value, ip) &&
			!strcasecmp( current->entry.type, type) )
		{
			break;
		}	
		current = current->next;
	}

	if((!current && !strcasecmp(action, CGI_ACTION_DEL ) ) ||
		(current && !strcasecmp(action, CGI_ACTION_ADD) )   )
	{
		sprintf(msg, "\"%s('%s', '%s')\" %s", gw?gw:"NULL", ip?ip:"NULL", 
			type?type:"NULL", (current)?gettext(CGI_STR_HAS_EXIST):gettext(CGI_STR_IS_NOT_EXIST) );
		cgi_error_page(NULL, gettext(CGI_STR_ERROR), msg );
		return -1;
	}

	if(current)
	{/* delete */
		list_delete_node(&qosCfg, current);
	}
	else
	{/* add */
		list_append_raw(&qosCfg, gw, ip, type, NULL );
	}
	
	writeconfig(IPTABLES_VPN_CONFIG, ITEM_TRI, NULL, &qosCfg );
#endif

	return 0;
}

int cgiNet_QoS(FW_INFO *info)
{
	char *action;

	if (__QoS_init() )
	{
		return -1;
	}

	action = GET_VALUE(&info->cgiVariables, CGI_ACTION );
	if(!action || !strlen(action) )
	{
		cgi_info_page(gettext("QoS Configuration"), "", __QoS_select_buttons( info) );
		return 0;
	}
	
	if(!strcasecmp(action, CGI_ACTION_ACTIVATE)  )
	{
		cgi_info_page(gettext("QoS Configuration"), "", __QoS_buttons(info) );
		return 0;
	}
	
	if(!strcasecmp(action, CGI_ACTION_ADD) ||
		!strcasecmp(action, CGI_ACTION_DEL)  )
	{
		if(__QoS_activate(info)< 0)
			return 1;

		cgi_refresh_page(1, WEB_URL_FIREWALL"?"CGI_KEYWORD_OPERATOR"="FIREWALL_OP_QOS , 
			gettext("QoS Configuration"), gettext("Writing now, please waiting.....") );
		return 0;		
	}
	return 0;	
}

