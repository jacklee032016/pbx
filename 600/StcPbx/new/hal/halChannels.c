/*
* $Id: halChannels.c,v 1.7 2007/09/08 18:56:18 lizhijie Exp $
*/

#pragma	ot(9,speed)
#pragma small

#include <REGSTCRD+.H>
#include "declarations.h"


/* 8 DTMF decoder(LC7385), first 4 is connect to array, last 4 connecto EXT0~3 */
BOOL dtmfDecodingChannelAllocate(INT8U devIndex)	
{
	INT8U	data i;

	i = dtmfDecodingChannelFind(devIndex);
	if(i!= PBX_VALUE_INVALIDATE)
	{
		decodingChannels[i].digits.length = 0;
		return TRUE;
	}
	
	if(devIndex >= DECODE_CHANNEL_NUMBER )
	{
		for(i=0; i< DECODE_CHANNEL_NUMBER; i++)
		{
			if(decodingChannels[i].owner == PBX_VALUE_INVALIDATE)
			{
				decodingChannels[i].owner = devIndex;
				decodingChannels[i].digits.length = 0;
				
				CONNECT(devIndex, GET_DECODE_CHANNEL_INDEX(i) );
				return TRUE;
			}
		}
	}
	
	return	FALSE;
}


void	dtmfDecodingChannelFree(INT8U devIndex)
{
	INT8U	data k =dtmfDecodingChannelFind(devIndex);
	
	if( k== PBX_VALUE_INVALIDATE )
		return;
	
	decodingChannels[k].digits.length = 0;
	if( devIndex < DECODE_CHANNEL_NUMBER )
		return;

	decodingChannels[k].owner = PBX_VALUE_INVALIDATE;
	if( devIndex >= DECODE_CHANNEL_NUMBER)
	{
		DISCONNECT(devIndex, GET_DECODE_CHANNEL_INDEX(k) );
	}
 }

INT8U dtmfDecodingChannelFind(INT8U devIndex)	
{
	INT8U	i;
#if 0	
	if(devIndex<4)/*EXT 0-3 : always success */
	{
		return devIndex+4;
	}
#endif

//	for(i=0; i<sizeof(decodingChannels)/sizeof(decode_channel_t); i++)	
	for(i=0; i<2*DECODE_CHANNEL_NUMBER; i++)	
	{/* EXT 4~7 and 4 Lines */
		if(decodingChannels[i].owner == devIndex )
		{
			return i;
		}
	}
	
	return	PBX_VALUE_INVALIDATE;
}


void dtmfDecodingChannelAddDigit(INT8U index, INT8U digit)	
{
	INT8U	data		j;
	if(decodingChannels[index].digits.length >= (2*PHONE_DIGIT_BUFFER_SIZE) )
		return;

	j = decodingChannels[index].digits.length;
	
	if(! IS_ODD(j) )
	{/* odd */
		decodingChannels[index].digits.value[j/2]= SET_BIT(digit, 4);
	}
	else
	{/* even */
		decodingChannels[index].digits.value[j/2] = (decodingChannels[index].digits.value[j/2]&0xf0)|digit;
	}

	decodingChannels[index].digits.length++;
}

#define	PBX_ENCODE_CHANNEL_BUSY( i)	\
		PBX_CLEAR_FLAGS(dtmfCodingChannelsFree, i);

#define	PBX_ENCODE_CHANNEL_FREE( i)	\
		PBX_SET_FLAGS(dtmfCodingChannelsFree, i);


void	dtmfEncoderPlayStart(INT8U extLineIndex)
{
	INT8U data column, index;

	index = dtmfEncodeChannelFind(extLineIndex);
	if(index==PBX_VALUE_INVALIDATE)
		return;
	if( encodingChannels[index].point != PBX_VALUE_INVALIDATE)
		return;
	if(encodingChannels[index].digits.length == 0 )
	{/* free it */
		encodingChannels[index].owner = PBX_VALUE_INVALIDATE;
		return;
	}

	column = pbx_find_free_column();
	if(column == PBX_VALUE_INVALIDATE)
	{
		return;
	}

	PBX_COLUMN_BUSY(column);
	PBX_ENCODE_CHANNEL_BUSY(index);

	encodingChannels[index].colChannel = column;
	if(IS_EXT_DEVICE(extLineIndex) )
	{
		encodingChannels[index].timeout = 3*PBX_100MS;
	}
	else if(IS_PSTN_LINE_DEVICE(extLineIndex) )
	{
		encodingChannels[index].timeout = 7*PBX_100MS;
	}

	encodingChannels[index].point = 0;
}

/* i : index of encodingChannels */
void	dtmfEncoderPlayStep(INT8U index)	
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
			dtfm_encoder_digit(0, digit);
		}
		else
		{
			CONNECT(CHANNEL_ROW_DTMF_ENCODE_2, encodingChannels[index].colChannel);
			dtfm_encoder_digit(1, digit);
		}
		
		encodingChannels[index].timeout = PBX_100MS;
	}
	else	
	{
		if(index<2)
		{
			DTMT_ENCODER0_DISBALE();
			DISCONNECT(CHANNEL_ROW_DTMF_ENCODE_1, encodingChannels[index].colChannel);
		}
		else
		{
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
				CALL_EVENT_EMIT(encodingChannels[index].owner, PBX_EVENT_OFFHOOK);
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

INT8U	dtmfEncodeChannelAllocate(INT8U extLineIndex, INT8U xdata *digitsValue, INT8U length )
{
	INT8U	data		i;
	
	for(i=0; i<ENCODE_CHANNEL_NUMBER; i++)
	{
		if(encodingChannels[i].owner ==  PBX_VALUE_INVALIDATE)
		{
			encodingChannels[i].owner = extLineIndex;
			encodingChannels[i].colChannel = PBX_VALUE_INVALIDATE;

			encodingChannels[i].timeout = 0;
			encodingChannels[i].point = PBX_VALUE_INVALIDATE;

			COPY_DIGITS_BUFFER( (&(encodingChannels[i].digits)), digitsValue, length );
			return i;
		}
	}

	return PBX_VALUE_INVALIDATE;
}

INT8U	dtmfEncodeChannelFind(INT8U extLineIndex)
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

void	dtmfEncodeChannelFree(INT8U extLineIndex)
{
	INT8U	data		i;
	
	i = dtmfEncodeChannelFind(extLineIndex);
	if(i != PBX_VALUE_INVALIDATE)
	{
		encodingChannels[i].owner = PBX_VALUE_INVALIDATE;
		if(encodingChannels[i].colChannel != PBX_VALUE_INVALIDATE)
		{
			PBX_COLUMN_FREE(encodingChannels[i].colChannel);
			encodingChannels[i].colChannel = PBX_VALUE_INVALIDATE;
		}
//		encodingChannels[i].timeout = 0;
	}
}


/* look for a free column, always return 4~7  */
INT8U	pbx_find_free_column(void )
{
	INT8U	data i;
	
	if( (pbxHwStatus.columnFrees&0xF0)==0)
		return PBX_VALUE_INVALIDATE;

	for(i=4; i < 8; i++)
	{
		if(PBX_CHECK_FLAGS(pbxHwStatus.columnFrees, i))
			return i;
	}
	return PBX_VALUE_INVALIDATE;
}

/* always return 0~3 */
INT8U	findFreeIpChannel(void)
{
	INT8U	data i;
	
	if( (pbxHwStatus.columnFrees&0x0F) == 0 )
		return PBX_VALUE_INVALIDATE;
	
	for(i=0; i<PBX_IP_COUNT; i++)
	{
		if(PBX_CHECK_FLAGS(pbxHwStatus.columnFrees,i))
			return i;
	}
	
	return PBX_VALUE_INVALIDATE;
}

/* find dynamic row which can be only found in PSTN line Channels */
INT8U	findFreeRow4Ip(void)
{
	if(PBX_CHECK_FLAGS(pbxHwStatus.rowFrees, CHANNEL_ROW_IP_R_1))
		return CHANNEL_ROW_IP_R_1;
	
	if(PBX_CHECK_FLAGS(pbxHwStatus.rowFrees, CHANNEL_ROW_IP_R_2))
		return CHANNEL_ROW_IP_R_2;
	
	return PBX_VALUE_INVALIDATE;
}

/* find a row between 0 ~7 which is for a EXT */
INT8U	pbx_find_free_row_ext(void)
{
	INT8U	data x, y=0x01;

	if(pbxHwStatus.rowFrees!=0)/* 0: all are busy */
	{
		for(x=0; x< PBX_FXS_COUNT; x++)
		{
			if((pbxHwStatus.rowFrees&y) !=0 )
			{
				return x;
			}
			y=y<<1;
		}
	}

	return PBX_VALUE_INVALIDATE;
}

/*************************************************************
* Build Voice Channel between Caller and Callee
**************************************************************/
#define	_CALL_LOCAL_2_LOCAL	0	/* LOCAL : exts and lines */
#define	_CALL_LOCAL_2_IP		1	/* LOCAL-->IP and IP-->LOCAL */
#define	_CALL_IP_2_IP			2	/* IP <--> IP*/

static INT8U	_find_call_mode(INT8U x, INT8U y)
{
	if( IS_ROW_DEVICE(x))
	{
		if( IS_ROW_DEVICE(y) )
			return _CALL_LOCAL_2_LOCAL;
		else
			return _CALL_LOCAL_2_IP;
	}
	else
	{
		if( IS_ROW_DEVICE(y) )
			return _CALL_LOCAL_2_IP;
		else
			return _CALL_IP_2_IP;
	}
	return PBX_VALUE_INVALIDATE;
}

/* */
BOOL	voiceChannelConnect(INT8U x, INT8U y)
{
	INT8U data mode, peer = PBX_VALUE_INVALIDATE;
	BOOL	isOK = FALSE;

	if(!IS_CALL_DEVICE(x) || !IS_CALL_DEVICE(y) )
		return FALSE;
	
	mode = _find_call_mode( x, y);
	if(mode == _CALL_LOCAL_2_LOCAL)
	{/* x and y are both in row, so find a free column to connect them */
		peer = pbx_find_free_column();
		if(peer != PBX_VALUE_INVALIDATE )
		{
			CONNECT(x, peer);
			CONNECT(y, peer);

			PBX_COLUMN_BUSY(peer);
#if WITH_UART_DEBUG_ARRAY
			uart_debug_4_int_value(UART_DEBUG_COL_RESOURCE, x, y, peer, 1);
#endif
			isOK = TRUE;
		}
	}
	else if(mode == _CALL_LOCAL_2_IP)
	{/* x and y are in row and column */
		if( IS_ROW_DEVICE(x) )
			CONNECT(x, GET_IP_CHANNEL_INDEX(y) );
		else
			CONNECT(y, GET_IP_CHANNEL_INDEX(x) );
		
		isOK = TRUE;
	}
	else	/*ip-ip*/
	{
		peer = findFreeRow4Ip();
		if(peer != PBX_VALUE_INVALIDATE)
		{
			CONNECT(peer, GET_IP_CHANNEL_INDEX(x) );
			CONNECT(peer, GET_IP_CHANNEL_INDEX(y) );

			PBX_ROW_BUSY(peer);
			isOK = TRUE;
		}
	}

#if 1	
//	if(peer != PBX_VALUE_INVALIDATE)
	{
		callDevices[x].phyPeerId = peer;
		callDevices[y].phyPeerId = peer;
	}
#endif

	return isOK;
}

void	voiceChannelBroken(INT8U x, INT8U y)
{
	INT8U data mode, peer = PBX_VALUE_INVALIDATE;
	
	if(!IS_CALL_DEVICE(x) || !IS_CALL_DEVICE(y) )
		return;
	peer = callDevices[x].phyPeerId;
	mode = _find_call_mode(x, y);
	if( (mode == _CALL_LOCAL_2_LOCAL) && (peer!= PBX_VALUE_INVALIDATE) )
	{
		DISCONNECT(x, peer);
		DISCONNECT(y, peer);
		PBX_COLUMN_FREE(peer);
#if WITH_UART_DEBUG_ARRAY
		uart_debug_4_int_value(UART_DEBUG_COL_RESOURCE,x, y, peer, 0);
#endif
	}
	else if(mode == _CALL_LOCAL_2_IP)
	{
		if( IS_ROW_DEVICE(x) )
			DISCONNECT(x, GET_IP_CHANNEL_INDEX(y) );
		else
			DISCONNECT(y, GET_IP_CHANNEL_INDEX(x) );
	}
	else	 if(peer != PBX_VALUE_INVALIDATE )
	{
		DISCONNECT(peer, GET_IP_CHANNEL_INDEX(x) );
		DISCONNECT(peer, GET_IP_CHANNEL_INDEX(y) );
		PBX_ROW_FREE(peer);
	}

#if 1	
	callDevices[x].phyPeerId = PBX_VALUE_INVALIDATE;
	callDevices[y].phyPeerId = PBX_VALUE_INVALIDATE;
#endif	
}

#if 0
/* all phone's music is play by CD4066 */
void	extension_ringback_tone_play(INT8U devIndex, BOOL isEnable)
{
	if(isEnable)
	{
		PBX_SET_FLAGS(pbxHwStatus.ringbackStatus, devIndex);
	}
	else
	{
		PBX_CLEAR_FLAGS(pbxHwStatus.ringbackStatus, devIndex);
	}

	EXT_CTRL_RINGBACK();
}
#endif


void	devMusicPlay(INT8U x, BOOL isPlay)
{
	if( IS_EXT_DEVICE(x) )
	{
		if(isPlay)
		{
			PBX_SET_FLAGS(extDevices.musicStatus, x);
		}
		else
		{
			PBX_CLEAR_FLAGS(extDevices.musicStatus, x);
		}

		EXT_CTRL_MUSIC();
	}
	else if( IS_PSTN_LINE_DEVICE(x) )
	{
		switch_connect(x, CHANNEL_COL_MUSIC, isPlay);
	}
#if WITH_IP_VOICE_CHANNEL			
	else
	{
		switch_connect(CHANNEL_ROW_IP_R_1, GET_IP_CHANNEL_INDEX(x), isPlay );
		switch_connect(CHANNEL_ROW_IP_R_1, CHANNEL_COL_MUSIC, isPlay);
	}
#endif
}

void devRecordPlay(INT8U rowDev, BOOL isPlay)
{
	if(!IS_ROW_DEVICE(rowDev) )
		return;
	
	switch_connect(rowDev, CHANNEL_COL_RECORD,isPlay);
	if(isPlay == TRUE)
	{
		RECORDER_PLAY_STOP();
		RECORDER_PLAY_START();
	}
	else
	{
		RECORDER_PLAY_STOP();
	}
}

void	devToneRingbackPlay(INT8U devIndex, BOOL isPlay)
{	
	if( IS_EXT_DEVICE(devIndex) )
	{
#if 1
		if(isPlay==TRUE)
		{
			PBX_SET_FLAGS(extDevices.toneCtrl, devIndex);
		}
		else
		{
			PBX_CLEAR_FLAGS(extDevices.toneCtrl, devIndex);
		}
		EXT_CTRL_TONE();
#else
		switch_connect(devIndex, CHANNEL_COL_TONE, isPlay);
//		switch_connect(devIndex, CHANNEL_COL_RINGBACK_TONE, isPlay);
#endif
	}
	else if( IS_PSTN_LINE_DEVICE(devIndex) )
	{
		switch_connect(devIndex, CHANNEL_COL_TONE, isPlay);
//		switch_connect(devIndex, CHANNEL_COL_RINGBACK_TONE, isPlay);
	}
#if WITH_IP_VOICE_CHANNEL			
	else
	{/* CONNECT IP col and Ringback tone col with a preallocated row(14 or 15) */
		switch_connect(CHANNEL_ROW_IP_R_1,  CHANNEL_COL_TONE, isPlay );	
		switch_connect(CHANNEL_ROW_IP_R_1,  GET_IP_CHANNEL_INDEX(devIndex), isPlay );
	}
#endif
}

