/*
* $Id: cgiIpTablesOps.c,v 1.1.1.1 2006/11/29 17:06:59 lizhijie Exp $
*/

static int iptables_add(iptables_item *p)
{
	char cmd[CMD_LENGTH_NAT];
	char localport[20];
	char outport[20];
	char localaddress[30];
	char outaddress[80];
	char state[45];
	char eth[20];
	char cmdhead[40];
	char mac[35];
	char tmp_ch[5];
	memset(cmdhead,0,sizeof(cmdhead));
	int result=0;
	memset(cmd,0,sizeof(cmd));
	memset(localport,0,sizeof(localport));
	memset(outport,0,sizeof(outport));
	memset(localaddress,0,sizeof(localaddress));
	memset(outaddress,0,sizeof(localaddress));
	memset(state,0,sizeof(state));
	memset(eth,0,sizeof(eth));
	memset(mac,0,strlen(mac));
	encapsulation_value(localaddress,p->interAddr,NULL,"-s",NULL);
	
	memset(tmp_ch,0,sizeof(tmp_ch));
	sprintf(tmp_ch,"%s",p->interPortEnd);
	
	if(!tmp_ch[1])
	{
		sprintf(p->interPortEnd,"%s","\0");
	}

	memset(tmp_ch,0,sizeof(tmp_ch));
	sprintf(tmp_ch,"%s",p->extAddrEnd);
	
	if(!tmp_ch[1])
	{
		sprintf(p->extAddrEnd,"%s","\0");
	}

	memset(tmp_ch,0,sizeof(tmp_ch));
	sprintf(tmp_ch,"%s",p->extPortBegin);
	
	if(!tmp_ch[1])
	{
		sprintf(p->extPortBegin,"%s","\0");
	}
	
	memset(tmp_ch,0,sizeof(tmp_ch));
	sprintf(tmp_ch,"%s",p->extPortEnd);
	
	if(!tmp_ch[1])
	{
		sprintf(p->extPortEnd,"%s","\0");
	}
	flag_zero=0;
	encapsulation_value_nat(localport,p->interPortBegin,p->interPortEnd,"--sport",NULL);
	if(flag_zero==1)
		return 0;
	encapsulation_eth(eth,p);
	if(!strcmp(ct,"nat"))
	{
		encapsulation_nat_add_outaddr(outaddress,p);
		flag_zero=0;
		encapsulation_nat_add_outport(outport,p);
		if(flag_zero==1)
			return 0;
		if(!strcmp(p->protocol,"all"))
		sprintf(cmd,"/usr/sbin/iptables -t nat -A POSTROUTING %s %s %s -j %s %s%s",localaddress,eth,
			localport,p->action,outaddress,outport);
		else
		sprintf(cmd,"/usr/sbin/iptables -t nat -A POSTROUTING %s %s -p %s %s -j %s %s%s",localaddress,eth,p->protocol,
			localport,p->action,outaddress,outport);
	}
	if(!strcmp(ct,"filter"))
	{
		encapsulation_value(outaddress,p->extAddrBegin,NULL,"-d",NULL);
		encapsulation_value(outport,p->extPortBegin,p->extPortEnd,"--dport",":");
		encapsulation_value(state,p->state1,p->state2,"-m state --state",",");
		encapsulation_value(mac,p->mac,NULL,"-m mac --mac-source",NULL);	
		sprintf(cmd,"/usr/sbin/iptables -A %s %s %s %s -p %s %s %s %s %s -j %s",p->rule,localaddress,outaddress,eth,p->protocol,
			mac,localport,outport,state,p->action);
	}
	result=system(cmd);
	memset(cmd,0,sizeof(cmd));
	sprintf(cmd,"/usr/sbin/iptables-save -t nat> %s",IPTABLES_NAT_SAVE);
	system(cmd);
	return 0;
}

static int iptables_delete(iptables_item *p)
{
	char cmd[CMD_LENGTH_NAT];
	char localport[20];
	char outport[20];
	char localaddress[30];
	char outaddress[80];
	char state[45];
	char eth[20];
	char cmdhead[40];
	char mac[35];
	memset(cmdhead,0,sizeof(cmdhead));
	int result=0;
	memset(cmd,0,sizeof(cmd));
	memset(localport,0,sizeof(localport));
	memset(outport,0,sizeof(outport));
	memset(localaddress,0,sizeof(localaddress));
	memset(outaddress,0,sizeof(localaddress));
	memset(state,0,sizeof(state));
	memset(eth,0,sizeof(eth));
	memset(mac,0,sizeof(mac));
	
	if(!strcmp(ct,"nat"))
	{
		parse_iptables_value(p->extPortBegin,p->extPortEnd,'-');	
		parse_iptables_value(p->interPortBegin,p->interPortEnd,':');
		parse_iptables_value(p->extAddrBegin,p->extAddrEnd,'-');
		encapsulation_value(localaddress,p->interAddr,NULL,"-s",NULL);
		encapsulation_value(localport,p->interPortBegin,p->interPortEnd,"--sport",NULL);
		encapsulation_eth(eth,p);
		encapsulation_nat_outport(outport,p);
		encapsulation_nat_outaddr(outaddress,p);
		if(strlen(p->protocol))
		sprintf(cmd,"/usr/sbin/iptables -t nat -D POSTROUTING %s %s -p %s %s -j %s %s%s",localaddress,eth,p->protocol,
			localport,p->action,outaddress,outport);
		else
		sprintf(cmd,"/usr/sbin/iptables -t nat -D POSTROUTING %s %s %s -j %s %s%s",localaddress,eth,
			localport,p->action,outaddress,outport);	
	}
	if(!strcmp(ct,"filter"))
	{
		parse_iptables_value(p->extPortBegin,p->extPortEnd,':');
		parse_iptables_value(p->interPortBegin,p->interPortEnd,':');
		parse_iptables_value(p->state1,p->state2,',');
		encapsulation_value(localaddress,p->interAddr,NULL,"-s",NULL);
		encapsulation_value(localport,p->interPortBegin,p->interPortEnd,"--sport",":");
		encapsulation_eth(eth,p);
		encapsulation_value(outaddress,p->extAddrBegin,NULL,"-d",NULL);
		encapsulation_value(outport,p->extPortBegin,p->extPortEnd,"--dport",":");
		encapsulation_state(state,p->state1,p->state2,"-m state --state",",");
		encapsulation_value(mac,p->mac,NULL,"-m mac --mac-source",NULL);
		
		sprintf(cmd,"/usr/sbin/iptables -D %s %s %s %s -p %s %s %s %s %s -j %s",p->rule,localaddress,outaddress,eth,p->protocol,
			mac,localport,outport,state,p->action);
	}
	//printf("%s<br>",cmd);
	result=system(cmd);
	sprintf(cmd,"/usr/sbin/iptables-save -t nat> %s",IPTABLES_NAT_SAVE);
	system(cmd);
	//printf("%d",result);
	//write_conf_iptables(NAT_CONF,cmd,result,"delete");
	
	return 0;
}


