/*
* $Id: pbx_utils_qd.c,v 1.1 2007/05/11 20:13:52 lizhijie Exp $
*/


bit	start_fahao(INT8U x,INT8U xdata *y)//开始向中继发送号码
{
	INT8U data i,j,k;
	
	if(columnFrees&&(dtmfCodingChannelsFree&0x0f))
	{
		if(dtmfCodingChanneFree_1)
			i=0;
		else if(dtmfCodingChanneFree_2)
			i=1;
		else if(dtmfCodingChanneFree_3)
			i=2;
		else
			i=3;
		
		j=pbx_find_free_column();
		PBX_COLUMN_BUSY(j);
		dtmfCodingChannelsFree=dtmfCodingChannelsFree&~(0x01<<i);
		
		encodingChannels[i].destChannel = x;
		encodingChannels[i].encodingChannel = j;
		encodingChannels[i].length = y[0];	/*号码个数*/
		encodingChannels[i].point = 1;		/*号码指针*/
		
		k=y[0];
		k=++k/2+2;
		
		for(j=2;j<k;j++)
		{
			x=y[j]&0xf0;
			if(x==0)
				x=0xa0;
			encodingChannels[i].digits[j-2]=x;
			x=y[j]&0x0f;
			if(x==0)
				x=0x0a;
			
			encodingChannels[i].digits[j-2]=encodingChannels[i].digits[j-2]|x;
		}
		
		qudian_jishi[i]=10;//计时1秒
		return 1;
	}
	else
		return 0;
}

BOOL start_fahao_ip(INT8U x,INT8U xdata *y)//开始向中继发送号码///repaired by cz 8.30 解决上位机IP多发一个0问题
{
	INT8U data i,j,k;
	
	if((columnFrees&&(dtmfCodingChannelsFree&0x0f))==0 )
		return FALSE;
	
	if(dtmfCodingChanneFree_1)
		i=0;
	else if(dtmfCodingChanneFree_2)
		i=1;
	else if(dtmfCodingChanneFree_3)
		i=2;
	else
		i=3;
	
	j=pbx_find_free_column();
	PBX_COLUMN_BUSY(j);
	dtmfCodingChannelsFree=dtmfCodingChannelsFree&~(0x01<<i);
	encodingChannels[i].destChannel = x;
	encodingChannels[i].encodingChannel = j;
	encodingChannels[i].length = y[0];	/*号码个数*/
	encodingChannels[i].point = 1;		/*号码指针*/
	
	k=y[0];
	k=++k/2+2;
	
	for(j=2;j<k;j++)
	{
		/*x=y[j]&0xf0;
		if(x==0)
			x=0xa0;
		qudian[i][j+2]=x;
		x=y[j]&0x0f;
		if(x==0)
			x=0x0a;
		qudian[i][j+2]=qudian[i][j+2]|x;*///060622
		if(j!=2)
		{
			x=y[j]&0xf0;
			if(x==0)
				x= DIAL_DIGIT_ZERO;
			qudian[i][j+1]|=x>>4;
		}
		
		x=y[j]&0x0f;
		if(x==0)
			x=0x0a;
		encodingChannels[i].digits[j-2] = (x<<4);
	}
	qudian_jishi[i]=10;//计时1秒
	
	return TRUE;
}


/* 中继收号结束处理*/
void	shouhao_over(INT8U i /* line Index */)	//中继收号结束
{
	INT8U data j,n;
	j= status[i][1];
	//pbx_voice_channel_connect(i,j);			repaired by cz 9.28	离位转移外线转外线
	
	if(status[j][0]==44)
	{
		pbx_line_offhook(j);
		pbx_voice_channel_connect(i,j);		//repaired by cz 9.28	离位转移外线转外线
		status[j][0]=31;
		status[i][0]=32;
	}
	else if(status[j][0]==29)
	{
		pbx_if_ringback_tone_stop(j);
		pbx_if_ringback_tone_stop(i);		//repaired by cz 9.28	离位转移外线转外线
		pbx_voice_channel_connect(i,j);		//repaired by cz 9.28	离位转移外线转外线
		pbx_if_start_music(j);
		status[j][0]=31;
		status[i][0]=32;
	}
	else if(status[j][0]== STATE_IP_PLAY_RINGBACK)//91)
	{
		pbx_if_ringback_tone_stop(j);		
		pbx_voice_channel_connect(i,j);		//repaired by cz 9.28	离位转移外线转外线
		status[j][0]= STATE_IP_CONNECTED;//92;
		status[i][0]=32;
		pbx_tx_call_offhook_as_callee( j -STATE_IP_OFFSET);
	}
	else
	{
		pbx_if_ringback_tone_stop(j);		
		pbx_voice_channel_connect(i,j);		//repaired by cz 9.28	离位转移外线转外线
		lode_thxs(j,i);
		status[j][0]=07;
		status[i][0]=32;
	}
}

void	stop_fahao(INT8U lineIndex)//停止向中继发号
{
	INT8U data i,j;
	
	for(i=0; i<4; i++)
	{
		if(encodingChannels[i].destChannel == lineIndex)
		{
			encodingChannels[i][0] = 0xff;
			j = encodingChannels[i].encodingChannel;
			PBX_COLUMN_FREE(j);
			DISCONNECT(lineIndex, j);
			
			if(i<2)
				DISCONNECT(CHANNEL_ROW_DTMF_ENCODE_1, j);
			else
				DISCONNECT(CHANNEL_ROW_DTMF_ENCODE_2, j);
			
			dtmfCodingChannelsFree = dtmfCodingChannelsFree|(0x01<<i);
			break;
		}
	}
}

