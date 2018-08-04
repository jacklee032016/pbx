/*
* $Id: pbx_hw_dtmf.c,v 1.11 2007/07/25 17:53:55 lizhijie Exp $
*/

#pragma	ot(9,speed)
#pragma small

#include <REGSTCRD+.H>
#include "function_declarations.h"

/*read digit in 50ms period. When StD pin is high,digit has been detected 
* total 8 DTMF decoder(LC7385) are used. 
StD0~3 connect to array decode_dtmf0~3, and StD4~7 connect to EXT0~3
*/
void	pbx_read_dtmf_digits(void)
{
	INT8U data i,status, digit;

	i = pbx_read_hw_status(HW_STATUS_DTMF_DECODER);
	status = i^ pbxHwStatus.dtmfDecodingStatus;/* which decoding channel has changed */
	
	if(status)	
	{
		pbxHwStatus.dtmfDecodingStatus = i;
		for(i=0; i< DECODE_CHANNEL_NUMBER; i++)/* count of decoding channels */
		{
			/* last time is low amd this time is high for every decoder */
			if( ((GET_BIT(status, 2*i) !=0)&&(GET_BIT(pbxHwStatus.dtmfDecodingStatus,2*i)!=0) ) ||
				((GET_BIT(status, 2*i+1) !=0) &&(GET_BIT(pbxHwStatus.dtmfDecodingStatus,2*i+1)!=0) )) 
			{

				DTMF_DECODER_ENABLE(i);
				digit = P0;

				if((decodingChannels[2*i].owner != PBX_VALUE_INVALIDATE) &&
					(GET_BIT(pbxHwStatus.dtmfDecodingStatus, 2*i) !=0) && 
					(GET_BIT(status, 2*i) !=0) )
				{
					decodingchannel_add_digits(2*i, (digit&0x0f) );
					DEVICE_EVENT_EMIT(decodingChannels[2*i].owner, PBX_EVENT_DIGIT);
#if UART_DEBUG					
					uart_debug_3_int_value(UART_DEBUG_DTMF, 2*i, decodingChannels[2*i].owner, (digit&0x0f) );
#endif
				}

				if((decodingChannels[2*i+1].owner != PBX_VALUE_INVALIDATE) &&
					(GET_BIT(pbxHwStatus.dtmfDecodingStatus, 2*i+1) !=0) &&
					(GET_BIT(status, 2*i+1) !=0) )
				{
					decodingchannel_add_digits(2*i+1, (digit>>4));
					DEVICE_EVENT_EMIT(decodingChannels[2*i+1].owner, PBX_EVENT_DIGIT);
#if UART_DEBUG					
					uart_debug_3_int_value(UART_DEBUG_DTMF, 2*i+1, decodingChannels[2*i+1].owner, (digit>>4));
#endif
				}
				
				DTMF_DECODER_DISABLE();

			}
		}
	}
}


/* 8 DTMF decoder(LC7385), first 4 is connect to array, last 4 connecto EXT0~3 */
BOOL decodingchannel_allocate(INT8U devIndex)	
{
	INT8U	data i;

	i = decodingchannel_find(devIndex);
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


void	decodingchannel_free(INT8U devIndex)
{
	INT8U	data k =decodingchannel_find(devIndex);
	
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

INT8U decodingchannel_find(INT8U devIndex)	
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


void decodingchannel_add_digits(INT8U index, INT8U digit)	
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


