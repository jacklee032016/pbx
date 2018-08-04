/*
* $Id: selfTest.c,v 1.4 2007/09/04 21:00:44 lizhijie Exp $
*/

#pragma	ot(9,speed)
//#pragma large
#pragma small

#include <REGSTCRD+.H>
#include "declarations.h"


#define	SELF_TEST_RTC				2
#define	SELF_TEST_ARRAY			4
#define	SELF_TEST_CALLERID		6
#define	SELF_TEST_EEPROM			8

BOOL _check_realtime(INT8U xdata *value)
{
	pbx_systime_reload();

#if WITH_UART_DEBUG_STSTIME
	uartTxCmds[uart.responseInIndex].value[0] = UART_DEBUG_STSTIME;
	uartTxCmds[uart.responseInIndex].value[1] = pbxHwStatus.sysTime.second;
	uartTxCmds[uart.responseInIndex].value[2] = pbxHwStatus.sysTime.minute;
	uartTxCmds[uart.responseInIndex].value[3] = pbxHwStatus.sysTime.hour;
	uartTxCmds[uart.responseInIndex].value[4] = pbxHwStatus.sysTime.date;
	uartTxCmds[uart.responseInIndex].value[5] = pbxHwStatus.sysTime.month;
	uartTxCmds[uart.responseInIndex].value[6] = pbxHwStatus.sysTime.day;
	uartTxCmds[uart.responseInIndex].value[7] = pbxHwStatus.sysTime.year;
	uart_tx_cmd(CMD_UART_DEBUG, 8);
#endif

	if( (pbxHwStatus.sysTime.second < 60) && 
		(pbxHwStatus.sysTime.minute < 60) &&
		(pbxHwStatus.sysTime.hour < 24) && 
		(pbxHwStatus.sysTime.date<31 ) && 
		(pbxHwStatus.sysTime.month<12) &&
		(pbxHwStatus.sysTime.day<8) && 
		(pbxHwStatus.sysTime.year <99 ) )
	{
		if(!((pbxHwStatus.sysTime.second == 0) && 
			(pbxHwStatus.sysTime.minute == 0) &&
			(pbxHwStatus.sysTime.hour == 0) && 
			(pbxHwStatus.sysTime.date == 0 ) && 
			(pbxHwStatus.sysTime.month == 0) &&
			(pbxHwStatus.sysTime.day == 0) && 
			(pbxHwStatus.sysTime.year == 0 ) ))
		{
			PBX_SELFTEST_FEEDBACK_OK( value[1]);
			PBX_SELFTEST_MODE_CANCEL();
			return TRUE;
		}
	}

	pbx_reset_systime();
	PBX_SELFTEST_FEEDBACK_FAIL(SELFTEST_CAUSE_NOCHIP);
	
	/* cancel is process by handler when return FALSE */
	return FALSE;
}

BOOL _check_ringer(INT8U xdata *value)
{
	if(value != PBX_VALUE_INVALIDATE)
	{
		pbx_ring_all(SET_BIT(1, (value[1]-1) ) );
	}
	else
	{
		pbx_ring_all(0XFF);
	}

	PBX_SELFTEST_FEEDBACK_OK( value[1]);
	PBX_SELFTEST_MODE_CANCEL();
	return TRUE;
}

BOOL _check_nvram(INT8U xdata *value)
{
	BOOL res = FALSE;
	
	if(baseConfigs.msgicString[0] == defaultBaseConfigs.msgicString[0] &&
		baseConfigs.msgicString[1] == defaultBaseConfigs.msgicString[1] )
	{
		res = TRUE;
		PBX_SELFTEST_FEEDBACK_OK( value[1]);
		PBX_SELFTEST_MODE_CANCEL();
	}
	else
	{
		PBX_SELFTEST_FEEDBACK_FAIL(SELFTEST_CAUSE_UNKNOWN);
	}

	return res;
}


#if 0
static void	__test_array_connect(INT8U data row,INT8U data col)
{
	CONNECT(row, col);
	CONNECT(row, CHANNEL_COL_DTMF_DECODE_1);
	
	if(col == 0)
	{
		CONNECT(row, CHANNEL_COL_DTMF_DECODE_2);
		CONNECT(row, CHANNEL_COL_DTMF_DECODE_3);
		CONNECT(row, CHANNEL_COL_DTMF_DECODE_4);
	}
	
	if(col < CHANNEL_COL_DTMF_DECODE_1 )
		CONNECT(CHANNEL_ROW_DTMF_ENCODE_1, col);
	else
		CONNECT(CHANNEL_ROW_DTMF_ENCODE_2, col);
}

static void	__test_array_disconnect(INT8U data row,INT8U data col)
{
	DISCONNECT(row, col);
	DISCONNECT(row, CHANNEL_COL_DTMF_DECODE_1);
	
	if(col==0)
	{
		DISCONNECT(row, CHANNEL_COL_DTMF_DECODE_2);
		DISCONNECT(row, CHANNEL_COL_DTMF_DECODE_3);
		DISCONNECT(row, CHANNEL_COL_DTMF_DECODE_4);
	}
	
	if(col<CHANNEL_COL_DTMF_DECODE_1)
		DISCONNECT(CHANNEL_ROW_DTMF_ENCODE_1, col);
	else
		DISCONNECT(CHANNEL_ROW_DTMF_ENCODE_2, col);
}

/* DTMF encoder/decoder and switch array */
static BOOL _check_ht9200_lc7385_mt8816(INT8U xdata *value)
{
	INT8U	data row, col;
	
	for(col =0; col <DECODE_CHANNEL_NUMBER; col++)
	{
		decodingChannels[col].digits.length = 0;
		decodingChannels[col].owner = col;
	}

	DTMF_ENCODE_SECOND_DISABLE();
	DTMF_ENCODE_FIRST_ENABLE(6);
	
	for(col=0; col<16; col++)
	{
		if(col == CHANNEL_COL_DTMF_DECODE_1)
		{
			col = CHANNEL_COL_MUSIC;
			DTMF_ENCODE_FIRST_DISABLE();
			DTMF_ENCODE_SECOND_ENABLE(6);
		}

		for(row =0; row<16; row++)
		{
			if(row == CHANNEL_ROW_DTMF_ENCODE_1)	
			{
				row = row+2;/*Skip row 12: TMF_Generate0 and row 13:DTMF_Generate1 */
			}
			
			__test_array_connect(row, col );
			pbx_mdelay(100);
			
			hwReadDtmfDecoders();

			__test_array_disconnect(row, col);
			pbx_mdelay(100);

			hwReadDtmfDecoders();

			WATCHDOG_FEED();
			
			if(col ==0)
			{
				if( ( (decodingChannels[0].digits.value[0]>>4)>>4==6 ) && 
					( (decodingChannels[1].digits.value[0]>>4)==6 )&& 
					( (decodingChannels[2].digits.value[0]>>4)==6 )&& 
					( (decodingChannels[3].digits.value[0]>>4)>>4==6) )
				{
					decodingChannels[col].digits.length = 0;
					decodingChannels[col].digits.value[0] = 0;
				}
				else
					return FALSE;
			}	
			else
			{
				if( (decodingChannels[col].digits.value[0]>>4 )==6)
				{
					decodingChannels[col].digits.length = 0;
					decodingChannels[col].digits.value[0] = 0;
				}
				else
					return FALSE;
			}
		}
	}
	
	return	TRUE;
}

static BOOL	_check_sm8233(INT8U xdata *value)
{
#define	__FIRST_PSTN_LINE		8
#define	__MY_COL				0

	INT8U data i;
	callerIdData[0].digits.value[0]=0x00;
	callerIdData[0].owner = PBX_VALUE_INVALIDATE;
	
	CONNECT(__FIRST_PSTN_LINE, __MY_COL);
	CallerIdChannelAllocate(__FIRST_PSTN_LINE);
	for(i=0;i<5;i++)
	{
		CONNECT(CHANNEL_ROW_DTMF_ENCODE_1,  __MY_COL);
		DTMF_ENCODE_FIRST_ENABLE(i);
		pbx_mdelay(100);
		DISCONNECT(CHANNEL_ROW_DTMF_ENCODE_1, __MY_COL);
		pbx_mdelay(100);
		
		WATCHDOG_FEED();
	}
	
	DISCONNECT(__FIRST_PSTN_LINE, __MY_COL);
	pstn_callerid_end(__FIRST_PSTN_LINE);

	if(callerIdData[0].digits.value[0] ==0x12 && callerIdData[0].digits.value[1] ==0x34)
		return TRUE;

	return FALSE;
}
#endif

/* DTMF encoder 9200Bx2 and DTMF decoder LC7385x4 */
static BOOL _check_dtmf_chips(INT8U xdata *value)
{
	INT8U	data encoder,decoder, row = pbx_find_free_row_ext();
	encoder = value[1];
	decoder = value[2];
	
	if(row == PBX_VALUE_INVALIDATE)
	{
		PBX_SELFTEST_FEEDBACK_FAIL( SELFTEST_CAUSE_BUSY );
		return FALSE;
	}
	
	if(encoder<0 || encoder > 1 ||decoder < 0 || decoder> 3)
	{
		PBX_SELFTEST_FEEDBACK_FAIL(SELFTEST_CAUSE_INVALIDATE_PARAM );
		return FALSE;
	}
	
	decodingChannels[decoder].digits.length = 0;
	decodingChannels[decoder].owner = row;

	PBX_ROW_BUSY(row);
	CALL_TIMER_START(row, SELFTEST_TIMER_LENGTH);

	if(encoder == 0)
	{
//		DTMF_ENCODE_FIRST_ENABLE(6);
		dtfm_encoder_digit(0, 6);
	}
	else
	{
//		DTMF_ENCODE_SECOND_ENABLE(6);
		dtfm_encoder_digit(1, 6);
	}
	
	CONNECT(CHANNEL_ROW_DTMF_ENCODE_1 +encoder, CHANNEL_COL_DTMF_DECODE_1 + decoder);
	
	return	TRUE;
}

static BOOL	_check_pcm_channel(INT8U xdata *value)
{
	INT8U	data col = value[1];
	INT8U	data row = pbx_find_free_row_ext();
	
	if(row == PBX_VALUE_INVALIDATE || col == PBX_VALUE_INVALIDATE)
	{
		return FALSE;
	}
#if WITH_UART_DEBUG						
	uart_tx_2_int_value(CMD_UART_DEBUG, CMD_SELFTEST_BUSY, col);
#endif

	if(!dtmfDecodingChannelAllocate(row) )
	{
		return FALSE;
	}
	
	PBX_ROW_BUSY(row);
	CALL_MY_PEER(row) = col;
	CALL_TIMER_START(row, SELFTEST_TIMER_LENGTH);
	
	CONNECT(row, col);
//	CONNECT(row, CHANNEL_COL_DTMF_DECODE_1);
	
	return TRUE;
}


pbx_selftest_handle_t		code		selftest_handlers[]=
{
	{
		SELF_TEST_TYPE_RTC,
		_check_realtime,
	},
	{
		SELF_TEST_TYPE_NVRAM,
		_check_nvram,
	},
	{
		SELF_TEST_TYPE_RINGER,
		_check_ringer,
	},
	/* following item exit from selftest in state of IDLE */
#if 0		
	{
		SELF_TEST_TYPE_ARRAY,
		_check_ht9200_lc7385_mt8816,
	},
#endif	
	{
		SELF_TEST_TYPE_CALLERID,
		_check_dtmf_chips,
	},
	{
		SELF_TEST_TYPE_PCM_CHANNEL,
		_check_pcm_channel,
	},
};

BOOL	self_test_handle(uart_cmd_t xdata *value)
{
	INT8U	data i;
	
	/* check everything is free in this time, need to enhanced here */
	
	for(i=0; i< sizeof(selftest_handlers)/sizeof(pbx_selftest_handle_t); i++)
	{
		if(selftest_handlers[i].testType == value->value[0] )
		{
			PBX_SELFTEST_MODE_ENTER(selftest_handlers[i].testType);
			
			if( (selftest_handlers[i].handle)(value->value) == FALSE)
			{
//				__selftest_error_sound(selftest_handlers[i].testType*2);
				pbxBuzzer(i+1);
				
	//			PBX_SELFTEST_FEEDBACK_FAIL( value->value[1]);
				
				PBX_SELFTEST_MODE_CANCEL();
			}
			return TRUE;
		}
	}

	return FALSE;
}


INT8U _check_dtmf_exit(INT8U rowIndex, INT8U event)
{
	INT8U	data channel;

	if( (rowIndex = dtmfDecodingChannelFind(rowIndex)) != PBX_VALUE_INVALIDATE)
	{
		if(decodingChannels[channel].digits.length >= 1)
		{
			PBX_SELFTEST_FEEDBACK_OK(decodingChannels[channel].digits.value[0] );
//			PBX_SELFTEST_FEEDBACK_OK(decodingChannels[channel].digits.length );
		}
		else
		{
			PBX_SELFTEST_FEEDBACK_FAIL( event);
		}
		
		dtmfDecodingChannelFree(rowIndex);
	}

	DISCONNECT(CHANNEL_ROW_DTMF_ENCODE_1, CHANNEL_COL_DTMF_DECODE_1);
	DISCONNECT(CHANNEL_ROW_DTMF_ENCODE_1, CHANNEL_COL_DTMF_DECODE_2);
	DISCONNECT(CHANNEL_ROW_DTMF_ENCODE_1, CHANNEL_COL_DTMF_DECODE_3);
	DISCONNECT(CHANNEL_ROW_DTMF_ENCODE_1, CHANNEL_COL_DTMF_DECODE_4);

	CALL_MY_PEER(rowIndex) = PBX_VALUE_INVALIDATE;

	PBX_ROW_FREE(rowIndex);
	PBX_SELFTEST_MODE_CANCEL();

	return PBX_STATE_CONTINUE;
}


INT8U _check_pcm_exit(INT8U rowIndex, INT8U event)
{
	INT8U	data channel;

	if( (rowIndex = dtmfDecodingChannelFind(rowIndex)) != PBX_VALUE_INVALIDATE)
	{
		if(decodingChannels[channel].digits.length >= 1)
		{
//				PBX_SELFTEST_FEEDBACK_OK(decodingChannels[channel].digits.value[0] );
			PBX_SELFTEST_FEEDBACK_OK(decodingChannels[channel].digits.length );
		}
		else
		{
			PBX_SELFTEST_FEEDBACK_FAIL( event);
		}
		
		dtmfDecodingChannelFree(rowIndex);
	}

	DISCONNECT(rowIndex, CALL_MY_PEER(rowIndex) );
//		DISCONNECT(devIndex, CHANNEL_COL_DTMF_DECODE_1);

	CALL_MY_PEER(rowIndex) = PBX_VALUE_INVALIDATE;

	PBX_ROW_FREE(rowIndex);
	PBX_SELFTEST_MODE_CANCEL();

	return PBX_STATE_CONTINUE;
}


pbx_selftest_exit_handle_t		code		selftest_exit_handlers[]=
{
	/* following item exit from selftest in state of IDLE */
#if 0	
	{
		SELF_TEST_TYPE_ARRAY,
		_check_ht9200_lc7385_mt8816,
	},
#endif	
	{
		SELF_TEST_TYPE_CALLERID,
		_check_dtmf_exit,
	},
	{
		SELF_TEST_TYPE_PCM_CHANNEL,
		_check_pcm_exit,
	},
};


INT8U	self_test_exit_handle(INT8U rowIndex, INT8U event)
{
	INT8U	data i;
	
	/* check everything is free in this time, need to enhanced here */
	
	if(PBX_SELFTEST_MODE_IS_ON() )
	{
		for(i=0; i< sizeof(selftest_exit_handlers)/sizeof(pbx_selftest_exit_handle_t); i++)
		{
			if(selftest_exit_handlers[i].testType == pbxHwStatus.selfTestFlags )
			{
				
				 i = (selftest_exit_handlers[i].handle)( rowIndex, event);
				
				PBX_SELFTEST_MODE_CANCEL();
				return i;
			}
		}
	}
	
	return PBX_STATE_CONTINUE;
}


