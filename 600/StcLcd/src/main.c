/*
* $Id: main.c,v 1.3 2007/06/01 18:03:24 lizhijie Exp $
*/

#pragma	ot(9,speed)
#pragma small

#include "lcdDeclarations.h"
#include "globalVaribles.h"

/*延时程序，以MS为单位，由参数决定时间（xms），1ms误差2us*/
void delay_ms(INT16U ms)
{
	INT8U abc;
	while(ms > 0 )
	{
		abc = 220;
		for(ms--; abc>0; abc--)
			NOP();
	}
}

void x_str_copy(INT8U *dest, INT8U *src, INT8U length )
{
	INT8U data i;

	for(i= 0; i< length; i++)
	{
		*(dest + i) = *(src+i);
	}
}


int	main(void)
{
	lcdInit();

	TR0 = STC51_PIN_ENABLED;

	lcdRefreshScreen();

	while(1)
	{   
		lcdTimerTask();
		lcdUartRxResponse();
		lcdStatemachineHandle();
	}
}

