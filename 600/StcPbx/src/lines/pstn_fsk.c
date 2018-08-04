/*
* $Id: pstn_fsk.c,v 1.1 2007/07/07 09:33:06 lizhijie Exp $
*/
/* Reenable External Interrupt 0 and disable Timer 1
*/
#define	ENABLE_FSK_DETECT()	\
	do{ EX0 = FALSE;\
		TR1 = TRUE;}while(0)

/* different ISR can not use same Register Bank */
void	timer_isr_03(void) interrupt 3 	/* //using 2 //Timer 1 : INT 3 */
{
	BOOL	errFlag = FALSE;
	INT8U  	pos = 0;
	
//	TL1|=0;//(65536-(unsigned)(22.1184*1000/1.2/12))%256;	/* 1ms */
	TL1 = 0;//(65536-(unsigned)(22.1184*1000/1.2/12))%256;	/* 1ms */
	TH1= 250;//(65536-(unsigned)(22.1184*1000/1.2/12))/256;
	/*1200 bps for FSK ??? */

#if UART_DEBUG						
//	uart_tx_2_int_value(CMD_UART_DEBUG, 8, ldbitcnt );
#endif

	if(currentCallerIdChannel == PBX_VALUE_INVALIDATE)
	{
		ENABLE_FSK_DETECT();
		return;
	}

	if(ldbitcnt==0)/* this is first bit : start bit */
	{
		if(INT0==0)/* 0 : is in interrupt ??? */	
		{/* start bit must be 0 , refer specs. p.12 */
			ldbitcnt++;
		}
		else
			errFlag = TRUE;
	}
	else
	{
#if UART_DEBUG						
		if(ldbitcnt==8)/*this is stop bit */
		{
			uart_tx_2_int_value(CMD_UART_DEBUG, 6, ldbytebuf );
		}
#endif
		if(ldbitcnt==9)/*this is stop bit */
		{
			if(INT0)	
			{/*  stop bit must be 1 */
				ldbitcnt = 0;
				
				if(ldfirstbyte)	
				{/* 是首字节*/
					if(ldbytebuf==0)
					{/* first byte must be 0 */
						ENABLE_FSK_DETECT();
						ldfirstbyte= FALSE;
						callerIdData[currentCallerIdChannel].length = 0;
					}
					else
						errFlag = TRUE;/*/首字节错误*/
				}
				else
				{
					if((ldbytebuf&0x0f)!=0x0f)
					{/* last byte must be 0x0F */
						if((ldbytebuf>>4)+(ldbytebuf&0x0f)==0x10)
						{	//数据正确
							if(callerIdData[currentCallerIdChannel].length > 2*PHONE_DIGIT_BUFFER_SIZE )
							{
								pos = PHONE_DIGIT_BUFFER_SIZE;
							}
							else
							{
								pos = callerIdData[currentCallerIdChannel].length/2;
							}
							
							if( IS_ODD(callerIdData[currentCallerIdChannel].length) )
							{
								callerIdData[currentCallerIdChannel].value[pos] |= ldbytebuf&0x0f;
							}
							else
							{
								callerIdData[currentCallerIdChannel].value[pos] = ldbytebuf<<4;
							}

							callerIdData[currentCallerIdChannel].length++;
							
							ENABLE_FSK_DETECT();
						}
						else
							errFlag = TRUE;	
					}
					else
					{
						ldfirstbyte = TRUE;
						errFlag = TRUE;	//接收结束
					}
				}
			}
			else
				errFlag = TRUE;		//结束位错误
		}
		else	/* data bit */
		{
			ldbitcnt++;
			ldbytebuf>>=1;
			if(INT0)
				ldbytebuf|= 0x80;
		}
	}
	
	if(errFlag)/* error */
	{
		ET1 = FALSE;	/* disable Timer 1 */
		ENABLE_FSK_DETECT();
	}

}

