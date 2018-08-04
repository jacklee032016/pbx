/*
* $Id: pbx_uart_rx_self_test.c,v 1.1 2007/05/11 20:14:13 lizhijie Exp $
*/

/* 0x18 : begin to self test */
BOOL ip_pbx_self_test_begin(void)
{
	INT8U	bufIndex, end = 0;
	bit	flag=0;
	bufIndex = found_send_buf();

	if(bufIndex!=0xff)
	{
		if(columnFrees==0xff)
		{
			if(decodingchannel_allocate(15))	//12检测PCM第0通道
			{
				uartTxCmds[bufIndex][0]=CMD_SELFTEST_RESULT;//0x98;
				uartTxCmds[bufIndex][2]=0;
				uartTxCmds[bufIndex][3]=0x55;	//首次发送DTMF标志
				columnFrees=0;		//检测期间屏蔽IP通道
				status[15][0]=113;
				tel_jishi[15]=100;
				flag=1;
				
				end = 4;
			}
		}
		
		if(flag==0)
		{
			uartTxCmds[bufIndex][0]=CMD_SELFTEST_BUSY;//0x99;
			end = 2;
		}
		
		if(++uartCmdSeq==0xff)
			uartCmdSeq++;
		uartTxCmds[bufIndex][1]=uartCmdSeq;
		uartTxCmds[bufIndex][end]=0xff;
		busy_send_buf(bufIndex);
		
		return	1;
	}

	else
		return	0;
}


