/*
* $Id: pbx_caller_id.c,v 1.12 2007/07/25 17:50:07 lizhijie Exp $
*/

#pragma	ot(9,speed)
#pragma small

#include <REGSTCRD+.H>
#include "function_declarations.h"

#define	PBX_ENCODE_CHANNEL_BUSY( i)	\
		PBX_CLEAR_FLAGS(dtmfCodingChannelsFree, i);

#define	PBX_ENCODE_CHANNEL_FREE( i)	\
		PBX_SET_FLAGS(dtmfCodingChannelsFree, i);


void	callerid_play_start(INT8U extLineIndex)
{
	INT8U data column, index;

	index = callerid_find_channel(extLineIndex);
	if(index==PBX_VALUE_INVALIDATE)
		return;

	column = pbx_find_free_column();
	if(column == PBX_VALUE_INVALIDATE)
	{
//		pbx_if_start_music(extLineIndex, TRUE);
		return;
	}

	PBX_COLUMN_BUSY(column);
	PBX_ENCODE_CHANNEL_BUSY(index);

	encodingChannels[index].colChannel = column;
	if(IS_EXT_DEVICE(extLineIndex) )
	{
		encodingChannels[index].timeout = 5*PBX_100MS;
	}
	else if(IS_PSTN_LINE_DEVICE(extLineIndex) )
	{
		encodingChannels[index].timeout = 15*PBX_100MS;
	}

	encodingChannels[index].point = 0;
}

/* i : index of encodingChannels */
void	callerid_play_step(INT8U index)	
{
	INT8U	data digit;
	BOOL	flag;
	
	flag=(BOOL)(index&0x01);

	if(flag^callerIdSendFlags)
	{
		if(IS_ODD(encodingChannels[index].point) )
			digit = encodingChannels[index].digits.value[(encodingChannels[index].point)/2]&0x0f;
		else
			digit = encodingChannels[index].digits.value[(encodingChannels[index].point)/2] >>4 ;

		encodingChannels[index].point++;
		
		CONNECT(encodingChannels[index].owner, encodingChannels[index].colChannel);
		if(index<2)
		{
			CONNECT(CHANNEL_ROW_DTMF_ENCODE_1, encodingChannels[index].colChannel );
			/* digit in the low nibble(4 bits) in byte */
//			DTMF_ENCODE_FIRST_ENABLE(digit);
			dtfm_encoder_digit(0, digit);
		}
		else
		{
			CONNECT(CHANNEL_ROW_DTMF_ENCODE_2, encodingChannels[index].colChannel);
//			DTMF_ENCODE_SECOND_ENABLE(digit);
			dtfm_encoder_digit(1, digit);
		}
		
		encodingChannels[index].timeout = PBX_100MS;
	}
	else	
	{
		if(index<2)
		{
//			DTMF_ENCODE_FIRST_DISABLE();
			DTMT_ENCODER0_DISBALE();
			DISCONNECT(CHANNEL_ROW_DTMF_ENCODE_1, encodingChannels[index].colChannel);
		}
		else
		{
//			DTMF_ENCODE_SECOND_DISABLE();
			DTMT_ENCODER1_DISBALE();
			DISCONNECT(CHANNEL_ROW_DTMF_ENCODE_2, encodingChannels[index].colChannel);
		}
		
		DISCONNECT(encodingChannels[index].owner, encodingChannels[index].colChannel);

		if( IS_PSTN_LINE_DEVICE(encodingChannels[index].owner) )//&& encodingChannels[i].point == 2)
			encodingChannels[index].timeout = PBX_100MS;/* must be odd number. tune this param for different PSTN requirement */
		else
			encodingChannels[index].timeout = PBX_100MS;
		
		if( encodingChannels[index].point == encodingChannels[index].digits.length )
		{
			encodingChannels[index].digits.length = 0;
			encodingChannels[index].point = 0;
			
			PBX_COLUMN_FREE(encodingChannels[index].colChannel);
			
			if(IS_PSTN_LINE_DEVICE(encodingChannels[index].owner) )
			{/* notify statemachine connect Caller Device to this PSTN callee device */
				DEVICE_EVENT_EMIT(encodingChannels[index].owner, PBX_EVENT_OFFHOOK);
			}

			/* for safe, encodingChannel is also freed explicitly */
			encodingChannels[index].owner = PBX_VALUE_INVALIDATE;
			encodingChannels[index].timeout = 0;
			encodingChannels[index].colChannel = PBX_VALUE_INVALIDATE;

#if 0
			if(encodingChannels[i].destChannel >= PBX_FXO_COUNT )
				shouhao_over(encodingChannels[i].destChannel );
#endif			
		}
	}
}

INT8U	callerid_allocate_channel(INT8U extLineIndex, INT8U xdata *digitsValue, INT8U length )
{
	INT8U	data		i;
	
	for(i=0; i<ENCODE_CHANNEL_NUMBER; i++)
	{
		if(encodingChannels[i].owner ==  PBX_VALUE_INVALIDATE)
		{
			encodingChannels[i].owner = extLineIndex;
			encodingChannels[i].colChannel = PBX_VALUE_INVALIDATE;

			encodingChannels[i].timeout = 0;
			encodingChannels[i].point = 0;

			COPY_DIGITS_BUFFER( (&(encodingChannels[i].digits)), digitsValue, length );
			return i;
		}
	}

	return PBX_VALUE_INVALIDATE;
}

INT8U	callerid_find_channel(INT8U extLineIndex)
{
	INT8U	data		i;
	
	for(i=0; i<ENCODE_CHANNEL_NUMBER; i++)
	{
		if(encodingChannels[i].owner ==  extLineIndex)
		{
			return i;
		}
	}

	return PBX_VALUE_INVALIDATE;
}

void	callerid_free_channel(INT8U extLineIndex)
{
	INT8U	data		i;
	i = callerid_find_channel(extLineIndex);
	if(i != PBX_VALUE_INVALIDATE)
	{
		encodingChannels[i].owner = PBX_VALUE_INVALIDATE;
//		encodingChannels[i].timeout = 0;
	}
}

