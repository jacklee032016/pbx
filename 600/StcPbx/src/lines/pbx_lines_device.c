/*
* $Id: pbx_lines_device.c,v 1.13 2007/07/21 16:08:40 lizhijie Exp $
* PSTN Line device operations
*/

#pragma	ot(9,speed)
#pragma small

#include <REGSTCRD+.H>
#include "function_declarations.h"


/* only row of PSTN line channel are used as dynamic(refer to pbx_find_free_row() ), 
* so change the row only PSTN line offhook */

void	pbx_row_changed(INT8U x)
{
#if 1
	return ;
#else
	INT8U data i, peer,k,rec_bsm;/*rec_bsm 接受回铃或音乐的行或列*/
	BOOL	flag=0;

	if( deviceStatus[x].phyPeerId == PBX_VALUE_INVALIDATE)
	{
		return;
	}

	peer = deviceStatus[x].phyPeerId;
	DISCONNECT(x, peer);
	rec_bsm = x;

	deviceStatus[x].phyPeerId = PBX_VALUE_INVALIDATE;
	i = x;
#if 0	
	for(i++;i<PBX_DEVICE_COUNT;i++)
	{
		k= deviceStatus[i].phyPeerId;
		if(k == x)
		{
			k=point[i]&0x0f;
			point[i] = PBX_VALUE_INVALIDATE;
			DISCONNECT(x,k);
			break;
		}
	}
	
	if (i<PBX_DEVICE_COUNT )
	{
		i = pbx_find_free_row();
		if(i!= PBX_VALUE_INVALIDATE)
		{
			PBX_ROW_BUSY(i);
			CONNECT(i,j);
			CONNECT(i,k);
			point[j+12]=(i<<4)|j;
			point[k+12]=(i<<4)|k;
		}
	}//end if (i==20)
	else	// corresponding if(i==20)  送回铃或音乐时
	{
		if (rec_bsm<12)
		{
			DISCONNECT(rec_bsm,j);
			PBX_COLUMN_FREE(j);
			if (PBX_CHECK_FLAGS(pbxHwStatus.ringbackStatus , x) )
			{
				CONNECT(rec_bsm, CHANNEL_COL_RINGBACK_TONE);
				extension_ringback_tone_play(x, FALSE);
			}
			else
			{
				CONNECT(rec_bsm, CHANNEL_COL_MUSIC);
				pbx_if_music_play(x, FALSE);
			}
		}
		else
		{
			if( PBX_CHECK_FLAGS(pbxHwStatus.ringbackStatus, x) )
			{
				CONNECT(CHANNEL_ROW_IP_R_1, rec_bsm-12);
				extension_ringback_tone_play(x, FALSE);
			}
			else
			{
				CONNECT(CHANNEL_ROW_IP_R_2, rec_bsm-12);
				pbx_if_music_play(x, FALSE);
			}
		}
	}
#endif

#endif	
}

/* return newly allocated line Index or 0xFF is failed, x is source device */
INT8U pbx_line_allocate_channel(void )
{
	INT8U data y, lineIndex,j;
	
	y = PBX_GET_LOW_NIBBLE(pbxHwStatus.trackFreeStatus& (baseConfigs.trunkStatusFlags) );
	if(y == 0)
		return PBX_VALUE_INVALIDATE;
	
	lineIndex = PBX_FXS_COUNT;
	j = 0x01;

	while((y&j)==0)
	{
		lineIndex ++;
		j=j<<1;
	}
	
	PSTN_SET_BUSY(lineIndex);
	pbx_row_changed(lineIndex);
	PBX_ROW_BUSY(lineIndex);
	
	return lineIndex;
}

void	pbx_line_offhook(INT8U devIndex)
{
	PBX_SET_FLAGS(pbxHwStatus.pstnRelayCtrl, GET_PSTN_NUMBER(devIndex));
	PSTN_CTRL_HOOK();

	pstn_callerid_free(devIndex);
}

void	pbx_line_onhook(INT8U devIndex)
{
	PBX_CLEAR_FLAGS(pbxHwStatus.pstnRelayCtrl, GET_PSTN_NUMBER(devIndex) );
	PSTN_CTRL_HOOK();
}


/*  */
void pbx_line_free_channel(INT8U lineIndex )
{
//	PSTN_SET_FREE(lineIndex);
	
	PBX_ROW_FREE(lineIndex);
	
	pbx_line_onhook(lineIndex);
	deviceStatus[lineIndex].timeout = 5*PBX_100MS;	/* delay n* 100ms for next offhook check */
//	pbx_voice_channel_broken(lineIndex, deviceStatus[lineIndex].peerId );
}

void	pstn_callerid_begin(INT8U pstnIndex)
{
	if(!IS_PSTN_LINE_DEVICE(pstnIndex))
		return;
	fskDecoder.currentChannel = GET_PSTN_NUMBER(pstnIndex);
	fskDecoder.length = 0;
	fskDecoder.fskIndex = 0;
	
	fskWaitStopFlag = FALSE;
	fskFirstByteFlag = TRUE;

#if WITH_PSTN_FLOW_DEBUG
	uart_tx_2_int_value(CMD_UART_DEBUG, PSTN_FLOW_BEGIN_CALLER_ID, fskDecoder.currentChannel );
#endif
	callerIdData[fskDecoder.currentChannel].length = 0;

	PBX_SET_FLAGS(pbxHwStatus.pstnFskCtrl, fskDecoder.currentChannel);
	PSTN_CTRL_DECODER();

//	busucomcome = busucomcome|0x0F;//(SET_BIT(1, currentCallerIdChannel));
//	send_busycome(busucomcome);

	MT88E39_DCLK = FALSE;
}


//断开中继到来电接收通道
void	pstn_callerid_end(INT8U pstnIndex)
{
	if(!IS_PSTN_LINE_DEVICE(pstnIndex) )
		return;
	
//	send_busycome(busucomcome);
	PBX_CLEAR_FLAGS(pbxHwStatus.pstnFskCtrl, fskDecoder.currentChannel);
	PSTN_CTRL_DECODER();

#if WITH_PSTN_FSK_DATA_DEBUG
	pbx_uart_debug(d, (fskDecoder.fskIndex>13)?13:fskDecoder.fskIndex);
#endif
	fskDecoder.currentChannel = PBX_VALUE_INVALIDATE;
}

void	pstn_callerid_free(INT8U pstnIndex)	
{
	if(!IS_PSTN_LINE_DEVICE(pstnIndex) )
		return;
	
	callerIdData[GET_PSTN_NUMBER(pstnIndex)].length = 0;
}

#if 0
BOOL isPstnDid(INT8U devIndex)
{
	INT8U data extIndex;

	if(PSTN_GROUP_CALL_IS_ON() )
		return TRUE;
	if(PSTN_IS_DIRECT_MODE() )
		return FALSE;
	
	if(!IS_PSTN_LINE_DEVICE(devIndex) )
	{
		return FALSE;
	}

	extIndex = PSTN_MY_VIRTUAL_CONSOLE(devIndex);
	if(!IS_EXT_DEVICE(extIndex) )
	{
		return FALSE;
	}

	if(deviceStatus[extIndex].status == STATE_IDLE)
		return TRUE;

	return FALSE;
}
#endif

/* ringing ext in DID mode, called in IDLE and TRYING state of DID */
BOOL pstnDidRinging(INT8U devIndex)
{
	INT8U data groupId, i, pos;
	BOOL isfound = FALSE;
	
	if(!IS_PSTN_LINE_DEVICE(devIndex) )
		return FALSE;

	pos = deviceStatus[devIndex].peerId;
//	if(!IS_EXT_DEVICE(pos) )
//		return;
	if(pos == PBX_VALUE_INVALIDATE)
		pos = 0;
	
	if(PSTN_GROUP_CALL_IS_ON() )
	{
		groupId = PSTN_MY_GROUP_ID(devIndex);
		for(i= pos; i< PBX_FXS_COUNT; i++)
		{
			if(((extConfigs.groups[i]&0x7F) == groupId) && (deviceStatus[i].status == STATE_IDLE))
			{
				pos = i;
				isfound = TRUE;
				break;
			}
		}

		if(isfound == FALSE)
		{
			for(i= 0; i < pos; i++)
			{
				if(( (extConfigs.groups[i]&0x7F) == groupId) && (deviceStatus[i].status == STATE_IDLE))
				{
					pos = i;
					break;
				}
			}
		}

		if(pos != deviceStatus[devIndex].peerId)
		{
			goto sucessReturn;
		}
	}

	if(PSTN_IS_CONSOLE_MODE() )
	{
		pos = PSTN_MY_VIRTUAL_CONSOLE(devIndex);
		if( (deviceStatus[devIndex].peerId !=  pos) && 
			(deviceStatus[pos].status == STATE_IDLE) )
		{
			goto sucessReturn;
		}

	}

	return FALSE;
	
sucessReturn:		
	
	if(deviceStatus[devIndex].peerId != PBX_VALUE_INVALIDATE)
	{
		DEVICE_EVENT_EMIT(deviceStatus[devIndex].peerId, PBX_SIGNAL_CANCEL);
	}
	
	deviceStatus[deviceStatus[devIndex].peerId].peerId = PBX_VALUE_INVALIDATE;
	deviceStatus[pos].peerId = devIndex;
	deviceStatus[devIndex].peerId = pos;

	if(callerIdData[GET_PSTN_NUMBER(devIndex)].length > 0)
	{
		callerid_allocate_channel(pos, callerIdData[GET_PSTN_NUMBER(devIndex)].value, callerIdData[GET_PSTN_NUMBER(devIndex)].length );
	}
	
	DEVICE_EVENT_EMIT(pos, PBX_SIGNAL_RINGING);

	return TRUE;

}

