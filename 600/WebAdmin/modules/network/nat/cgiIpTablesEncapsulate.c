/*
* $Id: cgiIpTablesEncapsulate.c,v 1.1.1.1 2006/11/29 17:06:59 lizhijie Exp $
*/

static void encapsulation_nat_outaddr(char *address, iptables_item *p)
{
	if(!strcmp(p->action, IPTABLES_KEYWORD_SNAT) )
	{
		if(strlen(p->extAddrBegin))
		{
			if(strlen(p->extAddrEnd))
			{
				sprintf(address,"--to-source %s%s",p->extAddrBegin, p->extAddrEnd);
			}
			else
				sprintf(address,"--to-source %s",p->extAddrBegin);
		}
		else
			sprintf(address,"%s","");
	}
	
	if(!strcmp(p->action, IPTABLES_KEYWORD_MASQUERADE))
		sprintf(address,"%s","\0");
	
}
static void encapsulation_nat_add_outaddr(char * address,iptables_item *p)
{
	if(!strcmp(p->action, IPTABLES_KEYWORD_SNAT))
	{
		if(strlen(p->extAddrBegin))
		{
			if(strlen(p->extAddrEnd))
			{
				sprintf(address,"--to-source %s%s",p->extAddrBegin,p->extAddrEnd);
			}
			else
				sprintf(address,"--to-source %s",p->extAddrBegin);
		}
		else
			sprintf(address,"%s","");
	}
	
	if(!strcmp(p->action, IPTABLES_KEYWORD_MASQUERADE))
		sprintf(address,"%s","\0");
}

static void encapsulation_nat_outport(char *port,iptables_item *p)
{
	/*char tmp_value[128];
	memset(tmp_value,0,sizeof(tmp_value));
	sprintf(tmp_value,"%s",p->extPortBegin);*/
	if(p->extPortBegin[1])
	{
		if(strlen(p->extPortEnd))
			sprintf(port,"%s%s",p->extPortBegin,p->extPortEnd);
		else
			sprintf(port,"%s",p->extPortBegin);		
	}
	else
		sprintf(port,"%s","\0");
}

static void encapsulation_nat_add_outport(char *port,iptables_item *p)
{
	if(strlen(p->extPortBegin))
	{
		if(strlen(p->extPortEnd))
			sprintf(port,"%s%s",p->extPortBegin,p->extPortEnd);
		else
			sprintf(port,"%s",p->extPortBegin);		
	}
	else
	{
		if(strlen(p->extPortEnd))
		flag_zero=1;
		else
		sprintf(port,"%s","\0");
	}	
}
static void encapsulation_eth(char * eth,iptables_item * p)
{
	
	if(!strcmp(p->rule, IPTABLES_KEYWORD_TBL_INPUT))
	{
		if(strlen(p->interDevice))
			sprintf(eth,"-i %s",p->interDevice);
	}		
	else if(!strcmp(p->rule, IPTABLES_KEYWORD_TBL_OUTPUT))
	{
		if(strlen(p->extDevice))
			sprintf(eth,"-o %s",p->extDevice);		
	}
	else if(!strcmp(p->rule, IPTABLES_KEYWORD_TBL_FORWARD ) )
	{
		if(strlen(p->interDevice))
		{
			if(strlen(p->extDevice))
				sprintf(eth,"-i %s -o %s",p->interDevice,p->extDevice);
			else
				sprintf(eth,"-i %s",p->interDevice);
		}
	}
	else if(strlen(p->extDevice))
		sprintf(eth,"-o %s",p->extDevice);
	
}

static void encapsulation_value_nat(char * dvalue,char * value1,char *value2,char * key1,char *key2)
{
	if(strlen(value1))
	{
		if( value2!=NULL)
		{
			sprintf(dvalue,"%s %s%s",key1,value1,value2);
		}
		else
		{
			sprintf(dvalue,"%s %s",key1,value1);
		}
	}
	else
	{
		if(strlen(value2))
			flag_zero=1;
		else
			sprintf(dvalue,"%s","\0");
	}
}

static void encapsulation_value(char * dvalue,char * value1,char *value2,char * key1,char *key2)
{
	if(strlen(value1))
	{
		if( value2!=NULL)
		{
			sprintf(dvalue,"%s %s%s",key1,value1,value2);
		}
		else
		{
			sprintf(dvalue,"%s %s",key1,value1);
		}
	}
	else
		sprintf(dvalue,"%s","\0");
}

static void encapsulation_state(char * state,char * state1,char *state2,char * key1,char * key2)
{
	if(strlen(state1))
	{
		if( strlen(state2) )
			sprintf(state,"%s %s%s%s",key1,state1,key2,state2);
		else
			sprintf(state,"%s %s%s%s",key1,state1,key2,state1);
	}
	else
		sprintf(state,"%s","\0");
}

static void encapsulation_filter_output_value(char * dvalue, char * svalue1, char * svalue2, char * key)
{
	if(strlen(svalue1))
	{
		if(strlen(svalue2))
			sprintf(dvalue,"%s%s%s",svalue1,key,svalue2);
		else
			sprintf(dvalue,"%s",svalue1);
	}
	else
	{
		if(strlen(svalue2))
			sprintf(dvalue,"%s",svalue2);
	}
			
}



