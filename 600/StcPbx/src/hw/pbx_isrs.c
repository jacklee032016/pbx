/*
* $Id: pbx_isrs.c,v 1.14 2007/07/22 20:48:13 lizhijie Exp $
*/

#pragma	ot(9,speed)
#pragma small

#include <REGSTCRD+.H>
#include "function_declarations.h"

#include "_pbx_state_handles.h"


void	pbx_isr_uart(void) interrupt 4 using 3
{
	INT8U 	xdata	*buf;
	INT8U	data		lastIndex;
	
	if(RI)
	{
		if(uartRxInIndex == PBX_VALUE_INVALIDATE)
		{/* no close RI when return */
			CTS = UART_CTS_DISABLE;
		}
		else
		{
			RI=0;
			
			buf = (INT8U xdata *)(&uartRxCmds[uartRxInIndex]);
			*(buf+ uartRxBuffersPosition) = SBUF;
			
			if( (*(buf+ uartRxBuffersPosition) == 0xff) ||((uartRxBuffersPosition+1)== UART_BUFFER_SIZE) )
			{
				uartRxBuffersPosition=0;
				lastIndex = uartRxInIndex;
				
				uartRxInIndex++;
				if( (uartRxInIndex == UART_BUFFER_COUNT) )
				{
					uartRxInIndex = 0;
				}
				
				if( uartRxInIndex == uartRxOutIndex )
				{/* block myself */
					uartRxInIndex = PBX_VALUE_INVALIDATE;
					CTS = UART_CTS_DISABLE;
				}
				
				if( uartRxOutIndex== PBX_VALUE_INVALIDATE)
				{/* wake up main loop to read data */
					uartRxOutIndex = lastIndex;
				}
				
			}
			else
			{
				uartRxBuffersPosition++;
			}
		}
	}
	else 

	if(TI)
	{
		TI=0;
		if( uartTxOutIndex != PBX_VALUE_INVALIDATE)
		{
			buf = (INT8U xdata *)(&uartTxCmds[uartTxOutIndex] );

			SBUF= *(buf+uartTxBuffersPosition);
			
			if( (*(buf+uartTxBuffersPosition) == PBX_VALUE_INVALIDATE ) )
			{
				lastIndex = uartTxOutIndex;
				uartTxBuffersPosition = 0;

				if(uartTxInIndex == PBX_VALUE_INVALIDATE )
				{/*wakeup main loop as once */
					uartTxInIndex = lastIndex;
				}

				uartTxOutIndex++;
				if(uartTxOutIndex == UART_TX_BUFFER_COUNT)
				{
					uartTxOutIndex = 0;
				}

				if(uartTxOutIndex == uartTxInIndex)
				{/* block myself */
					uartTxOutIndex = PBX_VALUE_INVALIDATE;
				}

			}
			else
			{		
				uartTxBuffersPosition++;
			}
		}
	}
}

#if WITH_PSTN_FSK_DATA_DEBUG
INT8U	xdata d[16];
#endif

#if WITH_FSK_MT88E39	
/*External Interrupt 0, highest priority */
void	pbx_external_isr_0(void) interrupt 0 using 2 
{
	EX0 = FALSE;		/*disable extern interrupt 0 */
	

	TL1= (65536-(unsigned)(22.1184*1000/12))%256;
	TH1= (65536-(unsigned)(22.1184*1000/12))/256;

	ET1 = TRUE;		/* enable time interrupt 1 */
	TR1 = TRUE;		/* open time 1 start now */

	/* initial buffer and count for caller ID from PSTN */
	fskDecoder.bitCount = 0;
	fskDecoder.currentDigits = 0;

	/* low level is active for 74HC245 */
	MT88E39_DATA_ENABLE = FALSE;
	MT88E39_DCLK = TRUE;
//		uart_tx_2_int_value(CMD_UART_DEBUG, FSK_DATA_RAW_DATA, fskDecoder.currentDigits );
}


/* different ISR can not use same Register Bank */
void	timer_isr_03(void) interrupt 3 	/* //using 2 //Timer 1 : INT 3 */
{
	BOOL	errFlag = FALSE;
	INT8U  	pos = 0;

	TF1 = FALSE;
	
	MT88E39_DCLK = FALSE;

	fskDecoder.bitCount++;

	if(fskDecoder.bitCount <= 8) /*this is last data bit, eg. b7 */
	{
		fskDecoder.currentDigits>>=1;
		if(MT88E39_DATA)
			fskDecoder.currentDigits|= 0x80;

	}

	
	if(fskDecoder.bitCount == 8)
	{
#if WITH_PSTN_FSK_DATA_DEBUG
#if 0
		if(fskDecoder.fskIndex == 0)
		{
			if(fskDecoder.currentDigits != FSK_FORMAT_SDMF && fskDecoder.currentDigits != FSK_FORMAT_MDMF)
			{
				uart_tx_2_int_value(CMD_UART_DEBUG, FSK_DATA_ERR_FORMAT, fskDecoder.currentDigits );
				errFlag = TRUE;
				fskDecoder.fskIndex = 0;
			}
		}
#endif

		d[fskDecoder.fskIndex] = fskDecoder.currentDigits;
		uart_tx_2_int_value(CMD_UART_DEBUG, FSK_DATA_RAW_DATA, fskDecoder.currentDigits );
//		if(fskDecoder.fskIndex == 4)
		{
//			pbx_uart_debug(d[], 1);
		}
#endif			
		if(fskDecoder.fskIndex == 1)
		{
			fskDecoder.length = fskDecoder.currentDigits;
		}

		fskDecoder.fskIndex++;
		if(LOCAL_IS_CHINA() && fskDecoder.fskIndex > (FSK_CHINESE_DATE_PARAM_LENGTH + 2) )
		{
			if(fskDecoder.currentDigits>= 0x31 && fskDecoder.currentDigits<= 0x39)
			{
				fskDecoder.currentDigits = fskDecoder.currentDigits - 0x30;
			}
			else if(fskDecoder.currentDigits == 0x30)
			{
				fskDecoder.currentDigits = DIAL_DIGIT_ZERO;
			}
			else if(fskDecoder.currentDigits == 0x23) /* '#' */
			{
				fskDecoder.currentDigits = DIAL_DIGIT_SPECIAL;
			}
			else
			{
				fskDecoder.currentDigits = DIAL_DIGIT_ASTREIC;
			}
		
			if(callerIdData[fskDecoder.currentChannel].length > 2*PHONE_DIGIT_BUFFER_SIZE )
			{
				pos = PHONE_DIGIT_BUFFER_SIZE;
			}
			else
			{
				pos = callerIdData[fskDecoder.currentChannel].length/2;
			}
			
			if( IS_ODD(callerIdData[fskDecoder.currentChannel].length) )
			{
				callerIdData[fskDecoder.currentChannel].value[pos] |= fskDecoder.currentDigits&0x0f;
			}
			else
			{
				callerIdData[fskDecoder.currentChannel].value[pos] = fskDecoder.currentDigits<<4;
			}

			callerIdData[fskDecoder.currentChannel].length++;

		}

#if 0
		if(fskDecoder.fskIndex == fskDecoder.length)
		{
		}
#endif
		errFlag = TRUE;
	}
	
	if(errFlag)/* error */
	{
		ET1 = FALSE;	/* disable Timer 1 */
		TR1 = FALSE;
		EX0 = TRUE;
		MT88E39_DCLK = FALSE;
		
		MT88E39_DATA_ENABLE = TRUE;
	}
	else
	{
		MT88E39_DCLK = TRUE;

		TL1= (65536-(unsigned)(22.1184*1000/12))%256;
		TH1= (65536-(unsigned)(22.1184*1000/12))/256;

		ET1 = TRUE;		/* enable time interrupt 1 */
		TR1 = TRUE;		/* open time 1 start now */
	}
}

#else

#if SM8223AP_FALLING_EDGE_SAMPLE
/*External Interrupt 0, highest priority */
void	pbx_external_isr_0(void) interrupt 0 using 2 
{
	if(fskWaitStopFlag == FALSE)
	{
		if( fskFirstByteFlag)
		{
			TL1=(65536-(unsigned)(22.1184*100*11/12))%256;
			TH1=(65536-(unsigned)(22.1184*100*11/12))/256;
			fskFirstByteFlag = FALSE;
		}
#if 1	
		else
		{
			TL1= (65536-(unsigned)(22.1184*100*11/1.2/12))%256;
			TH1= (65536-(unsigned)(22.1184*100*11/1.2/12))/256;
		}
#else	
		TL1=93;/* 1.2 ms delay for Tadd */
		TH1=247;
#endif	
	}
	else
	{
		TL1 = 254;
		TH1 = 0xFF;
		TF1 = TRUE;
	}

	EX0 = FALSE;		/*disable extern interrupt 0 */
	ET1 = TRUE;		/* enable time interrupt 1 */
	TR1 = TRUE;		/* open time 1 start now */

	/* initial buffer and count for caller ID from PSTN */
	fskDecoder.bitCount = 0;
	fskDecoder.currentDigits = 0;
}


/* different ISR can not use same Register Bank */
void	timer_isr_03(void) interrupt 3 	/* //using 2 //Timer 1 : INT 3 */
{
	BOOL	errFlag = FALSE;
	INT8U  	pos = 0;

	TF1 = FALSE;
//	TL1|=0;//(65536-(unsigned)(22.1184*1000/1.2/12))%256;	/* 1ms */
#if 0
	TL1 |= (65536-(unsigned)(22.1184*1000/1.2/12))%256;	/* 1ms */
	TH1= (65536-(unsigned)(22.1184*1000/1.2/12))/256;
#else	
//	TL1 += 0;//172;//(65536-(unsigned)(22.1184*1000/1.2/12))%256;	/* 1ms */
//	TH1= 250;//249;//(65536-(unsigned)(22.1184*1000/1.2/12))/256;
//	TL1 += (65536-(unsigned)(22.1184*1000/1.2/12))%256;	/* 1ms */
	TL1 = 0;//(65536-(unsigned)(22.1184*1000/1.2/12))%256;
	TH1= (65536-(unsigned)(22.1184*1000/1.2/12))/256;
#endif	
	ET1 = TRUE;		/* enable time interrupt 1 */
	TR1 = TRUE;		/* open time 1 start now */
	/*1200 bps for FSK ??? */

	fskDecoder.bitCount++;
	if(fskDecoder.bitCount==1) /* this is start bit */
	{

		if(INT0==0)/* 0 : is in interrupt ??? */	
		{/* start bit must be 0 , refer specs. p.12 */
			EX0 = FALSE;
		}
		else
		{
			fskWaitStopFlag = TRUE;
			errFlag = TRUE;
			
#if WITH_PSTN_FSK_DATA_DEBUG
			uart_tx_2_int_value(CMD_UART_DEBUG, FSK_DATA_ERR_START_BIT, fskDecoder.fskIndex );
			if(fskDecoder.fskIndex> 0)
			{
				pbx_uart_debug(d, fskDecoder.fskIndex);
			}
#endif			
		}

	}
	else
	{
		fskWaitStopFlag = FALSE;
		if(fskDecoder.bitCount >= 9) /*this is last data bit, eg. b7 */
		{

			fskDecoder.currentDigits >>=1;
			fskDecoder.bitCount = 0;

#if WITH_PSTN_FSK_DATA_DEBUG
			if(fskDecoder.fskIndex == 0)
			{
				if(d[0] != FSK_FORMAT_SDMF && d[0] != FSK_FORMAT_MDMF)
				{
					uart_tx_2_int_value(CMD_UART_DEBUG, FSK_DATA_ERR_FORMAT, fskDecoder.currentDigits );
					errFlag = TRUE;
					fskDecoder.fskIndex = 0;
				}
			}
#endif			
			if(fskDecoder.fskIndex == 1)
			{
				fskDecoder.length = fskDecoder.currentDigits;
			}

			fskDecoder.fskIndex++;
			if(LOCAL_IS_CHINA() && fskDecoder.fskIndex > (FSK_CHINESE_DATE_PARAM_LENGTH + 2) )
			{
				if(fskDecoder.currentDigits>= 0x31 && fskDecoder.currentDigits<= 0x39)
				{
					fskDecoder.currentDigits = fskDecoder.currentDigits - 0x30;
				}
				else if(fskDecoder.currentDigits == 0x30)
				{
					fskDecoder.currentDigits = DIAL_DIGIT_ZERO;
				}
				else if(fskDecoder.currentDigits == 0x23) /* '#' */
				{
					fskDecoder.currentDigits = DIAL_DIGIT_SPECIAL;
				}
				else
				{
					fskDecoder.currentDigits = DIAL_DIGIT_ASTREIC;
				}
			
				if(callerIdData[fskDecoder.currentChannel].length > 2*PHONE_DIGIT_BUFFER_SIZE )
				{
					pos = PHONE_DIGIT_BUFFER_SIZE;
				}
				else
				{
					pos = callerIdData[fskDecoder.currentChannel].length/2;
				}
				
				if( IS_ODD(callerIdData[fskDecoder.currentChannel].length) )
				{
					callerIdData[fskDecoder.currentChannel].value[pos] |= fskDecoder.currentDigits&0x0f;
				}
				else
				{
					callerIdData[fskDecoder.currentChannel].value[pos] = fskDecoder.currentDigits<<4;
				}

				callerIdData[fskDecoder.currentChannel].length++;

			}

#if 0
			if(fskDecoder.fskIndex == fskDecoder.length)
			{
			}
#endif
			
#if WITH_PSTN_FSK_DATA_DEBUG
			d[fskDecoder.fskIndex-1] = fskDecoder.currentDigits;
			if(fskDecoder.fskIndex == 14)
			{
				pbx_uart_debug(d, 12);
				fskDecoder.fskIndex = 0;
			}
#endif

			errFlag = TRUE;
		}
		else	/* data bit */
		{
			fskDecoder.currentDigits>>=1;
			if(INT0)
				fskDecoder.currentDigits|= 0x80;
		}
	}
	
	if(errFlag)/* error */
	{
		ET1 = FALSE;	/* disable Timer 1 */
		TR1 = FALSE;
		EX0 = TRUE;
	}

}
#else

/*External Interrupt 0, highest priority */
void	pbx_external_isr_0(void) interrupt 0 using 2 
{
	TL1= (65536-(unsigned)(22.1184*100*11/12))%256;
	TH1= (65536-(unsigned)(22.1184*100*11/12))/256;

	EX0 = FALSE;		/*disable extern interrupt 0 */
	ET1 = TRUE;		/* enable time interrupt 1 */
	TR1 = TRUE;		/* open time 1 start now */

	/* initial buffer and count for caller ID from PSTN */
	fskDecoder.bitCount = 0;
	fskDecoder.currentDigits = 0;
}


/* different ISR can not use same Register Bank */
void	timer_isr_03(void) interrupt 3 	/* //using 2 //Timer 1 : INT 3 */
{
	BOOL	errFlag = FALSE;
	INT8U  	pos = 0;

	TF1 = FALSE;
#if 0
	TL1 |= (65536-(unsigned)(22.1184*1000/1.2/12))%256;	/* 1ms */
	TH1= (65536-(unsigned)(22.1184*1000/1.2/12))/256;
#else	
//	TL1 += 0;//172;//(65536-(unsigned)(22.1184*1000/1.2/12))%256;	/* 1ms */
//	TH1= 250;//249;//(65536-(unsigned)(22.1184*1000/1.2/12))/256;
//	TL1 += (65536-(unsigned)(22.1184*1000/1.2/12))%256;	/* 1ms */
	TL1 = 0;//(65536-(unsigned)(22.1184*1000/1.2/12))%256;
	TH1= 250;//(65536-(unsigned)(22.1184*1000/1.2/12))/256;
#endif	
	ET1 = TRUE;		/* enable time interrupt 1 */
	TR1 = TRUE;		/* open time 1 start now */
	/*1200 bps for FSK ??? */

	fskDecoder.bitCount++;
#if 0
	if(fskDecoder.bitCount==1) /* this is start bit */
	{

		if(INT0==0)/* 0 : is in interrupt ??? */	
		{/* start bit must be 0 , refer specs. p.12 */
			EX0 = FALSE;
		}
		else
		{
			fskWaitStopFlag = TRUE;
			errFlag = TRUE;
			
#if WITH_PSTN_FSK_DATA_DEBUG
			uart_tx_2_int_value(CMD_UART_DEBUG, FSK_DATA_ERR_START_BIT, fskDecoder.fskIndex );
			if(fskDecoder.fskIndex> 0)
			{
				pbx_uart_debug(d, fskDecoder.fskIndex);
			}
#endif			
		}

	}
	else
#endif

	fskWaitStopFlag = FALSE;
	
	if(fskDecoder.bitCount <= 8) /*this is last data bit, eg. b7 */
	{

		fskDecoder.currentDigits>>=1;
		if(INT0)
			fskDecoder.currentDigits|= 0x80;

	}
	else if(fskDecoder.bitCount >= 9)
	{
#if WITH_PSTN_FSK_DATA_DEBUG
		if(fskDecoder.fskIndex == 0)
		{
			if(fskDecoder.currentDigits != FSK_FORMAT_SDMF && fskDecoder.currentDigits != FSK_FORMAT_MDMF)
			{
				uart_tx_2_int_value(CMD_UART_DEBUG, FSK_DATA_ERR_FORMAT, fskDecoder.currentDigits );
				errFlag = TRUE;
				fskDecoder.fskIndex = 0;
			}
		}
		
		d[fskDecoder.fskIndex] = fskDecoder.currentDigits;
		if(fskDecoder.fskIndex == 4)
		{
//			pbx_uart_debug(d, 4);
		}
#endif			
		if(fskDecoder.fskIndex == 1)
		{
			fskDecoder.length = fskDecoder.currentDigits;
		}

		fskDecoder.fskIndex++;
		if(LOCAL_IS_CHINA() && fskDecoder.fskIndex > (FSK_CHINESE_DATE_PARAM_LENGTH + 2) )
		{
			if(fskDecoder.currentDigits>= 0x31 && fskDecoder.currentDigits<= 0x39)
			{
				fskDecoder.currentDigits = fskDecoder.currentDigits - 0x30;
			}
			else if(fskDecoder.currentDigits == 0x30)
			{
				fskDecoder.currentDigits = DIAL_DIGIT_ZERO;
			}
			else if(fskDecoder.currentDigits == 0x23) /* '#' */
			{
				fskDecoder.currentDigits = DIAL_DIGIT_SPECIAL;
			}
			else
			{
				fskDecoder.currentDigits = DIAL_DIGIT_ASTREIC;
			}
		
			if(callerIdData[fskDecoder.currentChannel].length > 2*PHONE_DIGIT_BUFFER_SIZE )
			{
				pos = PHONE_DIGIT_BUFFER_SIZE;
			}
			else
			{
				pos = callerIdData[fskDecoder.currentChannel].length/2;
			}
			
			if( IS_ODD(callerIdData[fskDecoder.currentChannel].length) )
			{
				callerIdData[fskDecoder.currentChannel].value[pos] |= fskDecoder.currentDigits&0x0f;
			}
			else
			{
				callerIdData[fskDecoder.currentChannel].value[pos] = fskDecoder.currentDigits<<4;
			}

			callerIdData[fskDecoder.currentChannel].length++;

		}

#if 0
		if(fskDecoder.fskIndex == fskDecoder.length)
		{
		}
#endif
		errFlag = TRUE;
	}
	
	if(errFlag)/* error */
	{
		ET1 = FALSE;	/* disable Timer 1 */
		TR1 = FALSE;
		EX0 = TRUE;
	}

}
#endif
#endif	/* if SM88E30 */

/* interrupt in 25ms and set flags for 50/100ms , 1 minutes and ringing timer */
void	timer_isr_01(void) interrupt 1 using 1	/* Timer 0 */
{
	TL0|=0; /*(65536-(unsigned )(25000*22.1184/12))%256; */
	TH0= 76;/*(65536-25000*22.1184/12)/256; */
	
	bool25msFlag = TRUE;
	if(++ pbxHwStatus.Counter50ms == 2)
	{
		bool50msFlag = TRUE;
		pbxHwStatus.Counter50ms = 0;
	}

	if(++ pbxHwStatus.Counter100ms == 4 )
	{
		bool100msFlag = TRUE;
		pbxHwStatus.Counter100ms = 0;
	}

	if(++pbxHwStatus.Counter400ms == 16)
	{
		pbxHwStatus.Counter400ms = 0;
		bool400msFlag = TRUE;
		
		if(++pbxHwStatus.CounterMinutes == 150)
		{/* 60 seconds */
			bool1MinuteFlag = TRUE;
			pbxHwStatus.CounterMinutes =0;
		}
	}
	
}


