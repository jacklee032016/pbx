/*
* $Id: pbx_utils.c,v 1.10 2007/07/25 17:57:12 lizhijie Exp $
*/

#pragma	ot(9,speed)
//#pragma large
#pragma small

#include <REGSTCRD+.H>
#include "function_declarations.h"

/* for IP_PREFIX, EXT phone number and special service */
void ipdigit_2_pbxdigit(INT8U xdata *pbxData, INT8U xdata *ipDigit, INT8U phoneLength )
{
	INT8U xdata i;

	for(i= 0; i< phoneLength/2; i++)
	{
		*(pbxData +i ) = (PBX_GET_LOW_NIBBLE( (*(ipDigit+i*2)) )<<4)|PBX_GET_LOW_NIBBLE( (*(ipDigit+i*2+1)) );
	}

	if( IS_ODD(phoneLength) )
		*(pbxData+phoneLength/2 ) = (PBX_GET_LOW_NIBBLE( (*(ipDigit+phoneLength-1)) )<<4)|0x0F;
}

void pbxdigit_2_ipdigit(INT8U xdata *ipData, INT8U xdata *pbxDigit, INT8U phoneLength )
{
	INT8U xdata i;

	for(i= 0; i< phoneLength/2; i++)
	{
		*(ipData + i*2) = PBX_GET_HIGH_NIBBLE( (*(pbxDigit+i)) );
		*(ipData + i*2+1) = PBX_GET_LOW_NIBBLE( (*(pbxDigit+i)) );
	}

	if(IS_ODD(phoneLength) )
		*(ipData+phoneLength-1) = PBX_GET_HIGH_NIBBLE( (*(pbxDigit+i)) );
}

/* copy digits from odd position of srcDigits */
void pbxdigit_odd_copy(INT8U xdata *destDigits, INT8U xdata *srcDigits,INT8U length )
{
	INT8U xdata i, tmp;

	for(i= 0; i< length; i++)
	{
		if(IS_ODD(i+1) )
		{
			tmp = (PBX_GET_LOW_NIBBLE( srcDigits[(i+1)/2] ) <<4);
			*(destDigits + i/2) = tmp;
		}
		else
		{
			tmp = PBX_GET_HIGH_NIBBLE( srcDigits[(i+1)/2]) ;
			*(destDigits + i/2) |= tmp;
		}
		
	}

}



void pbx_x_str_copy(INT8U *dest, INT8U *src, INT8U length )
{
	INT8U data i;

	for(i= 0; i< length; i++)
	{
		*(dest + i) = *(src+i);
	}
}

#if UART_DEBUG
void pbx_uart_debug(INT8U xdata *buf, INT8U length)
{
	pbx_x_str_copy(uartTxCmds[uartTxInIndex].value, buf , length);
	uart_tx_cmd(CMD_UART_DEBUG, length);
}

void uart_debug_int_value(INT8U type, INT8U value)
{
	uart_tx_2_int_value(CMD_UART_DEBUG, type, value);
}

void uart_debug_2_int_value(INT8U type, INT8U value1, INT8U value2)
{
	uartTxCmds[uartTxInIndex].value[0] = type;
	uartTxCmds[uartTxInIndex].value[1] = value1;
	uartTxCmds[uartTxInIndex].value[2] = value2;
	uart_tx_cmd(CMD_UART_DEBUG, 3);
}

void uart_debug_3_int_value(INT8U type, INT8U value1, INT8U value2, INT8U value3)
{
	uartTxCmds[uartTxInIndex].value[0] = type;
	uartTxCmds[uartTxInIndex].value[1] = value1;
	uartTxCmds[uartTxInIndex].value[2] = value2;
	uartTxCmds[uartTxInIndex].value[3] = value3;
	uart_tx_cmd(CMD_UART_DEBUG, 4);
}

#endif

#if 0
INT8U pbx_compare_digits(INT8U xdata *dest, INT8U xdata *src, INT8U length)
{
	INT8U	xdata i;
	INT8U	xdata index = 0;

	for(i=0; i<length/2; i++)
	{
		if( PBX_GET_HIGH_NIBBLE(*(dest+i)) == PBX_GET_HIGH_NIBBLE(*(src+i)) )
			index = i*2;
		else 
			break;
		if( PBX_GET_LOW_NIBBLE(*(dest+i)) == PBX_GET_LOW_NIBBLE(*(src+i)) )
			index = i*2+1;
		else 
			break;
	}

	if((length%2)!= 0 )
	{
		if( PBX_GET_HIGH_NIBBLE(*(dest+length/2+1)) == PBX_GET_HIGH_NIBBLE(*(src+length/2+1)) )
			index ++;		
	}

	if(index == length)
		return PHONE_OK;
	else if(index == 0)
		return PHONE_ERROR;
	return PHONE_INCOMPLATE;
}
#endif

