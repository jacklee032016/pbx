/*
* $Id: fwParseCurrent.c,v 1.1.1.1 2006/11/29 17:06:59 lizhijie Exp $
*/

#include "cgi_lib.h"
#include "cgi_iptables.h"

/* parameter return with the format of address/mask */
static void __change_netmask_to_int(char *IPmask)
{
	unsigned short mask_len=0;
	char * record=NULL;
	unsigned long intmask;
	char IP[20];
	char mask[20];
	
	if( strlen(IPmask) )
	{
		memset(IP,0,sizeof(mask));
		memset(mask,0,sizeof(mask));
		record=index(IPmask,'/');
		if(record==NULL)
			return ;
		
		strcpy(mask,record+1);
		record[0]='\0';
		strcpy(IP,IPmask);
		intmask=inet_addr(mask);
		//intmask=~intmask;
		while (intmask&0x80000000)
		{
			mask_len++;
			intmask=intmask<<1;
		}		
		//mask_len=32-mask_len;
		int_to_str(mask_len,mask);
		sprintf(IPmask,"%s/%s",IP,mask);
	}
}

/* parse the content in the format of xx:xx or other into 2 parts */
static void __parse_iptables_value(char * value1, char *value2,char key)
{
	char tmpvalue[60];
	char * record=NULL;
	int len=0;
	memset(tmpvalue,0,sizeof(tmpvalue));
	
	if(strlen(value1))
	{
		strcpy(tmpvalue,value1);
		memset(value1,0,strlen(value1));
		record=index(tmpvalue,key);
		if(record!=NULL)
		{
			len=record-tmpvalue;
			strncpy(value1,tmpvalue,len);
			strcat(value1,"\0");
			memset(value2,0,strlen(value2));
			strcpy(value2,record);
		}
		else
			strcpy(value1,tmpvalue);
	}

}


static iptables_item *__parse_iptables_line(char *line )
{
	int 		i;
	char		fileline[25][128];
	iptables_item *p = (iptables_item *)malloc(sizeof(iptables_item) );
	if(!p)
		return NULL;
	memset(p, 0, sizeof(iptables_item) );

	memset(fileline,0,sizeof(fileline));
	for(i=0;i<strlen(line);i++)
		if( (line[i]=='\t') || (line[i]=='\r') ||(line[i]=='\n') )
			line[i]=' ';

	sscanf(line,"%s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s",
		fileline[0],fileline[1],fileline[2],fileline[3],fileline[4],fileline[5],fileline[6],fileline[7],fileline[8],fileline[9],fileline[10],
		fileline[11],fileline[12],fileline[13],fileline[14],fileline[15],fileline[16],fileline[17],fileline[18],fileline[19],fileline[20],
		fileline[21],fileline[22],fileline[23],fileline[24]);
	
	for(i=0;i<25;i++)
	{
		if(!strcmp(fileline[i],"-A"))
			strcpy(p->rule,fileline[i+1]);
		if(!strcmp(fileline[i],"-s"))
			strcpy(p->interAddr,fileline[i+1]);
		if(!strcmp(fileline[i],"-d"))
			strcpy(p->extAddrBegin,fileline[i+1]);
		if(!strcmp(fileline[i],"-i"))
			strcpy(p->interDevice,fileline[i+1]);
		if(!strcmp(fileline[i],"-o"))
			strcpy(p->extDevice,fileline[i+1]);
		if(!strcmp(fileline[i],"-p"))
			strcpy(p->protocol,fileline[i+1]);
		if(!strcmp(fileline[i],"--sport"))
			strcpy(p->interPortBegin,fileline[i+1]);
		if(!strcmp(fileline[i],"--dport"))
			strcpy(p->extPortBegin,fileline[i+1]);
		if(!strcmp(fileline[i],"--state"))
			strcpy(p->state1,fileline[i+1]);
		if(!strcmp(fileline[i],"--to-source"))
			strcpy(p->extAddrBegin,fileline[i+1]);
		if(!strcmp(fileline[i],"-j"))
			strcpy(p->action,fileline[i+1]);
		if(!strcmp(fileline[i],"--mac-source"))
			strcpy(p->mac,fileline[i+1]);
	}
//	if(!strlen(p->protocol))
//		strcpy(p->protocol,"all");
	if(!strlen(p->action))
		strcpy(p->action,"ACCEPT");

	return p;
}

iptables_item  *cgifw_read_iptables_actives()
{
	int i,linelen;
	FILE *fp;
	char line[PARSE_FILE_MAX_LENGTH];
	iptables_item *header = NULL;
	iptables_item  *current = NULL, *last = NULL;
	char	cmd[128];

	sprintf(cmd, IPTABLES_SAVE" -t nat > %s", IPTABLES_NAT_SAVE);
	system(cmd);
	
	if( (fp=fopen(IPTABLES_NAT_SAVE, "r"))==NULL)
		return NULL;

	while ( fgets(line,PARSE_FILE_MAX_LENGTH, fp) )
	{
cgitrace;
		linelen = strlen(line);
		for(i=linelen-1;i>0;i--)
		{
			if( (line[i]=='\t' )|| (line[i]=='\n') ||( line[i]=='\r') )
			line[i]=' ';
		}
		if(!strstr(line,"--dport 1723")&&!strstr(line,"-p 47"))
		{
			if( (line[0]=='-') && (line[1]=='A') )
			{
cgitrace;
				current = __parse_iptables_line(line );
				if(current)
				{
				cgitrace;

					if(header== NULL)
						header = current;
					__parse_iptables_value(current->extAddrBegin,current->extPortBegin,':');
					__change_netmask_to_int(current->interAddr);
					__change_netmask_to_int(current->extAddrBegin);
//					output_html_iptables(&iptables_info);
					if(last )
						last->next = current;
					last = current;
				}
			}
		}
	}

	return header;
}

#if IPTABLES_DEBUG
void fwNat_debug_item(iptables_item *item)
{
	cgidebug("Action \t\t: %s\r\n", item->action);
	cgidebug("Protocol \t\t: %s\r\n", item->protocol);
	cgidebug("Rule \t\t: %s\r\n", item->rule );
	
	cgidebug("WAN Device \t\t: %s\r\n", item->extDevice );
	cgidebug("Ext Address Being \t: %s\r\n", item->extAddrBegin);
	cgidebug("Ext Address End \t: %s\r\n", item->extAddrEnd);
	cgidebug("Ext Port Being \t: %s\r\n", item->extPortBegin );
	cgidebug("Ext Port End \t: %s\r\n", item->extPortEnd);
	
	cgidebug("LAN Device \t: %s\r\n", item->interDevice);
	cgidebug("Int Address \t: %s\r\n", item->interAddr);
	cgidebug("Int Port Begin \t: %s\r\n", item->interPortBegin );
	cgidebug("Int Port End \t: %s\r\n", item->interPortEnd);


	cgidebug("MAC \t\t\t: %s\r\n", item->mac);
	cgidebug("State1 \t\t: %s\r\n", item->state1);
	cgidebug("State2 \t\t: %s\r\n", item->state2 );

}
#endif

