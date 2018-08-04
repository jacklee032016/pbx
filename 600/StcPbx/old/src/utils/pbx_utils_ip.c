/*
* $Id: pbx_utils_ip.c,v 1.1 2007/05/11 20:13:52 lizhijie Exp $
*/


/* when Callee EXT's left transfer is IP */
void	left_transfer_to_ip(INT8U devIndex/* caller*/, INT8U y/* callee which is left now*/)
{
	INT8U	data ipChannel;
	
	ipChannel = pbx_tx_call_offhook(devIndex, phoneLeftTransfers[y]);
	if(ipChannel==0xff)
		return;

	ipChannel = ipChannel+ STATE_IP_OFFSET;
	pbx_if_stop_ringing(y);	/* stop callee ringing */
	stop_qudian(y);	/* free channale allocated to callee */
	PBX_ROW_FREE(y);
	status[y][0]=19;
	
	if(status[devIndex][1]==y)
		status[devIndex][1]= ipChannel;
	else
		status[devIndex][2]= ipChannel;

	status[ipChannel][0] = STATE_IP_SEND_PHONENUMBER;//95;
	status[ipChannel][1] = devIndex;
	
	bell_reg[devIndex]=0;		
}

/* 话机的转移目标是IP处理*/
bit	tel_target_ip(INT8U x,INT8U xdata*y)
{
	INT8U data i;
	i = pbx_tx_call_offhook(x,y);
	if(i ==0xff)
		return 0

	status[x][0]=77;
	status[x][1]=i+12;
	status[i+12][0]= STATE_IP_SEND_PHONENUMBER;
	status[i+12][1]= x;

	decodingchannel_free(x);
	return 1;
}

//直拨时中继的转移目标是IP处理
bit	track_target_ip(INT8U x,INT8U xdata*y)
{
	INT8U data i;
	i= pbx_tx_call_offhook(x,y);
	
	if(i==0xff)
		return 0;

	status[x][0]=29;
	status[x][1]=i+12;
	status[i+12][0]=STATE_IP_SEND_PHONENUMBER;//95;
	status[i+12][1]=x;
	decodingchannel_free(x);
	pbx_if_ringback_tone_play(x);
	return 1;
}

//IP的转移目标是IP处理
bit	ip_target_ip(INT8U x,INT8U xdata*y)
{
	INT8U data i;
	i= pbx_tx_call_offhook(x,y);

	if(i==0xff)
		return 0;
	
	status[x][0]=91;
	status[x][1]=i+12;
	status[i+12][0]= STATE_IP_SEND_PHONENUMBER;//95;
	status[i+12][1]=x;
	decodingchannel_free(x);
	pbx_if_ringback_tone_play(x);
	
	return 1;
}

/*虚拟总机方式时中继的转移目标是IP处理*/
bit	track_target_ipx(INT8U x,INT8U xdata*y)
{
	INT8U data i;
	i= pbx_tx_call_offhook(x,y);
	if(i == 0xff)
		return 0;
	
	status[x][0]=44;
	status[x][1]=i+12;
	status[i+12][0]= STATE_IP_SEND_PHONENUMBER;//95;
	status[i+12][1]=x;
	decodingchannel_free(x);
	tel_jishi[x]=1;
	return 1;
}

void	send_guaji(INT8U x)//向主机发送挂机信号,释放IP
{
	INT8U data i;
	if((x<STATE_IP_OFFSET) || (x>=20))
		return;
	
	i=found_send_buf();
	if(i!=0xff)
	{
		if(++uartCmdSeq==0xff)
			uartCmdSeq++;
		uartTxCmds[i][0]=CMD_CALL_PBX_IP_ONHOOK;//0x93;
		uartTxCmds[i][1]=uartCmdSeq;
		uartTxCmds[i][2]=x-STATE_IP_OFFSET;
		uartTxCmds[i][3]=peerOfIP[x-STATE_IP_OFFSET];
		uartTxCmds[i][4]=0xff;
		busy_send_buf(i);
		PBX_COLUMN_FREE_IP(x);
		status[x][0]=STATE_IP_IDLE;//86;
	}
	else
	{
		status[x][0]=STATE_IP_SEND_ONHOOK_2;//108;
		tel_jishi[x]=1;
	}
}

void	send_guajia(INT8U x)//发送挂机信号
{
	INT8U data i;
	i=found_send_buf();
	if(i!=0xff)
	{
		if(++uartCmdSeq==0xff)
			uartCmdSeq++;
		uartTxCmds[i][0]= CMD_CALL_PBX_IP_ONHOOK;//0x93;
		uartTxCmds[i][1]= uartCmdSeq;
		uartTxCmds[i][2]= x-STATE_IP_OFFSET;
		uartTxCmds[i][3]= peerOfIP[x-STATE_IP_OFFSET];
		uartTxCmds[i][4]= 0xff;
		busy_send_buf(i);
		status[x][0]= STATE_IP_IDLE;//86;
	}
	else
	{
		status[x][0]=STATE_IP_SEND_ONHOOK;//112;
		tel_jishi[x]=1;
	}
}


/* 断开IP*/
INT8U cut_ip()
{
	INT8U data i,j,n=0xff;
	
	for(i=STATE_IP_OFFSET;i<20;i++)
	{
		if(status[i][0] != STATE_IP_IDLE)//86)
		{
			if(status[i][0]==95||status[i][0]==102||
				status[i][0]==103||status[i][0]==104 )
			{
				ip_pbx_play_busytone(i);
			}	
			else if(status[i][0] == STATE_IP_RINGING)//96)
			{
				PBX_COLUMN_FREE_IP(i);
				j= status[i][1];
				
				if(status[j][0]==44)
				{
					status[j][0]= 26;
					zjld_reg[i][0]=0;
				}
				else
				{
					pbx_if_ringback_tone_stop(j);
					gjclb(j);
				}
			}
			else if(status[i][0]==105)
			{
				PBX_COLUMN_FREE_IP(i);
				j=status[i][1];
				pbx_if_ringback_tone_stop(j);
				pbx_if_busytone_play(j);
				status[j][0]=65;
			}
			else if(status[i][0]==108)
			{
				PBX_COLUMN_FREE_IP(i);
				status[i][0]=112;
			}
			else
			{
				ext_on_off_hook_process(i);
			}
			
			send_guajia(i);
			n=i-12;
			break;
		}
	}
	return	n;
}

