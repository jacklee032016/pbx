/*
* $Id: eepromFm24c08.c,v 1.1 2007/07/30 07:11:56 lizhijie Exp $
*/

#pragma	ot(9,speed)
#pragma small

#include <REGSTCRD+.H>
#include "declarations.h"


/* SCL is high and SDA from high to low */
static void fm2408_start_bit(void)
{
	FM2408_SCLK = TRUE;
	FM2408_SDATA = TRUE;
	DELAY5US();
	FM2408_SDATA = FALSE;
  	DELAY5US();
	FM2408_SCLK = FALSE;
  	DELAY5US();
}

/* SCL is high and SDA from low to high */
static void fm2408_stop_bit(void)
{
	FM2408_SDATA = FALSE;
	DELAY5US();
	FM2408_SCLK = TRUE;
	DELAY5US();
	FM2408_SDATA = TRUE;
	DELAY5US();
	FM2408_SCLK = FALSE;
}


static INT8U fm2408_read_raw_byte(void)
{
	INT8U data i, value = 0;
	INT8U data  res;
	
	for(i=0; i< 8; i++)
	{
//		DELAY5US();
		FM2408_SCLK = TRUE;
		DELAY5US();
		res = FM2408_SDATA;
		value = SET_BIT(value,1)|res;
		FM2408_SCLK = FALSE;
		DELAY5US();
	}

	return value;
}


static void fm2408_write_raw_byte(INT8U value)
{
	INT8U data i;

	for(i=0; i< 8; i++)
	{
		FM2408_SDATA = GET_BIT(value, (7-i));
		FM2408_SCLK = TRUE;
		DELAY5US();
		FM2408_SCLK = FALSE;
		DELAY5US();
	}
}

/* when data are output to EEPROM, host need RX ACK from EEPROM */
static BOOL fm2408_read_ack_bit(void)
{
	BOOL result;
	FM2408_SDATA = TRUE;
	FM2408_SCLK = TRUE;
	DELAY5US();
	result = FM2408_SDATA;	/* SDA should be low level */
	FM2408_SCLK = FALSE;
	DELAY5US();
	
	return (!result);
}

/* After host has rx 8 bit data, it must send out ACK, refer page7. figure 3*/
static void fm2408_send_ack_bit(void)
{
	FM2408_SDATA = FALSE;
	DELAY5US();
	FM2408_SCLK = TRUE;/* high*/
	DELAY5US();
	FM2408_SCLK = FALSE;/* low*/
	DELAY5US();
	FM2408_SDATA = TRUE;
}

static void _fm2408_write_address(INT8U pageNo)
{
	INT16U data address = pageNo*FM2408_PAGE_SIZE;
	INT8U data  addr;
	
	FM2408_SDATA = TRUE;
	fm2408_start_bit();

	addr = (address>>8);
	addr = SET_BIT(addr, 1);/* in order to get Page address bit : p0, p1*/
	addr = (addr&0X06)|FM24C08_DEVICE_ADDRESS;

	fm2408_write_raw_byte(addr);
#if 1	
	fm2408_read_ack_bit();
#else
	if( fm2408_read_ack_bit() == FALSE)
	{
		FM2408_SDATA = FALSE;
		return FALSE;
	}
#endif

	addr = (address&0xFF);
	fm2408_write_raw_byte(addr);
#if 1	
	fm2408_read_ack_bit();
#else
	if( fm2408_read_ack_bit() == FALSE)
	{
		FM2408_SDATA = FALSE;
		return FALSE;
	}
#endif

}

#if 0
INT8U eeprom_read_byte(INT16U address)
{
	INT8U data value, addr;

//	FM2408_SDATA = TRUE;
	fm2408_start_bit();

	addr = (address>>8);
	addr = SET_BIT(addr, 1);/* in order to get Page address bit : p0, p1*/
	addr = (addr&0X06)|FM24C08_DEVICE_ADDRESS;

	fm2408_write_raw_byte(addr);
	if( fm2408_read_ack_bit() == FALSE)
	{
		FM2408_SDATA = FALSE;
		return 0x33;
	}

	addr = (address&0xFF);
	fm2408_write_raw_byte(addr);
	if( fm2408_read_ack_bit() == FALSE)
	{
		FM2408_SDATA = FALSE;
		return 0x22;
	}

	fm2408_start_bit();

	addr = (address>>8);
	addr = SET_BIT(addr, 1);/* in order to get Page address bit : p0, p1*/
	addr = (addr&0X06)|FM24C08_DEVICE_ADDRESS|FM24C08_READ_CMD;

	fm2408_write_raw_byte(addr);
	if( fm2408_read_ack_bit() == FALSE)
	{
		FM2408_SDATA = FALSE;
		return 0x11;
	}

	value = fm2408_read_raw_byte();
//	fm2408_send_ack_bit();

	FM2408_SDATA = TRUE;
	fm2408_stop_bit();

	return value;
}

BOOL eeprom_write_byte(INT8U value, INT16U address)
{
	INT8U	data addr;
	
	FM2408_SDATA = TRUE;
	fm2408_start_bit();
	
	addr = (address>>8);
	addr = SET_BIT(addr, 1);/* in order to get Page address bit : p0, p1*/
	addr = (addr&0X06)|FM24C08_DEVICE_ADDRESS|FM24C08_WRITE_CMD;

	fm2408_write_raw_byte(addr);
	if( fm2408_read_ack_bit() == FALSE)
	{
		FM2408_SDATA = FALSE;
		return FALSE;
	}

	addr = (address&0xFF);
	fm2408_write_raw_byte(addr);
	if( fm2408_read_ack_bit() == FALSE)
	{
		FM2408_SDATA = FALSE;
		return FALSE;
	}

	fm2408_write_raw_byte( value);
	if( fm2408_read_ack_bit() == FALSE)
	{
		FM2408_SDATA = FALSE;
		return FALSE;
	}

	fm2408_stop_bit();

	return TRUE;
}
#endif


BOOL eeprom_write_page(INT8U xdata *buf, INT8U pageNo)
{
	INT8U	data i;
	
	_fm2408_write_address(pageNo);
	
	for(i=0; i< FM2408_PAGE_SIZE; i++)
	{
		fm2408_write_raw_byte(buf[i]);
#if 1
		fm2408_read_ack_bit();
#else
		if(fm2408_read_ack_bit()==FALSE)
			return FALSE;
#endif		
	}

	fm2408_stop_bit();

	return TRUE;
}


BOOL eeprom_read_buffer(INT8U xdata *buf, INT8U pageNo, INT8U length)
{
	INT8U	data i;
	INT16U	data address = pageNo*FM2408_PAGE_SIZE;
	
	/* peseudo write operation */
	_fm2408_write_address(pageNo);

	/* begin to continue read */
	fm2408_start_bit();
	i = (address>>8);
	i = SET_BIT(i, 1);/* in order to get Page address bit : p0, p1*/
	i = (i&0X06)|FM24C08_DEVICE_ADDRESS|FM24C08_READ_CMD;

	fm2408_write_raw_byte(i);
	if( fm2408_read_ack_bit() == FALSE)
	{
		FM2408_SDATA = FALSE;
		return FALSE;
	}

	for(i=0; i< length; i++)
	{
		buf[i] = fm2408_read_raw_byte();
		if(i< (length-1) )
			fm2408_send_ack_bit();
	}

	FM2408_SDATA = FALSE;
	fm2408_stop_bit();

	pbx_mdelay(10);
	return TRUE;
}

/*address must be in the border of fm24c08 page */
BOOL eeprom_write_buffer(INT8U xdata *buf, INT8U pageNo, INT8U length)
{
	INT8U	data i;//, pos;
	INT8U 	data pageCount;

	pageCount = length/FM2408_PAGE_SIZE;
	if(length%FM2408_PAGE_SIZE!= 0)
		pageCount++;

	for(i=0; i<pageCount;i++)
	{
		if(eeprom_write_page(buf+ i*FM2408_PAGE_SIZE, pageNo+i )== FALSE)
			return FALSE;
		pbx_mdelay(10);
	}
#if 0	
	pos = pageCount*FM2408_PAGE_SIZE;
	beginPage = (beginPage+i)*FM2408_PAGE_SIZE;
	pageCount = length%FM2408_PAGE_SIZE;
	for(i=0;i<pageCount;i++)
	{
		if(eeprom_write_byte(buf[pos+i], beginPage+i)==FALSE)
			return FALSE;
	}

	pbx_mdelay(10);
#endif	
	return TRUE;
}

