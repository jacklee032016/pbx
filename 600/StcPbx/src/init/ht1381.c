/*
* $Id: ht1381.c,v 1.1 2007/07/25 17:55:22 lizhijie Exp $
*/

#pragma	ot(9,speed)
#pragma small

#include <REGSTCRD+.H>
#include "function_declarations.h"


#define		HT1381_BIT_RW				0
#define		HT1381_BIT_ADDRESS		1

#define		HT1381_BIT_RW_READ		1
#define		HT1381_BIT_RW_WRITE		0

#define		HT1381_CMD_WP_ENABLE	0x8E
#define		HT1381_CMD_WP_DISABLE	0x0E
#define		HT1381_CMD_OSC_DISABLE	0x80
#define		HT1381_CMD_OSC_ENABLE	0x00

/*read can be only execute after write */
static INT8U ht1381_read_raw_byte(void)
{
	INT8U data i, value = 0;
	INT8U data  res;
	
//	HT1381_SCLK = TRUE;
	for(i=0; i< 8; i++)
	{/* data out on the falling edge of SCLK  */
		HT1381_SCLK = TRUE;
		DELAY5US();
		NOP();
		NOP();
		HT1381_SCLK = FALSE;
		DELAY5US();
		NOP();
		NOP();
		res = HT1381_DATA;
		value |= SET_BIT(res,i);
	}
	
	return value;
}


static void ht1381_write_raw_byte(INT8U value)
{
	INT8U data i;

	for(i=0; i< 8; i++)
	{/* data enter on the rising edge of SCLK */
		HT1381_SCLK = FALSE;
		HT1381_DATA = GET_BIT(value, i );
		DELAY5US();
		NOP();
		NOP();
		HT1381_SCLK = TRUE;
		DELAY5US();
		NOP();
		NOP();
		HT1381_SCLK = FALSE;
	}
}

/* disable Write-Protect and enable Oscillator, datasheet p.8 */
void	ht1381_enable(void)
{
	/* disable WP */
	HT1381_RESET_ON();
	ht1381_write_raw_byte(HT1381_CMD_WP_ENABLE);
	ht1381_write_raw_byte(HT1381_CMD_WP_DISABLE);
	HT1381_RESET_OFF();

	/* enable OSC */
	HT1381_RESET_ON();
	ht1381_write_raw_byte(HT1381_CMD_OSC_DISABLE);
	ht1381_write_raw_byte(HT1381_CMD_OSC_ENABLE);
	HT1381_RESET_OFF();
}

static INT8U ht1381_read_data_byte(INT8U address)
{
	INT8U data 	value = 0;
	
	HT1381_RESET_ON();
	value = SET_BIT(address,HT1381_BIT_ADDRESS)|SET_BIT(HT1381_BIT_RW_READ, HT1381_BIT_RW);
	ht1381_write_raw_byte(value);

	value = ht1381_read_raw_byte();
	HT1381_RESET_OFF();
	return value;
}

static void ht1381_write_data_byte(INT8U address, INT8U value)
{
	HT1381_RESET_ON();
	address = SET_BIT(address,HT1381_BIT_ADDRESS)|SET_BIT(HT1381_BIT_RW_WRITE, HT1381_BIT_RW);
	ht1381_write_raw_byte(address);

	ht1381_write_raw_byte(value);

	HT1381_RESET_OFF();
}


void	pbx_systime_write(void)
{
	INT8U data address = 0, value;
#if 0	
	value = (SET_BIT(pbxHwStatus.sysTime.second/10, 4)|(pbxHwStatus.sysTime.second%10) )&0x7F;
#else
	value = pbxHwStatus.sysTime.second;
#endif
	ht1381_write_data_byte(address, value);
	address++;

#if 0
	value = (SET_BIT(pbxHwStatus.sysTime.minute/10, 4)|(pbxHwStatus.sysTime.minute%10) )&0x7F;
#else
	value = pbxHwStatus.sysTime.minute;
#endif
	ht1381_write_data_byte(address, value);
	address++;

#if 0
	value = (SET_BIT(pbxHwStatus.sysTime.hour/10, 4)|(pbxHwStatus.sysTime.hour%10))&0x1F;
#else
	value = pbxHwStatus.sysTime.hour;
#endif
	ht1381_write_data_byte(address, value);
	address++;

#if 0
	value = (SET_BIT(pbxHwStatus.sysTime.date/10, 4)|(pbxHwStatus.sysTime.date%10) )&0x3F;
#else
	value = pbxHwStatus.sysTime.date;
#endif
	ht1381_write_data_byte(address, value);
	address++;

#if 0
	value = (SET_BIT(pbxHwStatus.sysTime.month/10, 4)|(pbxHwStatus.sysTime.month%10))&0x1F;
#else
	value = pbxHwStatus.sysTime.month;
#endif
	ht1381_write_data_byte(address, value);
	address++;

#if 0
	value = (pbxHwStatus.sysTime.day%10) &0x0F;
#else
	value = pbxHwStatus.sysTime.day;
#endif
	ht1381_write_data_byte(address, value);
	address++;

#if 0
	value = SET_BIT(pbxHwStatus.sysTime.year/10, 4)|(pbxHwStatus.sysTime.year%10) ;
#else
	value = pbxHwStatus.sysTime.year;
#endif
	ht1381_write_data_byte(address, value);
	address++;

}

void	pbx_systime_reload(void)
{
	INT8U data address = 0, value;

	value = ht1381_read_data_byte(address);
#if 0//UART_DEBUG
	uart_debug_2_int_value(UART_DEBUG_STSTIME, value, address);
	pbxHwStatus.sysTime.second = PBX_GET_HIGH_NIBBLE(value)*10+PBX_GET_LOW_NIBBLE(value);
#endif
	pbxHwStatus.sysTime.second = value;
	address++;

	value = ht1381_read_data_byte(address);
#if 0//UART_DEBUG
	uart_debug_2_int_value(UART_DEBUG_STSTIME, value, address);
	pbxHwStatus.sysTime.minute = PBX_GET_HIGH_NIBBLE(value)*10+PBX_GET_LOW_NIBBLE(value);
#endif
	pbxHwStatus.sysTime.minute = value;
	address++;

	value = ht1381_read_data_byte(address);
#if 0//UART_DEBUG
	uart_debug_2_int_value(UART_DEBUG_STSTIME, value, address);
	pbxHwStatus.sysTime.hour = PBX_GET_HIGH_NIBBLE(value)*10+PBX_GET_LOW_NIBBLE(value);
#endif
	pbxHwStatus.sysTime.hour = value;
	address++;

	value = ht1381_read_data_byte(address);
#if UART_DEBUG
	uart_debug_2_int_value(UART_DEBUG_STSTIME, value, address);
	pbxHwStatus.sysTime.date = PBX_GET_HIGH_NIBBLE(value)*10+PBX_GET_LOW_NIBBLE(value);
#endif
	pbxHwStatus.sysTime.date = value;
	address++;

	value = ht1381_read_data_byte(address);
#if 0//UART_DEBUG
	uart_debug_2_int_value(UART_DEBUG_STSTIME, value, address);
	pbxHwStatus.sysTime.month = PBX_GET_HIGH_NIBBLE(value)*10+PBX_GET_LOW_NIBBLE(value);
#endif
	pbxHwStatus.sysTime.month = value;
	address++;

	value = ht1381_read_data_byte(address);
#if 0//UART_DEBUG
	uart_debug_2_int_value(UART_DEBUG_STSTIME, value, address);
	pbxHwStatus.sysTime.day = PBX_GET_LOW_NIBBLE(value);
#endif
	pbxHwStatus.sysTime.day = value;
	address++;

	value = ht1381_read_data_byte(address);
#if 0//UART_DEBUG
	uart_debug_2_int_value(UART_DEBUG_STSTIME, value, address);
	pbxHwStatus.sysTime.year = PBX_GET_HIGH_NIBBLE(value)*10+PBX_GET_LOW_NIBBLE(value);
#endif
	pbxHwStatus.sysTime.year = value;
	address++;

}


