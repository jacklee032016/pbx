/*
* $Id: fwFilterServices.c,v 1.1.1.1 2006/11/29 17:06:59 lizhijie Exp $
*/

//#define	

typedef	struct
{
	char		*serviceName;
	char		*protocol;
	int		port;
}iptables_filter_t;


iptables_filter_t 	servicesFilters[] =
{
	{
		IPT_SERVICE_WWW,
		IPTABLES_PROTOCOL_NAME_TCP,	
	},
};

static void cmmn_iptbls_serviceFilter(
	service_Info * pservice,
	char * LAN,
	char * WAN,
	int visit_Internet)
{
	char cmd[256];
	char servicePort[10];
	int i=0;
	
	memset(cmd,0,256);
	memset(servicePort,0,10);

	if(visit_Internet==1) //can visit Internet
	{
		for(i=0;i<SERVICE_NUM;i++)
		{
			if( !strcmp(pservice[i].servicename,WWW) && (atoi(pservice[i].authority)==0) )
			{
				sprintf(cmd,"%s/iptables -A FORWARD -i %s -o %s -p tcp --dport 80 -j DROP",IPTABLES_DIR,LAN,WAN);
				system(cmd);
				memset(cmd,0,256);
				sprintf(cmd,"%s/iptables -A FORWARD -i %s -o %s -p tcp --dport 8080 -j DROP",IPTABLES_DIR,LAN,WAN);
				system(cmd);
			}
			else if( !strcmp(pservice[i].servicename,FTP) && (atoi(pservice[i].authority)==0) )
			{
				memset(cmd,0,256);
				sprintf(cmd,"%s/iptables -A FORWARD -i %s -o %s -p tcp --dport 21 -j DROP",IPTABLES_DIR,LAN,WAN);
				system(cmd);
				memset(cmd,0,256);
				sprintf(cmd,"%s/iptables -A FORWARD -i %s -o %s -p udp --dport 69 -j DROP",IPTABLES_DIR,LAN,WAN);
				system(cmd);
				memset(cmd,0,256);
				sprintf(cmd,"%s/iptables -A FORWARD -i %s -o %s -p tcp --dport 6881:6889 -j DROP",IPTABLES_DIR,LAN,WAN);	
				system(cmd);
			}
			else if( !strcmp(pservice[i].servicename,EMAIL) && (atoi(pservice[i].authority)==0) )
			{
				memset(cmd,0,256);
				sprintf(cmd,"%s/iptables -A FORWARD -i %s -o %s -p tcp --dport 25 -j DROP",IPTABLES_DIR,LAN,WAN);	
				system(cmd);
			}	
			else if( !strcmp(pservice[i].servicename,QQ) && (atoi(pservice[i].authority)==0) )
			{
				memset(cmd,0,256);
				sprintf(cmd,"%s/iptables -A FORWARD -i %s -o %s -p tcp --dport 8000 -j DROP",IPTABLES_DIR,LAN,WAN);	
				system(cmd);
			}
		
		}
	}
	else //can't visit INternet
	{
		for(i=0;i<SERVICE_NUM;i++)
		{
			if( !strcmp(pservice[i].servicename,WWW) && (atoi(pservice[i].authority)==1) )
			{
				sprintf(cmd,"%s/iptables -A FORWARD -i %s -o %s -p tcp --dport 80 -j ACCEPT",IPTABLES_DIR,LAN,WAN);
				system(cmd);
				sprintf(cmd,"%s/iptables -A FORWARD -i %s -o %s -p tcp --sport 80 -j ACCEPT",IPTABLES_DIR,WAN,LAN);
				system(cmd);
				
				memset(cmd,0,256);
				sprintf(cmd,"%s/iptables -A FORWARD -i %s -o %s -p tcp --dport 8080 -j ACCEPT",IPTABLES_DIR,LAN,WAN);
				system(cmd);
				sprintf(cmd,"%s/iptables -A FORWARD -i %s -o %s -p tcp --sport 8080 -j ACCEPT",IPTABLES_DIR,WAN,LAN);
				system(cmd);
			}
			else if( !strcmp(pservice[i].servicename,FTP) && (atoi(pservice[i].authority)==1) )
			{
				memset(cmd,0,256);
				sprintf(cmd,"%s/iptables -A FORWARD -i %s -o %s -p tcp --dport 21 -j ACCEPT",IPTABLES_DIR,LAN,WAN);
				system(cmd);
				sprintf(cmd,"%s/iptables -A FORWARD -i %s -o %s -p tcp --sport 21 -j ACCEPT",IPTABLES_DIR,WAN,LAN);
				system(cmd);

				memset(cmd,0,256);
				sprintf(cmd,"%s/iptables -A FORWARD -i %s -o %s -p udp --dport 69 -j ACCEPT",IPTABLES_DIR,LAN,WAN);
				system(cmd);
				sprintf(cmd,"%s/iptables -A FORWARD -i %s -o %s -p udp --sport 69 -j ACCEPT",IPTABLES_DIR,WAN,LAN);
				system(cmd);
				
				memset(cmd,0,256);
				sprintf(cmd,"%s/iptables -A FORWARD -i %s -o %s -p tcp --dport 6881:6889 -j ACCEPT",IPTABLES_DIR,LAN,WAN);
				system(cmd);
				sprintf(cmd,"%s/iptables -A FORWARD -i %s -o %s -p tcp --sport 6881:6889 -j ACCEPT",IPTABLES_DIR,WAN,LAN);
				system(cmd);
			}
			else if( !strcmp(pservice[i].servicename,EMAIL) && (atoi(pservice[i].authority)==1) )
			{
				memset(cmd,0,256);
				sprintf(cmd,"%s/iptables -A FORWARD -i %s -o %s -p tcp --dport 25 -j ACCEPT",IPTABLES_DIR,LAN,WAN);	
				system(cmd);
				sprintf(cmd,"%s/iptables -A FORWARD -i %s -o %s -p tcp --sport 25 -j ACCEPT",IPTABLES_DIR,WAN,LAN);	
				system(cmd);
			}	
			else if( !strcmp(pservice[i].servicename,QQ) && (atoi(pservice[i].authority)==1) )
			{
				memset(cmd,0,256);
				sprintf(cmd,"%s/iptables -A FORWARD -i %s -o %s -p tcp --dport 8000 -j ACCEPT",IPTABLES_DIR,LAN,WAN);	
				system(cmd);
				sprintf(cmd,"%s/iptables -A FORWARD -i %s -o %s -p tcp --sport 8000 -j ACCEPT",IPTABLES_DIR,WAN,LAN);	
				system(cmd);
			}
		
		}
	}	
}


static void cmmn_iptbls_hostFilter(
	host_filter *phost,
	char * LAN,
	char * WAN,
	int visit_Internet)
{
	char cmd[256];
	char target[10];
	int i=0;

	while(i<HOST_FILTER_NUM)
	{
		memset(target,0,10);
		memset(cmd,0,256);
		if( (visit_Internet==1) && ( atoi(phost[i].authority)==0) )
		{
			strcpy(target,DROP);
		}
		if( (visit_Internet==0) && ( atoi(phost[i].authority)==1) )
		{
			strcpy(target,ACCEPT);
		}

		if( strlen(phost[i].host) )
		{
			if( strlen(phost[i].MAC_addr ) )
				sprintf(cmd,"%s/iptables -A FORWARD -s %s -i %s -o %s -p all -m mac --mac-source %s -j %s",
				IPTABLES_DIR,phost[i].host,LAN,WAN,phost[i].MAC_addr,target);
			else
				sprintf(cmd,"%s/iptables -A FORWARD -s %s -i %s -o %s -p all -j %s",
				IPTABLES_DIR,phost[i].host,LAN,WAN,target);
		}
		else
		{
			if( strlen(phost[i].MAC_addr ) )
				sprintf(cmd,"%s/iptables -A FORWARD -i %s -o %s -p all -m mac --mac-source %s -j %s",
				IPTABLES_DIR,LAN,WAN,phost[i].MAC_addr,target);
		}

		if(strlen(cmd))
			system(cmd);
		i++;			
	}
	
}


static void cmmn_iptbls_addIptablesRule( 
	host_filter * phost,
	service_Info * pservice,
	eth_addr * ppp0,
	eth_addr * peth0,
	eth_addr * peth1)

{
	int visit_Internet=-1;
	int i=0;
	char cmd[256];
	char WAN[20];
	char LAN[20];
	char local_netaddr[40];
	memset(cmd,0,256);
	memset(WAN,0,20);
	memset(LAN,0,20);
	memset(local_netaddr,0,40);

	while(i<SERVICE_NUM)
	{
		if( !strcmp(pservice[i].servicename,INTERNET) )
		{
			if( atoi(pservice[i].authority)==1 )
				visit_Internet=1;
			else if( atoi(pservice[i].authority)==0 )
				visit_Internet=0;
		}
		i++;
	}
	
	//make nat(POSTROUTING) empty
	sprintf(cmd,"%s/iptables -F -t nat",IPTABLES_DIR);
	system(cmd);

	memset(cmd,0,256);
	//make filter empty
	sprintf(cmd,"%s/iptables -F",IPTABLES_DIR);
	system(cmd);
	//add NAT(POSTROUTING) rule
	if( (peth1->state==1)	&& strlen(peth1->addr) )
	{
		cmmn_iptbls_getNetworkaddr(local_netaddr,peth1->addr,peth1->netmask);
		strcpy(LAN,ETH1);
		memset(cmd,0,256);
		if(ppp0->state==1)
		{
			sprintf(cmd,"%s/iptables -t nat -A POSTROUTING -s %s -o ppp0 -p all -j MASQUERADE",IPTABLES_DIR,local_netaddr);
			strcpy(WAN,PPP0);
		}
		else
		{
			sprintf(cmd,"%s/iptables -t nat -A POSTROUTING -s %s -o eth0 -p all -j SNAT --to-source %s",IPTABLES_DIR,local_netaddr,peth0->addr);
			strcpy(WAN,ETH0);
		}
		system(cmd);	
	}
	else
		return ;
	
	cmmn_iptbls_serviceFilter(pservice,LAN,WAN,visit_Internet); 
	cmmn_iptbls_hostFilter(phost,LAN,WAN,visit_Internet);

	if(visit_Internet==1)
	{
		memset(cmd,0,256);
		sprintf(cmd,"%s/iptables -A FORWARD -s %s -i %s -o %s -p all -j ACCEPT",
			IPTABLES_DIR,local_netaddr,LAN,WAN);
		system(cmd);
		sprintf(cmd,"%s/iptables -A FORWARD -d %s -i %s -o %s -p all  -j ACCEPT",
			IPTABLES_DIR,local_netaddr,WAN,LAN);
		system(cmd);
	}
		
}

