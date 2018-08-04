/*
* $Id: wifiStations.c,v 1.5 2007/09/06 09:19:59 lizhijie Exp $
*/

#include "cgi_lib.h"
#include "cgi_wifi.h"
#include "cgi_syscfg.h"

#define	WIFI_STATION_PROC					"/proc/sys/net/"

int	__station_init(WIFI_INFO *info)
{
#define	TMP_FILE	"/tmp/stas.txt"

	char		line[PARSE_FILE_MAX_LENGTH];
	FILE  	*fp;
	char		t1[32], t2[32], t3[32], t4[32];
	int		i = 0;
	char		filename[128];
	wifi_station_info_t *station = NULL;
	char		cmd[128];

	info->stations = NULL;
	
//	sprintf(filename, WIFI_STATION_PROC"%s/associated_sta", info->wifiInfo.b.name );
	sprintf(filename, WIFI_STATION_PROC"%s/associated_sta", "wifi0");//GET_VALUE(&info->nicCfgs, NIC_KEYWORD_DEVICE));
//	if((fp=fopen(filename, "r"))==NULL)
	sprintf(cmd, "cat %s > "TMP_FILE, filename );
	system(cmd);
	if((fp=fopen(TMP_FILE, "r"))==NULL)
	{
		fprintf(stderr, "Open WIFI Stations proc file '%s' failure!\n", filename );
		return -1;
	}

	i = 0;
	while( fgets(line,PARSE_FILE_MAX_LENGTH,fp) )
	{
		int __count = 0;
		
		if(strstr(line, "macaddr:" ) )
		{
			station = (wifi_station_info_t *)malloc(sizeof(wifi_station_info_t) );
			if(!station )
			{
				return -1;
			}
			memset(station, 0, sizeof(wifi_station_info_t) );
			info->stations = glist_add( info->stations, i, station );
			__count = sscanf(line, "%s %s", t1, t2 );
			snprintf(station->macAddress, strlen(t2)-1, "%s", t2+1 );

			i++;
		}
		else if(strstr(line, "txseq:" ) )
		{
			__count = sscanf(line, "%s %s %s %s", t1, t2, t3, t4 );
			station->txPackets = atoi(t2);
			station->rxPackets = atoi(t4);
		}
		else if(strstr(line, "fails:" ) )
		{
			__count = sscanf(line, "%s %s %s %s", t1, t2, t3, t4 );
			station->failedPackets = atoi(t2);
		}
		
	}
	fclose(fp);

	return 0;
}

char *__wifi_station_status(WIFI_INFO *info)
{
	char 	buf[8192*5];
	int 		length = 0;
	int 		i = 0;
	wifi_station_info_t *station = NULL;

//	length += CGI_SPRINTF(buf,length, "<table width=\"95%%\" border=\"2\" align=\"center\" cellpadding=\"1\" cellspacing=\"1\" bgcolor=\"#FFFFFF\" >\n");
	length += CGI_SPRINTF(buf, length, "\t<TR><TD align=\"center\"><strong>%s</strong></TD><TD align=\"center\"><strong>%s</strong></TD><TD align=\"center\"><strong>%s</strong></TD><TD align=\"center\"><strong>%s</strong></TD></TR>\n", 
		gettext(CGI_STR_MAC_ADDRESS), gettext("TX Packets"), gettext("RX Packets") , gettext("Failed Packets")  );


	while(1)
	{
		station = glist_get_data(info->stations, i);
		if(!station )
			break;
		
		length += CGI_SPRINTF(buf,length, "<TR><TD align=\"center\">%s</TD>\r\n", station->macAddress );
		length += CGI_SPRINTF(buf,length, "\t\t<TD align=\"center\">%d</TD><TD align=\"center\">%d</TD><TD align=\"center\">%d</TD></TR>\r\n" , 
			station->txPackets, station->rxPackets, station->failedPackets );

		i++;
	};
	
//	length += CGI_SPRINTF(buf, length, "</table>\n" );

	length += CGI_SPRINTF(buf, length, "%s", cgi_refresh_script(WEB_URL_WLAN"?"CGI_KEYWORD_OPERATOR"="CGI_WIFI_OP_STATION, 5 ) );

	return strdup(buf);
}


int	wifi_Stations(WIFI_INFO *info)
{
	__station_init(info);

	cgi_info_page(gettext("Current Associated Stations Information"), __wifi_station_status(info), "" );
	return 0;
}

