/*
* $Id: cgi_wireless_mode.c,v 1.4 2007/09/06 09:19:58 lizhijie Exp $
*/

#include "cgi_lib.h"
#include "cgi_wireless.h"

struct mac_t _wifimacs[] =
{
	{
		"Master",
		_("AP(Access Point)")
	},
	{
		"Ad-Hoc",
		CGI_STR_AD_HOC
	},
	{
		"Managed",
		_("Mobile Station")
	},
	{
//		-1,
		NULL,
		NULL	
	}
};

struct phy_mode _wifiphys[] =
{
	{
		WIFI_MODE_AUTO, 
		CGI_STR_AUTO
	},
	{
		WIFI_MODE_11B, 
		_("802.11B")
	},
	{
		WIFI_MODE_11G, 
		_("802.11G")
	},
	{
		WIFI_MODE_TURBO, 
		_("Turbo")
	},
	{
		-1,
		NULL	
	}
};


rate_type_t	rates_11b[] =
{
	{
		"0",
		CGI_STR_AUTO	
	},
	{
		"1M",
		_("1M")	
	},
	{
//		WIFI_RATE_2M,
		"2M",	
		_("2M")	
	},
	{
//		WIFI_RATE_5_5M,
		"5.5M",
		_("5.5M")	
	},
	{
//		WIFI_RATE_11M,
		"11M",
		_("11M")	
	},
	{
//		-1,
		NULL,
		NULL	
	}
};

rate_type_t	rates_11g[] =
{
	{
//		WIFI_RATE_AUTO,
		"auto",
		CGI_STR_AUTO	
	},
	{
//		WIFI_RATE_1M,
		"1M",
		_("1M")	
	},
	{
//		WIFI_RATE_2M,
		"2M",
		_("2M")	
	},
	{
//		WIFI_RATE_5_5M,
		"5.5M",
		_("5.5M")	
	},
	{
//		WIFI_RATE_11M,
		"11M",
		_("11M")	
	},
	{
//		WIFI_RATE_6M,
		"6M",
		_("6M")	
	},
	{
//		WIFI_RATE_9M,
		"9M",
		_("9M")	
	},
	{
//		WIFI_RATE_12M,
		"12M",
		_("12M")	
	},
	{
//		WIFI_RATE_18M,
		"18M",
		_("18M")	
	},
	{
//		WIFI_RATE_24M,
		"24M",
		_("24M")	
	},
	{
//		WIFI_RATE_36M,
		"36M",
		_("36M")	
	},
	{
//		WIFI_RATE_48M,
		"48M",
		_("48M")	
	},
	{
//		WIFI_RATE_54M,
		"54M",
		_("54M")	
	},
	{
//		-1,
		NULL,
		NULL	
	}
};

char *__wireless_mac_mode_status(WIFI_INFO *info)
{
	char 	buf[8192*5];
	int 		length = 0;
	int		isOn = 0;
	struct phy_mode *phyMode = _wifiphys;

	length += CGI_SPRINTF(buf,length, "<TR><TD colspan=2><strong>%s</strong><p></TD>\n", gettext("Select Wireless Mode") );
	length += CGI_SPRINTF(buf, length, "<form name=\"wifiForm\" method=\"post\" action=\"%s\">\n",(info->type== CGI_WIRELESS_WIFI)?WEB_URL_WLAN:WEB_URL_MESH );
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_KEYWORD_OPERATOR, CGI_WIRELESS_OP_MODE);
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_ACTION, CGI_ACTION_DEFAULT);

	length += CGI_SPRINTF(buf, length, "\t<TR><TD align=\"right\"><strong>%s:</strong></TD><TD>", 
		gettext(CGI_STR_WORK_MODE)  );
	if(info->type== CGI_WIRELESS_WIFI)
	{
		struct mac_t *mac = _wifimacs;
		length += CGI_SPRINTF(buf,length, "\n\t<select name=\"%s\" style='width:120px;'>\n", WIFI_KEYWORD_MODE);
		while(mac->name)
		{
			length += CGI_SPRINTF(buf,length, "\t\t<option value=\"%s\" %s>%s</option>\n", 
				mac->name, (strcasecmp(GET_VALUE(&info->globalCfgs, WIFI_KEYWORD_MODE), mac->name) )?"":"selected",  gettext(mac->description) );

			mac++;
		}
		length += CGI_SPRINTF(buf,length, "\t</select>\n</TD></TR>\r\n");
	}
	else
	{
		length += CGI_SPRINTF(buf,length, "%s\n</TD></TR>\r\n", gettext(CGI_STR_AD_HOC) );
	}
	
	length += CGI_SPRINTF(buf, length, "\t<TR><TD align=\"right\"><strong>%s:</strong></TD><TD>\n\t<select name=\"%s\" style='width:120px;'>\n", 
		gettext(CGI_STR_PHY_MODE), WIFI_KEYWORD_PHY_MODE );
	while(phyMode->type != -1 )
	{
		int phy = atoi(GET_VALUE(&info->globalCfgs, WIFI_KEYWORD_PHY_MODE) );
		length += CGI_SPRINTF(buf,length, "\t\t<option value=\"%d\" %s>%s</option>\n", 
			phyMode->type, (phy==phyMode->type) ?"selected":"", phyMode->description);

		phyMode++;
	}
	length += CGI_SPRINTF(buf,length, "\t</select>\n</TD></TR>\r\n");

	length += CGI_SPRINTF(buf, length, "\t<TR><TD  align=\"right\"><strong>%s:</strong></TD><TD>\n\t<select name=\"%s\" style='width:120px;'>\n", 
		gettext(CGI_STR_START_WHEN_BOOT), WIFI_KEYWORD_ONBOOT );
	length += CGI_SPRINTF(buf,length, "\t\t<option value=\"%s\" %s>%s</option>\n", 
		WIFI_VALUE_ON, (!strcasecmp(GET_VALUE(&info->globalCfgs, WIFI_KEYWORD_ONBOOT),WIFI_VALUE_ON) )?"selected":"",gettext(WIFI_VALUE_ON));
	length += CGI_SPRINTF(buf,length, "\t\t<option value=\"%s\" %s>%s</option>\n", 
		WIFI_VALUE_OFF, (!strcasecmp(GET_VALUE(&info->globalCfgs, WIFI_KEYWORD_ONBOOT),WIFI_VALUE_OFF) )?"selected":"",gettext(WIFI_VALUE_OFF));
	length += CGI_SPRINTF(buf,length, "\t</select>\n</TD></TR>\r\n");

	if(info->type == CGI_WIRELESS_WIFI)
	{
		length += sprintf(buf+length, "<TR><TD align=\"right\"><strong>%s:</strong></TD>", 
			gettext(CGI_STR_START_AODV) );
		
		if(!strcasecmp(GET_VALUE(&info->globalCfgs, WIFI_KEYWORD_AODV), WIFI_VALUE_ON) )
			isOn = 1;
		length += CGI_SPRINTF(buf,length, "<TD align=\"left\">	<input name=\"%s\" type=\"Radio\" value=\"%s\" %s>%s<input name=\"%s\" type=\"Radio\" value=\"%s\" %s>%s</TD></TR>\r\n", 
			WIFI_KEYWORD_AODV, WIFI_VALUE_ON, (isOn==1)?"checked":"", gettext(CGI_STR_ON), WIFI_KEYWORD_AODV, WIFI_VALUE_OFF, (isOn==1)?"":"checked", gettext(CGI_STR_OFF) );
	}

	length += CGI_SPRINTF(buf,length, "<TR><TD colspan=2 align=\"center\">%s\r\n",cgi_submit_button( gettext(CGI_STR_NEXT) ) );

	length += CGI_SPRINTF(buf,length, "\t\t%s</TD></TR></FORM>\r\n" , cgi_help_button(CGI_HELP_MSG_PBX_ACCOUNT) );

	if(info->type == CGI_WIRELESS_WIFI)
	{
		length += CGI_SPRINTF(buf,length, "<TR><TD colspan=2><strong>%s:</strong><p>%s<br>%s</TD></TR>\r\n",
			gettext(CGI_STR_NOTE),gettext("AODV can only be used when Ad-Hoc Mode"),
			gettext("When AODV is enabled, WIFI is in Ad-hoc no matter when you set"));
	}
	return strdup(buf);
}


char *__wireless_params_status(WIFI_INFO *info)
{
	char 	buf[8192*5];
	int 		length = 0;
	int 		j;
	rate_type_t	*rateType;
	char			*rate;
	int			phy;

	length += CGI_SPRINTF(buf,length, "<TR><TD colspan=2><strong>%s</strong><p></TD>\n", gettext("Set Wireless Basic Parameters") );
	length += CGI_SPRINTF(buf, length, "<form name=\"wifiForm\" method=\"post\" action=\"%s\">\n", (info->type==CGI_WIRELESS_WIFI)?WEB_URL_WLAN:WEB_URL_MESH );
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_KEYWORD_OPERATOR, CGI_WIRELESS_OP_MODE);
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_ACTION, CGI_ACTION_ADD);

	length += sprintf(buf+length, "<TR><TD  align=\"right\"><strong>%s:</strong></TD>", 
		gettext(CGI_STR_ESSID) );
	length += sprintf(buf+length, "<TD><input name=\"%s\" type=\"text\" size=\"15\" value=\"%s\" maxlength=\"32\"></TD></TR>\r\n",
		WIFI_KEYWORD_ESSID, GET_VALUE(&info->globalCfgs, WIFI_KEYWORD_ESSID));

	length += sprintf(buf+length, "<TR><TD align=\"right\"><strong>%s:</strong></TD>", 
		gettext(CGI_STR_NICKNAME) );
	length += sprintf(buf+length, "<TD><input name=\"%s\" type=\"text\" size=\"15\" value=\"%s\" maxlength=\"32\"></TD></TR>\r\n",
		WIFI_KEYWORD_NICKNAME, GET_VALUE(&info->globalCfgs, WIFI_KEYWORD_NICKNAME));

	phy = atoi(GET_VALUE(&info->globalCfgs, WIFI_KEYWORD_PHY_MODE) );
	if( phy == WIFI_MODE_11B || phy == WIFI_MODE_11G )
	{
		if(phy==WIFI_MODE_11B)
			rateType = rates_11b;
		else
			rateType = rates_11g;
			
		rate = GET_VALUE(&info->globalCfgs, WIFI_KEYWORD_RATE) ;

		length += CGI_SPRINTF(buf, length, "\t<TR><TD align=\"right\"><strong>%s:</strong></TD><TD>\n\t<select name=\"%s\" style='width:140px;'>\n", 
			gettext(CGI_STR_RATE), WIFI_KEYWORD_RATE );
//		while(rateType->type != -1 )
		while(rateType->type != NULL )
		{
			length += CGI_SPRINTF(buf,length, "\t\t<option value=\"%s\" %s>%s</option>\n", 
				rateType->type, (!strcasecmp(rate, rateType->type) )?"selected":"", rateType->description);
			rateType++;
		}
		length += CGI_SPRINTF(buf,length, "\t</select>\n</TD></TR>\r\n");
	}
	else
	{
		length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%d\">\n", WIFI_KEYWORD_RATE, WIFI_RATE_AUTO);
	}

	if(info->wifiInfo.has_range )
	{
		struct iw_range *range = &info->wifiInfo.range;
		int	freq;
		int 	channel;
		length += CGI_SPRINTF(buf, length, "\t<TR><TD align=\"right\"><strong>%s:</strong></TD><TD>\n\t<select name=\"%s\" style='width:140px;'>\n", 
			gettext(CGI_STR_CHANNEL_FREQ), WIFI_KEYWORD_CHANNEL );
		for(j=0; j< range->num_frequency; j++ )
		{
			GET_KHZ(freq, range->freq[j]);
			channel =atoi( GET_VALUE(&info->globalCfgs, WIFI_KEYWORD_CHANNEL) );
			length += CGI_SPRINTF(buf,length, "\t\t<option value=\"%d\" %s>%s %d(%d MHz)</option>\n", 
				range->freq[j].i,(range->freq[j].i==channel)?"selected":"", gettext(CGI_STR_CHANNEL), iw_freq_to_channel(freq, range), freq/KILO_INT );
		}
		length += CGI_SPRINTF(buf,length, "\t</select>\n</TD></TR>\r\n");
	}
	else
	{
		length += CGI_SPRINTF(buf,length, "<TR><TD colspan=2 ><strong>%s</strong>:%s</TD></TR>\r\n",gettext(CGI_STR_NOTE), gettext("Channel Parameter can be set only when drivers have been loaded") );
	}
	
	length += CGI_SPRINTF(buf,length, "<TR><TD colspan=2 align=\"center\">%s\r\n",cgi_submit_button( gettext(CGI_STR_NEXT) ) );

	length += CGI_SPRINTF(buf,length, "\t\t%s</TD></TR></FORM>\r\n" , cgi_help_button(CGI_HELP_MSG_PBX_ACCOUNT) );

	return strdup(buf);
}


char *__wireless_result_info(WIFI_INFO *info)
{
	char 	buf[8192*5];
	int 		length = 0;
	int 		i;
	int		isValidate = 0;
	char		buffer[128];
	rate_type_t	*rate = rates_11g;
	char			*currentRate;
	struct phy_mode *phyMode = _wifiphys;

	length += CGI_SPRINTF(buf,length, "<TR><TD colspan=2><strong>%s</strong><p></TD></TR>\n", gettext("Wireless Basic Parameters") );

	length += sprintf(buf+length, "<TR><TD  align=\"right\"><strong>%s:</strong></TD><TD>%s</TD></TR>\r\n", 
		gettext(CGI_STR_MODE), GET_VALUE(&info->globalCfgs, WIFI_KEYWORD_MODE) );

	length += sprintf(buf+length, "<TR><TD  align=\"right\"><strong>%s:</strong></TD><TD>%s</TD></TR>\r\n", 
		gettext(CGI_STR_START_WHEN_BOOT), GET_VALUE(&info->globalCfgs, WIFI_KEYWORD_ONBOOT) );

	length += sprintf(buf+length, "<TR><TD  align=\"right\"><strong>%s:</strong></TD><TD>%s</TD></TR>\r\n", 
		gettext(CGI_STR_ESSID), GET_VALUE(&info->globalCfgs, WIFI_KEYWORD_ESSID) );
#if 0
	length += sprintf(buf+length, "<TR><TD  align=\"right\"><strong>%s:</strong></TD><TD>%s</TD></TR>\r\n", 
		gettext(CGI_STR_ESSID), GET_VALUE(&info->globalCfgs, WIFI_KEYWORD_ESSID) );
#endif

	length += sprintf(buf+length, "<TR><TD align=\"right\"><strong>%s:</strong></TD><TD>%s</TD></TR>\r\n", 
		gettext(CGI_STR_NICKNAME), GET_VALUE(&info->globalCfgs, WIFI_KEYWORD_NICKNAME) );

	i = atoi(GET_VALUE(&info->globalCfgs, WIFI_KEYWORD_PHY_MODE) );

	while(phyMode->type != -1)
	{
		if( i== phyMode->type )
		{
			sprintf(buffer, "%s", phyMode->description );
			isValidate = 1;
		}	

		phyMode++;
	}
	if(! isValidate)
		sprintf(buffer, "%s", "unknown" );
	
	length += CGI_SPRINTF(buf, length, "\t<TR><TD align=\"right\"><strong>%s:</strong></TD><TD>%s</TD></TR>\n", 
		gettext(CGI_STR_PHY_MODE), buffer );

	i = atoi(GET_VALUE(&info->globalCfgs, WIFI_KEYWORD_CHANNEL) );
	sprintf(buffer, "%d(%d MHz)", i, iw_channel_to_freq(i, &info->wifiInfo.range)/KILO_INT );
	length += CGI_SPRINTF(buf, length, "\t<TR><TD align=\"right\"><strong>%s:</strong></TD><TD>%s</TD></TR>\r\n", 
		gettext(CGI_STR_CHANNEL_FREQ), buffer );
	
	currentRate = GET_VALUE(&info->globalCfgs, WIFI_KEYWORD_RATE) ;
	while(rate->type != NULL)
	{
//		if(rate->type == i )
		if(!strcasecmp(rate->type , currentRate) )
		{
			length += CGI_SPRINTF(buf, length, "\t<TR><TD align=\"right\"><strong>%s:</strong></TD><TD>%s</TD></TR>\r\n", 
				gettext(CGI_STR_RATE), rate->description );
			break;
		}
		
		rate++;
	}
	return strdup(buf);
}

int	cgi_wireless_mode(WIFI_INFO *info)
{
	char *action;

	action = GET_VALUE( &info->cgiVariables, CGI_ACTION);
	if(!action || !strlen(action) )
	{
		cgi_info_page(gettext("Mode Configuration of Wireless"), gettext("Basic parameters of Wireless are set here"), __wireless_mac_mode_status(info));
		return 0;
	}	

	cgi_wireless_save_config(info);

	if(!strcasecmp(action, CGI_ACTION_DEFAULT) )
	{/* mode save and display basic parameters */
		cgi_info_page(gettext("Basic Parameters of Wireless"), gettext("Basic parameters of Wireless are set here"), __wireless_params_status(info));
		return 0;
	}

	if(!strcasecmp(action, CGI_ACTION_ADD) )
	{/* mode save */
		cgi_info_page(gettext("Basic Parameters of Wireless"),  __wireless_result_info(info), "");
		return 0;
	}
	
	return 0;
}

