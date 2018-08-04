/*
* $Id: wifiInfo.c,v 1.3 2007/09/06 09:19:59 lizhijie Exp $
*/

#include "cgiLib.h"
#include "cgi_wifi.h"

static char *__wifi_params_list(WIFI_INFO *info)
{
	char 	buf[8192*5];
	int 		length = 0;
	wireless_info *wInfo = &info->wifiInfo;
	struct wireless_config *bInfo = &wInfo->b;
	int 		channel;
	char		buffer[128];

//	length += CGI_SPRINTF(buf,length, "<TR><TD colspan=2><strong>%s</strong><p></TD>\n", gettext("Set WiFi Basic Parameters") );
	cgitrace;

	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"right\"><strong>%s</strong></TD><TD>%s</TD>\r\n", 
		gettext(CGI_STR_MODE),(bInfo->has_mode)? iw_operation_mode[bInfo->mode]:"" );
	length += CGI_SPRINTF(buf,length, "<TD align=\"right\"><strong>%s:</strong></TD><TD>%s</TD></TR>\r\n", 
		gettext("PHY"), bInfo->name );
	
	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"right\"><strong>%s</strong></TD><TD>%s</TD>\r\n", 
		gettext(CGI_STR_ESSID),(bInfo->has_essid)? bInfo->essid :"" );
	length += CGI_SPRINTF(buf,length, "<TD align=\"right\"><strong>%s</strong></TD><TD>%s</TD></TR>\r\n", 
		gettext(CGI_STR_NICKNAME), (wInfo->has_nickname)?wInfo->nickname:"");

	if(wInfo->has_range )//&& (freq < KILO_INT))
		channel = iw_freq_to_channel(bInfo->freq, &wInfo->range);
	else
		channel = 0;
	
	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"right\"><strong>%s</strong></TD><TD>%d(%d MHz)</TD>\r\n", 
		gettext("Channel(Frequency)"), channel, (bInfo->has_freq)? bInfo->freq/KILO_INT:0 );
	length += CGI_SPRINTF(buf,length, "<TD align=\"right\"><strong>%s</strong></TD><TD>%s</TD></TR>\r\n", 
		(bInfo->mode == IW_MODE_ADHOC)?gettext("Cell"):gettext("AP(Access Point)"), 
		(wInfo->has_ap_addr)?iw_pr_ether(buffer, wInfo->ap_addr.sa_data):"" );

	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"right\"><strong>%s</strong></TD><TD>%d Mbps</TD>\r\n", 
		gettext("Bit Rate"), (wInfo->has_bitrate)? wInfo->bitrate.value/MEGA_INT:0 );
	if((wInfo->has_txpower))
		iw_print_txpower(buffer, sizeof(buffer), &wInfo->txpower);
	length += CGI_SPRINTF(buf,length, "<TD align=\"right\"><strong>%s</strong></TD><TD>%s</TD></TR>\r\n", 
		gettext("TX Power"), (wInfo->has_txpower)?buffer:"" );

	if(wInfo->has_rts)
	{
		if(wInfo->rts.disabled)
			sprintf(buffer, "%s", "off");
		else
			sprintf(buffer, "%d B", wInfo->rts.value);
	}
	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"right\"><strong>%s</strong></TD><TD>%s</TD>\r\n", 
		gettext("RTS Thrushold"), (wInfo->has_rts)? buffer:"" );
	if(wInfo->has_frag)
	{
		if(wInfo->frag.disabled)
			sprintf(buffer, "%s", "off");
		else
			sprintf(buffer, "%d B", wInfo->frag.value);
	}
	length += CGI_SPRINTF(buf,length, "<TD align=\"right\"><strong>%s</strong></TD><TD>%s</TD></TR>\r\n", 
		gettext("Fragment Thrushold"), (wInfo->has_frag)?buffer:"" );

	if(wInfo->has_rts)
	{
		if(wInfo->rts.disabled)
			sprintf(buffer, "%s", "off");
		else
			sprintf(buffer, "%d B", wInfo->rts.value);
	}
	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"right\"><strong>%s</strong></TD><TD>%s</TD>\r\n", 
		gettext("Signal level"), (wInfo->has_rts)? buffer:"" );
	if(wInfo->has_frag)
	{
		if(wInfo->frag.disabled)
			sprintf(buffer, "%s", "off");
		else
			sprintf(buffer, "%d B", wInfo->frag.value);
	}
	length += CGI_SPRINTF(buf, length, "<TD align=\"right\"><strong>%s</strong></TD><TD>%s</TD></TR>\r\n", 
		gettext("Noise level"), (wInfo->has_frag)?buffer:"" );

	length += CGI_SPRINTF(buf, length, "%s", cgi_refresh_script(WEB_URL_WLAN"?"CGI_KEYWORD_OPERATOR"="CGI_WIFI_OP_STATUS, 5) );
	return strdup(buf);
}

int	wifi_Info(WIFI_INFO *info)
{
#if 0
	char *action;

	action = GET_VALUE( &info->cgiVariables, CGI_ACTION);
	if(!action || !strlen(action) )
	{
		cgi_info_page(gettext("Mode Configuration of WiFi"), gettext("Basic parameters of WiFi are set here"), __wifi_mac_mode_status(info));
		return 0;
	}	
#endif
//	cgi_info_page(gettext("Parameters Info of WiFi"), gettext("Parameters Info of WiFi are list here"), __wifi_params_list(info));
	cgitrace;
	cgi_info_page(gettext("Parameters Info of WiFi"), __wifi_params_list(info), "");
	
	return 0;
}

