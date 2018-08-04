/*
* $Id: pbxIsrs.c,v 1.4 2007/09/03 21:08:58 lizhijie Exp $
*/

#pragma	ot(9,speed)
#pragma small

#include <REGSTCRD+.H>
#include "declarations.h"

#if WITH_UART_DEBUG_PSTN_FSK_DATA
INT8U	xdata d[16];
INT8U 	data totalD = 0;	
#endif

#if WITH_FSK_MT88E39	
/*External Interrupt 0, highest priority */
void	pbx_external_isr_0(void) interrupt 0 using 2 
{
	EX0 = FALSE;		/*disable extern interrupt 0 */
	
	TL1= (65536-(unsigned)(22.1184*800/12))%256;
	TH1= (65536-(unsigned)(22.1184*800/12))/256;

	ET1 = TRUE;		/* enable time interrupt 1 */
	TR1 = TRUE;		/* open time 1 start now */

	/* initial buffer and count for caller ID from PSTN */
	fskDecoder.bitCount = 0;
	fskDecoder.currentDigits = 0;

	/* low level is active for 74HC245 */
	MT88E39_DATA_ENABLE = FALSE;
	MT88E39_DCLK = FALSE;
#if WITH_UART_DEBUG_PSTN_FSK_DATA
	totalD++;
	fskDecoder.fskIndex++;
#endif
}


/* different ISR can not use same Register Bank */
void	timer_isr_03(void) interrupt 3 	/* //using 2 //Timer 1 : INT 3 */
{
	INT8U  	pos = 0;

	MT88E39_DCLK = TRUE;
	NOP();
	NOP();
	NOP();
	NOP();
	NOP();

	TF1 = FALSE;
#if WITH_UART_DEBUG_PSTN_FSK_DATA
	d[0] = totalD;
	d[fskDecoder.fskIndex] = fskDecoder.fskIndex;//fskDecoder.currentDigits;
#endif

	if(fskDecoder.bitCount < 8) /*this is last data bit, eg. b7 */
	{
		if(MT88E39_DATA)
		{
			fskDecoder.currentDigits |= SET_BIT(1, fskDecoder.bitCount);
//			fskDecoder.currentDigits|= 0x80;
		}
//		fskDecoder.currentDigits>>=1;
	}
	
	fskDecoder.bitCount++;
	
	if(fskDecoder.bitCount < 8 )
	{/* next bit : continue timer IRQ */
		MT88E39_DCLK = FALSE;

		TL1= (65536-(unsigned)(22.1184*400/12))%256;
		TH1= (65536-(unsigned)(22.1184*400/12))/256;

		ET1 = TRUE;		/* enable time interrupt 1 */
		TR1 = TRUE;		/* open time 1 start now */
	}
	else
	{
		ET1 = FALSE;	/* disable Timer 1 */
		TR1 = FALSE;
		
		EX0 = TRUE;
		MT88E39_DCLK = FALSE;
		MT88E39_DATA_ENABLE = TRUE;/* disable 74LS */
		
//		if(fskDecoder.currentDigits != 0x55 )//&& fskDecoder.currentDigits != 0xaa)
		{
			
#if WITH_UART_DEBUG_PSTN_FSK_DATA
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

		d[fskDecoder.fskIndex+1] = fskDecoder.fskIndex+1;//fskDecoder.currentDigits;
//		uart_tx_2_int_value(CMD_UART_DEBUG, FSK_DATA_RAW_DATA, fskDecoder.currentDigits );
//		if(fskDecoder.fskIndex == 4)
		{
//			pbx_uart_debug(d, 2);
		}
#endif			
		if(fskDecoder.fskIndex == 1)
		{
			fskDecoder.length = fskDecoder.currentDigits;
		}

#if 0
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
		
			if(callerIdData[fskDecoder.currentChannel].digits.length > 2*PHONE_DIGIT_BUFFER_SIZE )
			{
				pos = PHONE_DIGIT_BUFFER_SIZE;
			}
			else
			{
				pos = callerIdData[fskDecoder.currentChannel].digits.length/2;
			}
			
			if( IS_ODD(callerIdData[fskDecoder.currentChannel].digits.length) )
			{
				callerIdData[fskDecoder.currentChannel].digits.value[pos] |= fskDecoder.currentDigits&0x0f;
			}
			else
			{
				callerIdData[fskDecoder.currentChannel].digits.value[pos] = fskDecoder.currentDigits<<4;
			}

			callerIdData[fskDecoder.currentChannel].digits.length++;

		}

		if(fskDecoder.fskIndex == fskDecoder.length)
		{
		}
#endif
			}
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
	totalD++;
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
			
#if WITH_UART_DEBUG_PSTN_FSK_DATA
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

#if WITH_UART_DEBUG_PSTN_FSK_DATA
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
			
				if(callerIdData[fskDecoder.currentChannel].digits.length > 2*PHONE_DIGIT_BUFFER_SIZE )
				{
					pos = PHONE_DIGIT_BUFFER_SIZE;
				}
				else
				{
					pos = callerIdData[fskDecoder.currentChannel].digits.length/2;
				}
				
				if( IS_ODD(callerIdData[fskDecoder.currentChannel].digits.length) )
				{
					callerIdData[fskDecoder.currentChannel].digits.value[pos] |= fskDecoder.currentDigits&0x0f;
				}
				else
				{
					callerIdData[fskDecoder.currentChannel].digits.value[pos] = fskDecoder.currentDigits<<4;
				}

				callerIdData[fskDecoder.currentChannel].digits.length++;

			}

#if 0
			if(fskDecoder.fskIndex == fskDecoder.length)
			{
			}
#endif
			
#if WITH_UART_DEBUG_PSTN_FSK_DATA
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
	totalD++;
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
			
#if WITH_UART_DEBUG_PSTN_FSK_DATA
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
#if WITH_UART_DEBUG_PSTN_FSK_DATA
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
		
			if(callerIdData[fskDecoder.currentChannel].digits.length > 2*PHONE_DIGIT_BUFFER_SIZE )
			{
				pos = PHONE_DIGIT_BUFFER_SIZE;
			}
			else
			{
				pos = callerIdData[fskDecoder.currentChannel].digits.length/2;
			}
			
			if( IS_ODD(callerIdData[fskDecoder.currentChannel].digits.length) )
			{
				callerIdData[fskDecoder.currentChannel].digits.value[pos] |= fskDecoder.currentDigits&0x0f;
			}
			else
			{
				callerIdData[fskDecoder.currentChannel].digits.value[pos] = fskDecoder.currentDigits<<4;
			}

			callerIdData[fskDecoder.currentChannel].digits.length++;

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

void	pbx_isr_uart(void) interrupt 4 using 3
{
	INT8U 	xdata	*buf;
	INT8U	data		lastIndex;
	
	if(RI)
	{
		if(uart.requestInIndex == PBX_VALUE_INVALIDATE)
		{/* no close RI when return */
			CTS = UART_CTS_DISABLE;
		}
		else
		{
			RI=0;
			
			buf = (INT8U xdata *)(&uartRxCmds[uart.requestInIndex]);
			*(buf+ uart.requestBuffersPosition) = SBUF;
			
			if( (*(buf+ uart.requestBuffersPosition) == 0xff) ||((uart.requestBuffersPosition+1)== UART_BUFFER_SIZE) )
			{
				uart.requestBuffersPosition=0;
				lastIndex = uart.requestInIndex;
				
				uart.requestInIndex++;
				if( (uart.requestInIndex == UART_BUFFER_COUNT) )
				{
					uart.requestInIndex = 0;
				}
				
				if( uart.requestInIndex == uart.requestOutIndex )
				{/* block myself */
					uart.requestInIndex = PBX_VALUE_INVALIDATE;
					CTS = UART_CTS_DISABLE;
				}
				
				if( uart.requestOutIndex== PBX_VALUE_INVALIDATE)
				{/* wake up main loop to read data */
					uart.requestOutIndex = lastIndex;
				}
				
			}
			else
			{
				uart.requestBuffersPosition++;
			}
		}
	}
	else 

	if(TI)
	{
		TI=0;
		if( uart.responseOutIndex != PBX_VALUE_INVALIDATE)
		{
			buf = (INT8U xdata *)(&uartTxCmds[uart.responseOutIndex] );

			SBUF= *(buf+uart.responseBuffersPosition);
			
			if( (*(buf+uart.responseBuffersPosition) == PBX_VALUE_INVALIDATE ) )
			{
				lastIndex = uart.responseOutIndex;
				uart.responseBuffersPosition = 0;

				if(uart.responseInIndex == PBX_VALUE_INVALIDATE )
				{/*wakeup main loop as once */
					uart.responseInIndex = lastIndex;
				}

				uart.responseOutIndex++;
				if(uart.responseOutIndex == UART_TX_BUFFER_COUNT)
				{
					uart.responseOutIndex = 0;
				}

				if(uart.responseOutIndex == uart.responseInIndex)
				{/* block myself */
					uart.responseOutIndex = PBX_VALUE_INVALIDATE;
				}

			}
			else
			{		
				uart.responseBuffersPosition++;
			}
		}
	}
}

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

	if(++pbxHwStatus.Counter500ms == 20)
	{
		pbxHwStatus.Counter500ms = 0;
		bool500msFlag = TRUE;
		
		if(++pbxHwStatus.CounterMinutes == 120)
		{/* 60 seconds */
			bool1MinuteFlag = TRUE;
			pbxHwStatus.CounterMinutes =0;
		}
	}
	
}


