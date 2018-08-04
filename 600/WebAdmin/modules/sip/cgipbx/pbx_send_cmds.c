/*
* $Id: pbx_send_cmds.c,v 1.5 2007/03/26 09:06:26 lizhijie Exp $
*/
#include "_pbx.h"

static unsigned char cmd_seq = 0;

int c2i(char ac)
{
	if(!isdigit(ac))
	{
		return -1;
	}
	else
	{
		return ac-'0';
	}
}

int pbx_send_do_spec_comcmd(unsigned char cmd,char *v1,char *v2)
{
	int slen=strlen(v2), i=0, v=0;
	//char v2tmp[2];
	char tmpstring[20];

	cgidebug("pbx_send_do_spec_comcmd.\n");
	memset(tmpstring, '\0', strlen(tmpstring));
	
	if(slen>0&&slen<18)
	{

		for(i=0,v=0;i<slen/2;i++,v++)
		{
			tmpstring[i]=c2i(v2[v]);
			v++;
			tmpstring[i]=(tmpstring[i]<<4)|c2i(v2[v]);
		}
		
		if(1==(slen%2))	//odd number, the low 4 bit of last charator is 0x0f
		{
			tmpstring[i]=c2i(v2[v]);
			//			v++;
			tmpstring[i]=tmpstring[i]<<4|0x0f;
			tmpstring[i+1]=0;
		}
		else
			tmpstring[i]=0;
		
		pbx_send_spec_comcmd(cmd, v1, tmpstring);
	}
	else
	{
		cgidebug("Phone number unrecongnized!\n");
		return -1;
	}
	
	return 1;
}


int pbx_send_spec_comcmd(unsigned char cmd,char *v1,char *v2)
{
	int i=0;
	//compare to pbx_send_comcmd: do three special command
	//flowing should be replaced by com data IP_PBX_CMD
	PBX_COMMAND cmdbuf;
	unsigned char *p,*pbuf;

	cmdbuf.op=cmd;
	cmdbuf.id= cmd_seq;
	pbuf=cmdbuf.value;
	if(v1)
	{
		*pbuf++=*v1-'0';
	}
	if(v2)
	{
		p=v2;
		while(*p)
		{
			*pbuf++=*p++;
		}
	}
	*pbuf=0xff;

	if(-1==as_cmd_apps_to_uart(&cmdbuf, AS_MSG_TYPE_CGI))
	{
		return -1;
	}
		
	for(i=0;v2[i];i++)
	{
		cgidebug("%02x",(unsigned char)v2[i]);
	}
	cgidebug(" to phone %s \n",v1);
	
	if(cmd_seq==254)
		cmd_seq=0;
	else 
		cmd_seq++;
	
	return 0;
}

int pbx_send_teshu_comcmd(unsigned char cmd,char *v1,char *v2)
{
	PBX_COMMAND cmdbuf;
	unsigned char *p,*pbuf,*pbufout;
	
	cmdbuf.op=cmd;
	cmdbuf.id= cmd_seq;//id;
	pbuf=cmdbuf.value;
	pbufout=pbuf+LEN_CMD-2;
	if(v1)
	{
		*pbuf++=*v1-'0';
	}
	
	if(v2)
	{
		p=v2;
		while(*p)
		{
			if(pbuf==pbufout)
				return -1;
			if(isdigit(*p))
				*pbuf++=*p++-'0';
			else if(isupper(*p))
				*pbuf++=*p++-'A';
			else if(islower(*p))
				*pbuf++=*p++-'a';
			else 
				return -1;
			
			*(pbuf-1) = (*(pbuf-1)<<4)|0x0f;
		}
	}
	
	if(pbuf==pbufout)
		return -1;
	*pbuf=0xff;
	
	//if(-1==cgi_pbx_cmd(&cmdbuf))
	if(-1==as_cmd_apps_to_uart(&cmdbuf, AS_MSG_TYPE_CGI ))
	{
		return -1;
	}

	if(cmd_seq==254)
		cmd_seq=0;
	else 
		cmd_seq++;
	
	return 0;
}

int pbx_send_digit_comcmd(unsigned char cmd,char * v1,char * v2)
{
	int num=0;
	//flowing should be replaced by com data IP_PBX_CMD
	PBX_COMMAND cmdbuf;
	unsigned char *pbuf,*pbufout;

	cmdbuf.op=cmd;
	cmdbuf.id= cmd_seq;
	pbuf=cmdbuf.value;
	pbufout=pbuf+LEN_CMD-2;
	if(v1)
	{
		*pbuf++=*v1-'0';
	}
	num=atoi(v2);
	//sprintf(buf,"%x",num);
	//len=strlen(buf);
	*pbuf++=(unsigned char )num;
	#if 0
	while ( (buf[i]!='\0') && (i<20 ) )
	{
		if(pbuf==pbufout)return -1;
		if(isdigit(buf[i]))
			buf[i]=buf[i]-'0';
		else if(isupper(buf[i]))
			buf[i]=buf[i]-55;
		else if(islower(buf[i]))
			buf[i]=buf[i]-87;
		else return -1;
		 i++;
	}
	for(i=0; i<len;i=i+2)
	{
		*pbuf=buf[i]<<4;
		*pbuf=*pbuf & buf[i+1];
		pbuf++;
	}
	#endif
	
	if(pbuf==pbufout)
		return -1;
	*pbuf=0xff;
	
	if(-1==as_cmd_apps_to_uart(&cmdbuf, AS_MSG_TYPE_CGI ))
	{
		return -1;
	}
	
	if(cmd_seq==254)
		cmd_seq=0;
	else 
		cmd_seq++;
	
	return 0;
}

int pbx_send_comcmd(unsigned char cmd,char *v1,char *v2)
{
	//flowing should be replaced by com data IP_PBX_CMD
	PBX_COMMAND cmdbuf;
	unsigned char *p,*pbuf,*pbufout;
	
	cmdbuf.op=cmd;
	cmdbuf.id= cmd_seq;//id;
	
	pbuf = cmdbuf.value;
	pbufout = pbuf + LEN_CMD-2;
	
	if(v1)
	{
		*pbuf++=*v1-'0';
	}
	
	if(v2)
	{
		p=v2;
		while(*p)
		{
			if(pbuf==pbufout)
				return -1;
			if(isdigit(*p))
				*pbuf++=*p++-'0';
			else if(isupper(*p))
				*pbuf++=*p++-'A';
			else if(islower(*p))
				*pbuf++=*p++-'a';
			else 
				return -1;
		}
	}
	
	if(pbuf==pbufout)
		return -1;
	
	*pbuf=0xff;
	
	if(as_cmd_apps_to_uart(&cmdbuf, AS_MSG_TYPE_CGI)< 0)
	{
		AS_MSG_LOG(AS_LOG_ERR, "write to UART message queue failed");
		return -1;
	}

	if(cmd_seq==254)
		cmd_seq=0;
	else 
		cmd_seq++;
	
	return 0;
}


