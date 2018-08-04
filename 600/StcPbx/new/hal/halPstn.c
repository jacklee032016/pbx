/*
* $Id: halPstn.c,v 1.7 2007/09/03 21:09:07 lizhijie Exp $
*/

#pragma	ot(9,speed)
#pragma small

#include <REGSTCRD+.H>
#include "declarations.h"


void	pstn_callerid_end(INT8U pstnIndex)
{
	if(!IS_PSTN_LINE_DEVICE(pstnIndex) )
		return;

	if(fskDecoder.currentChannel == PBX_VALUE_INVALIDATE)
		return;
	
//	PBX_CLEAR_FLAGS(pstnDevices.fskCtrl, fskDecoder.currentChannel);
//	PSTN_CTRL_DECODER();

#if WITH_UART_DEBUG_PSTN_FSK_DATA
//	pbx_uart_debug(d, ((fskDecoder.fskIndex+1)>13)?13:(fskDecoder.fskIndex+1));

	uartTxCmds[uart.responseInIndex].value[0] = totalD;
	uartTxCmds[uart.responseInIndex].value[1] = fskDecoder.fskIndex;
	uartTxCmds[uart.responseInIndex].value[2] = d[0];
	uartTxCmds[uart.responseInIndex].value[3] = d[1];
	uartTxCmds[uart.responseInIndex].value[4] = d[2];
	uart_tx_cmd(CMD_UART_DEBUG, 5);
#endif
	fskDecoder.currentChannel = PBX_VALUE_INVALIDATE;
}


/* ringing ext in DID mode, called in IDLE and TRYING state of DID */
BOOL pstnDidRinging(INT8U devIndex)
{
	INT8U data groupId, i, pos;
	BOOL isfound = FALSE;
	
	if(!IS_PSTN_LINE_DEVICE(devIndex) )
		return FALSE;

	pos = CALL_MY_PEER(devIndex);
//	if(!IS_EXT_DEVICE(pos) )
//		return;
	if(pos == PBX_VALUE_INVALIDATE)
		pos = 0;
	else
		pos = (pos+1)%PBX_FXS_COUNT;
	
	if(PSTN_GROUP_CALL_IS_ON() )
	{
		groupId = PSTN_MY_GROUP_ID(devIndex);
		for(i= pos; i< PBX_FXS_COUNT; i++)
		{
			if(((extConfigs.groups[i]&0x7F) == groupId) && (CALL_CHECK_STATUS(i, PBX_STATE_IDLE)))
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
				if(( (extConfigs.groups[i]&0x7F) == groupId) && (CALL_CHECK_STATUS(i,PBX_STATE_IDLE) ))
				{
					pos = i;
					break;
				}
			}
		}

		if(pos != CALL_MY_PEER(devIndex) )
		{
			goto sucessReturn;
		}
	}

	if(PSTN_IS_DIRECT_MODE() )
	{
		pos = PSTN_MY_VIRTUAL_CONSOLE(devIndex);
		if( ( CALL_MY_PEER(devIndex) !=  pos) && 
			(CALL_CHECK_STATUS(pos,PBX_STATE_IDLE)) )
		{
			goto sucessReturn;
		}

	}

	return FALSE;
	
sucessReturn:		
	
	if( CALL_MY_PEER(devIndex) != PBX_VALUE_INVALIDATE)
	{
		CALL_PEER_EVENT(devIndex, PBX_SIGNAL_CANCEL);
		CALL_MY_PEER(CALL_MY_PEER(devIndex)) = PBX_VALUE_INVALIDATE;
	}
	
	CALL_MY_PEER(devIndex ) = pos;
	CALL_MY_PEER(pos) = devIndex;
	
	CALL_EVENT_EMIT(pos, PBX_SIGNAL_RINGING);
	return TRUE;
}

INT8U CallerIdChannelFind(INT8U devIndex)	
{
	INT8U data	i;

	for(i=0; i< sizeof(callerIdData)/sizeof(decode_channel_t); i++)
	{
		if(callerIdData[i].owner == devIndex )
		{
			return i;
		}
	}
	
	return	PBX_VALUE_INVALIDATE;
}

INT8U CallerIdChannelAllocate(INT8U devIndex)	
{
	INT8U	data i;

	i = CallerIdChannelFind(devIndex);
	if( i != PBX_VALUE_INVALIDATE)
		return i;

	for(i=0; i< sizeof(callerIdData)/sizeof(decode_channel_t); i++)
	{
		if(callerIdData[i].owner == PBX_VALUE_INVALIDATE)
		{
			callerIdData[i].owner = devIndex;
			callerIdData[i].digits.length = 0;

			if(IS_PSTN_LINE_DEVICE(devIndex) )
			{
				fskDecoder.currentChannel = i;
				fskDecoder.length = 0;
				fskDecoder.fskIndex = 0;
				
				fskWaitStopFlag = FALSE;
				fskFirstByteFlag = TRUE;

#if WITH_UART_DEBUG_PSTN_RINGER
			//	uart_debug_3_int_value(UART_DEBUG_PSTN_RINGER, GET_PSTN_NUMBER(pstnIndex), PSTN_FLOW_BEGIN_CALLER_ID, fskDecoder.currentChannel );
				totalD = 0;
#endif

				PBX_SET_FLAGS(pstnDevices.fskCtrl, GET_PSTN_NUMBER(devIndex) );
			//	PBX_SET_FLAGS(pstnDevices.fskCtrl, 0x0F);
				pstnDevices.fskCtrl = pstnDevices.fskCtrl|0x0F;
				PSTN_CTRL_DECODER();
				MT88E39_DCLK = FALSE;

			}
			return i;
		}
	}
	
	return	PBX_VALUE_INVALIDATE;
}


void	CallerIdChannelFree(INT8U devIndex)
{
	INT8U	data k = CallerIdChannelFind(devIndex);
	
	if( k== PBX_VALUE_INVALIDATE )
		return;
	
	callerIdData[k].digits.length = 0;
	callerIdData[k].owner = PBX_VALUE_INVALIDATE;
 }


