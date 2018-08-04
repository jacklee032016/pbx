#include"receive_h.h"
#pragma ot(9,speed)


bit	command5a_process()		//提取时间
{
	INT8U i,j;
	i=found_send_buf();
	if(i!=0xff)
		{
		uartTxCmds[i][0]=0xdc;
		if(++uartCmdSeq==0xff)
			uartCmdSeq++;
		uartTxCmds[i][1]=uartCmdSeq;
		for(j=0;j<7;j++)
			uartTxCmds[i][j+2]=pbxCurrentTime[j];
		uartTxCmds[i][j+2]=0xff;
		busy_send_buf(i);
		return 1;
		}
	else
		return 0;
}

