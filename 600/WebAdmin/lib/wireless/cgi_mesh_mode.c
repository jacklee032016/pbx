/*
* $Id: cgi_mesh_mode.c,v 1.4 2007/09/06 09:19:58 lizhijie Exp $
*/

#include "cgi_lib.h"
#include "cgi_wireless.h"

#define	MESH_GET_VALUE( list, key, index, value) \
	do{char keyword[256];	\
		sprintf(keyword, "%s_%d", key, index); \
		value = GET_VALUE((list), keyword);\
		}while(0)
		

char *__mesh_mac_nic_status(WIFI_INFO *info, int index)
{
	char 	buf[8192*5];
	int 		length = 0;
	int		isOn = 0;
	char		*value;
	struct mac_t *mac = _wifimacs;
	struct phy_mode *phyMode = _wifiphys;

	length += CGI_SPRINTF(buf,length, "<TR><TD colspan=2 align=\"center\" bgcolor=\"#cccccc\"><strong>%s %d</strong></TD>\n", gettext(CGI_STR_MESH_DEVICE), index+1 );

	length += CGI_SPRINTF(buf, length, "\t<TR><TD align=\"right\"><strong>%s:</strong></TD><TD>", 
		gettext(CGI_STR_WORK_MODE)  );
	
	MESH_GET_VALUE(&info->globalCfgs, WIFI_KEYWORD_MODE, index, value );
//	if(info->type== CGI_WIRELESS_WIFI)
	{
		length += CGI_SPRINTF(buf,length, "\n\t<select name=\"%s_%d\" style='width:120px;'>\n", WIFI_KEYWORD_MODE, index);
		while(mac->name)
		{
			length += CGI_SPRINTF(buf,length, "\t\t<option value=\"%s\" %s>%s</option>\n", 
				mac->name, (strcasecmp(value, mac->name) )?"":"selected",  gettext( mac->description) );

			mac++;
		}
		length += CGI_SPRINTF(buf,length, "\t</select>\n</TD></TR>\r\n");
	}
#if 0
	else
	{
		length += CGI_SPRINTF(buf,length, "%s\n</TD></TR>\r\n", gettext(CGI_STR_AD_HOC) );
	}
#endif

	length += CGI_SPRINTF(buf, length, "\t<TR><TD align=\"right\"><strong>%s:</strong></TD><TD>\n\t<select name=\"%s_%d\" style='width:120px;'>\n", 
		gettext(CGI_STR_PHY_MODE), WIFI_KEYWORD_PHY_MODE, index );

	MESH_GET_VALUE(&info->globalCfgs, WIFI_KEYWORD_PHY_MODE, index, value );
	while(phyMode->type != -1 )
	{
		int phy = atoi(value);
		length += CGI_SPRINTF(buf,length, "\t\t<option value=\"%d\" %s>%s</option>\n", 
			phyMode->type, (phy==phyMode->type) ?"selected":"",  phyMode->description);

		phyMode ++;
	}
	length += CGI_SPRINTF(buf,length, "\t</select>\n</TD></TR>\r\n");

	length += CGI_SPRINTF(buf, length, "\t<TR><TD  align=\"right\"><strong>%s:</strong></TD><TD>\n\t<select name=\"%s_%d\" style='width:120px;'>\n", 
		gettext(CGI_STR_START_WHEN_BOOT), WIFI_KEYWORD_ONBOOT, index );
	
	MESH_GET_VALUE(&info->globalCfgs, WIFI_KEYWORD_ONBOOT, index, value );
	length += CGI_SPRINTF(buf,length, "\t\t<option value=\"%s\" %s>%s</option>\n", 
		WIFI_VALUE_ON, (!strcasecmp(value,WIFI_VALUE_ON) )?"selected":"",gettext(WIFI_VALUE_ON));
	length += CGI_SPRINTF(buf,length, "\t\t<option value=\"%s\" %s>%s</option>\n", 
		WIFI_VALUE_OFF, (!strcasecmp(value,WIFI_VALUE_OFF) )?"selected":"",gettext(WIFI_VALUE_OFF));
	length += CGI_SPRINTF(buf,length, "\t</select>\n</TD></TR>\r\n");

	if(info->type == CGI_WIRELESS_WIFI)
	{
		length += sprintf(buf+length, "<TR><TD align=\"right\"><strong>%s:</strong></TD>", 
			gettext(CGI_STR_START_AODV) );
		
		MESH_GET_VALUE(&info->globalCfgs, WIFI_KEYWORD_AODV, index, value );
		if(!strcasecmp(value, WIFI_VALUE_ON) )
			isOn = 1;
		length += CGI_SPRINTF(buf,length, "<TD align=\"left\">	<input name=\"%s_%d\" type=\"Radio\" value=\"%s\" %s>%s<input name=\"%s_%d\" type=\"Radio\" value=\"%s\" %s>%s</TD></TR>\r\n", 
			WIFI_KEYWORD_AODV, index, WIFI_VALUE_ON, (isOn==1)?"checked":"", gettext(CGI_STR_ON), WIFI_KEYWORD_AODV, index, WIFI_VALUE_OFF, (isOn==1)?"":"checked", gettext(CGI_STR_OFF) );
	}

	return strdup(buf);
}


char *__mesh_mac_mode_status(WIFI_INFO *info)
{
	char 	buf[8192*5];
	int 		length = 0;
	int 		i;
	int		nicCount = 0;
	int		nicConfig = 0;
	char		*value;

	length += CGI_SPRINTF(buf,length, "<TR><TD colspan=2><strong>%s</strong><p></TD>\n", gettext("Select Wireless Mode") );
	nicCount = atoi( GET_VALUE(&info->meshDevs, WIFI_KEYWORD_MESH_NUM) );
	nicConfig = atoi( GET_VALUE(&info->globalCfgs, WIFI_KEYWORD_MESH_NUM) );
	if(nicConfig!=nicCount)
	{
		length += CGI_SPRINTF(buf,length, "<TR><TD colspan=2><strong>%s : %d</strong><p></TD>\n", gettext("MESH Device configed is") , nicConfig );
	}
	
	length += CGI_SPRINTF(buf, length, "<form name=\"wifiForm\" method=\"post\" action=\"%s\">\n",(info->type== CGI_WIRELESS_WIFI)?WEB_URL_WLAN:WEB_URL_MESH );
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_KEYWORD_OPERATOR, CGI_WIRELESS_OP_MODE);
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_ACTION, CGI_ACTION_DEFAULT);
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%d\">\n", WIFI_KEYWORD_MESH_NUM, nicCount );

	length += CGI_SPRINTF(buf, length, "\t<TR><TD  align=\"right\"><strong>%s:</strong></TD><TD>\n\t<select name=\"%s\" style='width:120px;'>\n", 
		gettext(CGI_STR_START_WHEN_BOOT), WIFI_KEYWORD_ONBOOT );
	value = GET_VALUE(&info->globalCfgs, WIFI_KEYWORD_ONBOOT);
	length += CGI_SPRINTF(buf,length, "\t\t<option value=\"%s\" %s>%s</option>\n", 
		WIFI_VALUE_ON, (!strcasecmp(value,WIFI_VALUE_ON) )?"selected":"",gettext(WIFI_VALUE_ON));
	length += CGI_SPRINTF(buf,length, "\t\t<option value=\"%s\" %s>%s</option>\n", 
		WIFI_VALUE_OFF, (!strcasecmp(value,WIFI_VALUE_OFF) )?"selected":"",gettext(WIFI_VALUE_OFF));
	length += CGI_SPRINTF(buf,length, "\t</select>\n</TD></TR>\r\n");

	length += CGI_SPRINTF(buf, length, "\t<TR><TD  align=\"right\"><strong>%s:</strong></TD>\n\t<TD>%d</TD></TR>\r\n\n", 
		gettext("MESH Device Number"), nicCount );

	for(i=0; i< nicCount; i++)
	{
		length += CGI_SPRINTF(buf,length, "%s",__mesh_mac_nic_status( info, i) );
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


char *__mesh_params_nic_status(WIFI_INFO *info, int index)
{
	char 	buf[8192*5];
	int 		length = 0;
	int 		j;
	rate_type_t	*rateType;
	char			*rate;
	int			phy;
	char			*value;

	length += CGI_SPRINTF(buf,length, "<TR><TD colspan=2 align=\"center\" bgcolor=\"#cccccc\"><strong>%s %d</strong></TD>\n", gettext(CGI_STR_MESH_DEVICE), index+1 );

	length += sprintf(buf+length, "<TR><TD  align=\"right\"><strong>%s:</strong></TD>", 
		gettext(CGI_STR_ESSID) );
	MESH_GET_VALUE(&info->globalCfgs, WIFI_KEYWORD_ESSID, index, value );
	length += sprintf(buf+length, "<TD><input name=\"%s_%d\" type=\"text\" size=\"15\" value=\"%s\" maxlength=\"32\"></TD></TR>\r\n",
		WIFI_KEYWORD_ESSID, index, value );

	length += sprintf(buf+length, "<TR><TD align=\"right\"><strong>%s:</strong></TD>", 
		gettext(CGI_STR_NICKNAME) );
	MESH_GET_VALUE(&info->globalCfgs, WIFI_KEYWORD_NICKNAME, index, value );
	length += sprintf(buf+length, "<TD><input name=\"%s_%d\" type=\"text\" size=\"15\" value=\"%s\" maxlength=\"32\"></TD></TR>\r\n",
		WIFI_KEYWORD_NICKNAME, index, value);

	MESH_GET_VALUE(&info->globalCfgs, WIFI_KEYWORD_PHY_MODE, index, value );
	phy = atoi(value);
	
	if( phy == WIFI_MODE_11B || phy == WIFI_MODE_11G )
	{
		if(phy==WIFI_MODE_11B)
			rateType = rates_11b;
		else
			rateType = rates_11g;
			
		MESH_GET_VALUE(&info->globalCfgs, WIFI_KEYWORD_RATE, index, rate );

		length += CGI_SPRINTF(buf, length, "\t<TR><TD align=\"right\"><strong>%s:</strong></TD><TD>\n\t<select name=\"%s_%d\" style='width:160px;'>\n", 
			gettext(CGI_STR_RATE), WIFI_KEYWORD_RATE, index );
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

	if(info->type ==CGI_WIRELESS_WIFI )
	{
		if(info->wifiInfo.has_range )
		{
			struct iw_range *range = &info->wifiInfo.range;
			int	freq;
			int 	channel;
			length += CGI_SPRINTF(buf, length, "\t<TR><TD align=\"right\"><strong>%s:</strong></TD><TD>\n\t<select name=\"%s_%d\" style='width:160px;'>\n", 
				gettext(CGI_STR_CHANNEL_FREQ), WIFI_KEYWORD_CHANNEL, index );
			for(j=0; j< range->num_frequency; j++ )
			{
				GET_KHZ(freq, range->freq[j]);
				MESH_GET_VALUE(&info->globalCfgs, WIFI_KEYWORD_CHANNEL, index, value );
				channel =atoi( value);
				length += CGI_SPRINTF(buf,length, "\t\t<option value=\"%d\" %s>%s %d(%d MHz)</option>\n", 
					range->freq[j].i,(range->freq[j].i==channel)?"selected":"", gettext(CGI_STR_CHANNEL), iw_freq_to_channel(freq, range), freq/KILO_INT );
			}
			length += CGI_SPRINTF(buf,length, "\t</select>\n</TD></TR>\r\n");
		}
		else
		{
			length += CGI_SPRINTF(buf,length, "<TR><TD colspan=2 ><strong>%s</strong>:%s</TD></TR>\r\n",gettext(CGI_STR_NOTE), gettext("Channel Parameter can be set only when drivers have been loaded") );
		}
	}
	else
	{
		mesh_info_t *mInfo = &info->meshInfo[index];
		if( mInfo->numFreq > 0 )//|| mInfo->numChannel >0 )
		{
			int channel;
			length += CGI_SPRINTF(buf, length, "\t<TR><TD align=\"right\"><strong>%s:</strong></TD><TD>\n\t<select name=\"%s_%d\" style='width:160px;'>\n", 
				gettext(CGI_STR_CHANNEL_FREQ), WIFI_KEYWORD_CHANNEL, index );
			for(j=0; j< mInfo->numFreq; j++ )
			{
				MESH_GET_VALUE(&info->globalCfgs, WIFI_KEYWORD_CHANNEL, index, value );
				channel =atoi( value);
				length += CGI_SPRINTF(buf,length, "\t\t<option value=\"%d\" %s>%s %d(%d MHz)</option>\n", 
					j+1, ( (j+1)==channel)?"selected":"", gettext(CGI_STR_CHANNEL), j+1, mInfo->freqs[j] );
			}
			length += CGI_SPRINTF(buf,length, "\t</select>\n</TD></TR>\r\n");
		}
		else
		{
			length += CGI_SPRINTF(buf,length, "<TR><TD colspan=2 ><strong>%s</strong>:%s</TD></TR>\r\n",gettext(CGI_STR_NOTE), gettext("Channel Parameter can not access from MESH device") );
		}
	}
	
	return strdup(buf);
}



char *__mesh_params_status(WIFI_INFO *info)
{
	char 	buf[8192*5];
	int 		length = 0;
	int		nicCount = 0;
	int		nicConfig = 0;
	int		i;

	length += CGI_SPRINTF(buf,length, "<TR><TD colspan=2><strong>%s</strong><p></TD>\n", gettext("Set Wireless Basic Parameters") );
	nicCount = atoi( GET_VALUE(&info->meshDevs, WIFI_KEYWORD_MESH_NUM) );
	nicConfig = atoi( GET_VALUE(&info->globalCfgs, WIFI_KEYWORD_MESH_NUM) );
	if(nicConfig!=nicCount)
	{
		length += CGI_SPRINTF(buf,length, "<TR><TD colspan=2><strong>%s : %d</strong><p></TD>\n", gettext("MESH Device configed is") , nicConfig );
	}
	

	length += CGI_SPRINTF(buf, length, "<form name=\"wifiForm\" method=\"post\" action=\"%s\">\n", (info->type==CGI_WIRELESS_WIFI)?WEB_URL_WLAN:WEB_URL_MESH );
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_KEYWORD_OPERATOR, CGI_WIRELESS_OP_MODE);
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_ACTION, CGI_ACTION_ADD);
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%d\">\n", WIFI_KEYWORD_MESH_NUM, nicCount );

	for(i=0; i< nicCount; i++)
	{
		length += CGI_SPRINTF(buf,length, "%s",__mesh_params_nic_status( info, i) );
	}

	length += CGI_SPRINTF(buf,length, "<TR><TD colspan=2 align=\"center\">%s\r\n",cgi_submit_button( gettext(CGI_STR_NEXT) ) );
	length += CGI_SPRINTF(buf,length, "\t\t%s</TD></TR></FORM>\r\n" , cgi_help_button(CGI_HELP_MSG_PBX_ACCOUNT) );

	return strdup(buf);
}

char *__mesh_result_nic_info(WIFI_INFO *info, int index)
{
	char 	buf[8192*5];
	int 		length = 0;
	int 		i;
	int		isValidate = 0;
	char		buffer[128];
	rate_type_t	*rate = rates_11g;
	char			*currentRate;
	char			*value;
	struct phy_mode *phyMode = _wifiphys;

	length += CGI_SPRINTF(buf,length, "<TR><TD colspan=2 align=\"center\" bgcolor=\"#cccccc\"><strong>%s %d</strong></TD>\n", gettext(CGI_STR_MESH_DEVICE), index+1 );

	MESH_GET_VALUE(&info->globalCfgs, WIFI_KEYWORD_MODE, index, value );
	length += sprintf(buf+length, "<TR><TD  align=\"right\"><strong>%s:</strong></TD><TD>%s</TD></TR>\r\n", 
		gettext(CGI_STR_MODE), value);

	MESH_GET_VALUE(&info->globalCfgs, WIFI_KEYWORD_ONBOOT, index, value );
	length += sprintf(buf+length, "<TR><TD  align=\"right\"><strong>%s:</strong></TD><TD>%s</TD></TR>\r\n", 
		gettext(CGI_STR_START_WHEN_BOOT), value);

	MESH_GET_VALUE(&info->globalCfgs, WIFI_KEYWORD_ESSID, index, value );
	length += sprintf(buf+length, "<TR><TD  align=\"right\"><strong>%s:</strong></TD><TD>%s</TD></TR>\r\n", 
		gettext(CGI_STR_ESSID), value);

	MESH_GET_VALUE(&info->globalCfgs, WIFI_KEYWORD_NICKNAME, index, value );
	length += sprintf(buf+length, "<TR><TD align=\"right\"><strong>%s:</strong></TD><TD>%s</TD></TR>\r\n", 
		gettext(CGI_STR_NICKNAME), value );

	MESH_GET_VALUE(&info->globalCfgs, WIFI_KEYWORD_PHY_MODE, index, value );
	i = atoi(value);
	
	while(phyMode->type != -1 )
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

	MESH_GET_VALUE(&info->globalCfgs, WIFI_KEYWORD_CHANNEL, index, value );
	i = atoi(value);

	if(info->type == CGI_WIRELESS_WIFI )
	{
		sprintf(buffer, "%d(%d MHz)", i, iw_channel_to_freq(i, &info->wifiInfo.range)/KILO_INT );
		length += CGI_SPRINTF(buf, length, "\t<TR><TD align=\"right\"><strong>%s:</strong></TD><TD>%s</TD></TR>\r\n", 
			gettext(CGI_STR_CHANNEL_FREQ), buffer );
	}
	else
	{
		sprintf(buffer, "%d(%d MHz)", i, ((i-1) < info->meshInfo[index].numFreq)? info->meshInfo[index].freqs[i-1]: -1 );
		length += CGI_SPRINTF(buf, length, "\t<TR><TD align=\"right\"><strong>%s:</strong></TD><TD>%s</TD></TR>\r\n", 
			gettext(CGI_STR_CHANNEL_FREQ), buffer );
	}
	
	MESH_GET_VALUE(&info->globalCfgs, WIFI_KEYWORD_RATE, index, currentRate );
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

char *__mesh_result_info(WIFI_INFO *info)
{
	char 	buf[8192*5];
	int 		length = 0;
	int 		i;
	int		nicCount = 0;

	nicCount = atoi( GET_VALUE(&info->meshDevs, WIFI_KEYWORD_MESH_NUM) );

	length += CGI_SPRINTF(buf,length, "<TR><TD colspan=2><strong>%s</strong><p></TD></TR>\n", gettext("Wireless Basic Parameters") );

	length += CGI_SPRINTF(buf, length, "\t<TR><TD  align=\"right\"><strong>%s:</strong></TD>\n\t<TD>%s</TD></TR>\n", 
		gettext(CGI_STR_START_WHEN_BOOT), GET_VALUE(&info->globalCfgs, WIFI_KEYWORD_ONBOOT) );
	for(i=0; i< nicCount; i++)
	{
		length += CGI_SPRINTF(buf,length, "%s",__mesh_result_nic_info( info, i) );
	}

	return strdup(buf);
}

int	cgi_mesh_mode(WIFI_INFO *info)
{
	char *action;

	action = GET_VALUE( &info->cgiVariables, CGI_ACTION);
	if(!action || !strlen(action) )
	{
		cgi_info_page(gettext("Mode Configuration of Wireless"), gettext("Basic parameters of Wireless are set here"), __mesh_mac_mode_status(info));
		return 0;
	}	

	cgi_mesh_save_config(info);

	if(!strcasecmp(action, CGI_ACTION_DEFAULT) )
	{/* mode save and display basic parameters */
		cgi_info_page(gettext("Basic Parameters of Wireless"), gettext("Basic parameters of Wireless are set here"), __mesh_params_status(info));
		return 0;
	}

	if(!strcasecmp(action, CGI_ACTION_ADD) )
	{/* mode save */
		cgi_info_page(gettext("Basic Parameters of Wireless"),  __mesh_result_info(info), "");
		return 0;
	}
	
	return 0;
}

