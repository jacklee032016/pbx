/*
* $Id: ascii12x16.c,v 1.4 2007/05/27 09:18:10 lizhijie Exp $
*/

#pragma	ot(9,speed)
#pragma small

#include "lcdDeclarations.h"

/*
* for 12x16 ASCII : 192 bits/char, 24 bytes/Char, 
*/
INT8U code ascii12x16[95][24]=
{
	{
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
	},/* " " 0*/
	{
		0x00,0x00,0x00,0x00,0x00,0x38,0xF8,0xF8,0x38,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x30,0x33,0x33,0x30,0x00,0x00,0x00
	},/* "!" 1*/
	{
		0x00,0x00,0x10,0x18,0x0C,0x0E,0x16,0x1A,0x0C,0x0E,0x06,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
	},/* """ 2*/
	{
		0x00,0x40,0x40,0xF8,0xF8,0x40,0x40,0x40,0x40,0xF8,0xF8,0x40,0x00,0x04,0x3F,0x3F,0x05,0x04,0x04,0x04,0x3F,0x3F,0x05,0x04
	},/*"#" 3*/
	{
		0x00,0x00,0x70,0x70,0xF8,0xD8,0xFC,0xFC,0x38,0x38,0x30,0x00,0x00,0x00,0x18,0x38,0x38,0x20,0xFF,0xFF,0x3F,0x3E,0x1C,0x00
	},/* "$" 4*/
	{
		0xF0,0xF8,0x08,0x08,0xF8,0xF0,0x80,0xE0,0x78,0x18,0x08,0x00,0x00,0x01,0x21,0x39,0x1D,0x0F,0x1F,0x3F,0x21,0x21,0x3F,0x1E
	},/* "%" 5*/
	{
		0x00,0x00,0xF0,0xF8,0x88,0xF8,0xF8,0x70,0x00,0x00,0x00,0x00,0x1C,0x1E,0x3F,0x23,0x27,0x2E,0x3C,0x3D,0x2F,0x23,0x31,0x10
	},/* "&" 6*/
	{
		0x00,0x00,0x12,0x1E,0x0E,0x06,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
	},/* "'" 7*/
	{
		0x00,0x00,0x00,0x00,0x00,0xC0,0xF0,0xF8,0x1C,0x0C,0x06,0x02,0x00,0x00,0x00,0x00,0x00,0x03,0x0F,0x1F,0x38,0x30,0x60,0x40
	},/* "(" 8*/
	{
		0x00,0x02,0x06,0x0C,0x1C,0xF8,0xF0,0xC0,0x00,0x00,0x00,0x00,0x00,0x40,0x60,0x30,0x38,0x1F,0x0F,0x03,0x00,0x00,0x00,0x00
	},/*")"9*/
	{
		0x00,0x40,0x40,0x40,0xC0,0x80,0xF0,0xF0,0x80,0xC0,0x40,0x40,0x00,0x02,0x02,0x02,0x03,0x01,0x0F,0x0F,0x01,0x03,0x02,0x02
	},/*"*"10*/
	{
		0x00,0x00,0x00,0x00,0x00,0x00,0xF0,0xF0,0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x01,0x01,0x01,0x1F,0x1F,0x01,0x01,0x01,0x01
	},/*"+"11*/
	{
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x90,0xF0,0x70,0x30,0x00,0x00,0x00,0x00,0x00,0x00
	},/*","12*/
	{
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01
	},/*"-"13*/
	{
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x30,0x30,0x30,0x30,0x00,0x00,0x00,0x00,0x00,0x00
	},/*"."14*/
	{
		0x00,0x00,0x00,0x00,0x00,0x00,0x80,0xC0,0x70,0x38,0x0C,0x04,0x00,0x40,0x60,0x38,0x1C,0x07,0x03,0x01,0x00,0x00,0x00,0x00
	},/*"/"15*/
	{
		0x00,0xC0,0xF0,0xF0,0x38,0x18,0x08,0x08,0x38,0xF0,0xF0,0xC0,0x00,0x07,0x1F,0x1F,0x38,0x30,0x20,0x20,0x38,0x1F,0x1F,0x07
	},/* "0"16*/
	{
		0x00,0x00,0x10,0x10,0x10,0xF0,0xF8,0xF8,0x00,0x00,0x00,0x00,0x00,0x00,0x20,0x20,0x20,0x3F,0x3F,0x3F,0x20,0x20,0x20,0x00
	},/*"1"17*/
	{
		0x00,0x70,0x70,0x58,0x08,0x08,0x08,0x18,0xF8,0xF0,0x70,0x00,0x00,0x20,0x30,0x38,0x2C,0x24,0x26,0x23,0x21,0x38,0x38,0x00
	},/*"2"18*/
	{
		0x00,0x30,0x38,0x38,0x88,0x88,0xD8,0xF8,0x78,0x30,0x00,0x00,0x00,0x18,0x38,0x38,0x20,0x20,0x20,0x31,0x3F,0x1F,0x0E,0x00
	},/*"3"19*/
	{
		0x00,0x00,0x00,0x80,0xC0,0x60,0x30,0xF8,0xF8,0xF8,0x00,0x00,0x00,0x06,0x07,0x05,0x04,0x24,0x24,0x3F,0x3F,0x3F,0x24,0x24
	},/*"4"20*/
	{
		0x00,0x00,0xF8,0xF8,0x88,0x88,0x88,0x88,0x88,0x08,0x08,0x00,0x00,0x18,0x19,0x39,0x21,0x20,0x20,0x31,0x3F,0x1F,0x0F,0x00
	},/*"5"21*/
	{
		0x00,0xC0,0xE0,0xF0,0xB8,0x98,0x88,0x88,0x98,0x98,0x10,0x00,0x00,0x0F,0x1F,0x1F,0x31,0x20,0x20,0x20,0x31,0x3F,0x1F,0x0E
	},/*"6"22*/
	{
		0x00,0x00,0x30,0x38,0x18,0x08,0x08,0x88,0xE8,0x78,0x18,0x08,0x00,0x00,0x00,0x00,0x00,0x38,0x3E,0x3F,0x01,0x00,0x00,0x00
	},/*"7"23*/
	{
		0x00,0x70,0xF0,0xF8,0x98,0x88,0x88,0x08,0x98,0xF8,0x70,0x70,0x00,0x0C,0x1E,0x3F,0x31,0x21,0x21,0x21,0x33,0x3F,0x1E,0x0C
	},/*"8"24*/
	{
		0x00,0xE0,0xF0,0xF8,0x18,0x08,0x08,0x08,0x18,0xF0,0xF0,0xE0,0x00,0x01,0x11,0x33,0x33,0x22,0x22,0x32,0x3B,0x1F,0x0F,0x07
	},/*"9"25*/
	{
		0x00,0x00,0x00,0x00,0x00,0xC0,0xC0,0xC0,0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x30,0x30,0x30,0x30,0x00,0x00,0x00
	},/*":"26*/
	{
		0x00,0x00,0x00,0x00,0x00,0x80,0x80,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xA0,0xE0,0x60,0x00,0x00,0x00,0x00
	},/*";"27*/
	{
		0x00,0x00,0x00,0x00,0x80,0xC0,0x40,0x60,0x30,0x10,0x18,0x08,0x00,0x00,0x01,0x01,0x03,0x06,0x0C,0x08,0x18,0x30,0x20,0x20
	},/*"<"28*/
	{
		0x00,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x00,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04
	},/*"="29*/
	{
		0x00,0x00,0x08,0x18,0x10,0x30,0x60,0x40,0xC0,0x80,0x00,0x00,0x00,0x00,0x20,0x20,0x30,0x18,0x08,0x0C,0x06,0x03,0x01,0x01
	},/*">"30*/
	{
		0x00,0x60,0x70,0x58,0x08,0x08,0x08,0x08,0x98,0xF8,0xF0,0x70,0x00,0x00,0x00,0x00,0x30,0x36,0x37,0x31,0x01,0x00,0x00,0x00
	},/*"?"31*/
	{
		0x00,0xC0,0xE0,0xF0,0xB8,0xD8,0x68,0x28,0xE8,0xF8,0x78,0xF0,0x00,0x07,0x0F,0x1F,0x3F,0x37,0x24,0x26,0x27,0x27,0x36,0x1B
	},/*"@"32*/
	{
		0x00,0x00,0x00,0x80,0xF0,0x78,0x78,0xF8,0xE0,0x80,0x00,0x00,0x20,0x30,0x3E,0x3F,0x23,0x02,0x02,0x23,0x2F,0x3F,0x3C,0x30
	},/*"A"33*/
	{
		0x08,0xF8,0xF8,0xF8,0x88,0x88,0x88,0xC8,0xF8,0x78,0x30,0x00,0x20,0x3F,0x3F,0x3F,0x20,0x20,0x20,0x20,0x31,0x3F,0x1F,0x0E
	},/*"B"34*/
	{
		0x00,0xC0,0xE0,0xF0,0x38,0x18,0x08,0x08,0x08,0x18,0x38,0x38,0x00,0x07,0x1F,0x1F,0x38,0x30,0x20,0x20,0x20,0x30,0x18,0x08
	},/*"C"35*/
	{
		0x08,0xF8,0xF8,0xF8,0x08,0x08,0x08,0x18,0x38,0xF0,0xE0,0xC0,0x20,0x3F,0x3F,0x3F,0x20,0x20,0x20,0x30,0x38,0x1F,0x0F,0x07
	},/*"D"36*/
	{
		0x08,0xF8,0xF8,0xF8,0x88,0x88,0x88,0xE8,0xE8,0x08,0x18,0x10,0x20,0x3F,0x3F,0x3F,0x20,0x20,0x20,0x23,0x23,0x30,0x38,0x08
	},/*"E"37*/
	{
		0x08,0xF8,0xF8,0xF8,0x88,0x88,0x88,0xE8,0xE8,0x08,0x18,0x10,0x20,0x3F,0x3F,0x3F,0x20,0x00,0x00,0x03,0x03,0x00,0x00,0x00
	},/*"F"38*/
	{
		0x00,0xC0,0xE0,0xF0,0x38,0x18,0x08,0x08,0x18,0x38,0x38,0x00,0x00,0x07,0x0F,0x1F,0x38,0x20,0x22,0x22,0x3E,0x1E,0x1E,0x02
	},/*"G"39*/
	{
		0x08,0xF8,0xF8,0xF8,0x08,0x00,0x00,0x00,0x08,0xF8,0xF8,0xF8,0x20,0x3F,0x3F,0x3F,0x21,0x01,0x01,0x01,0x21,0x3F,0x3F,0x3F
	},/*"H"40*/
	{
		0x00,0x00,0x08,0x08,0x08,0xF8,0xF8,0xF8,0x08,0x08,0x08,0x00,0x00,0x00,0x20,0x20,0x20,0x3F,0x3F,0x3F,0x20,0x20,0x20,0x00
	},/*"I"41*/
	{
		0x00,0x00,0x00,0x00,0x08,0x08,0x08,0xF8,0xF8,0xF8,0x08,0x08,0x00,0x40,0xC0,0xC0,0x80,0x80,0x80,0xFF,0x7F,0x7F,0x00,0x00
	},/*"J"42*/
	{
		0x08,0xF8,0xF8,0xF8,0x88,0x80,0xC0,0x68,0x38,0x18,0x08,0x08,0x20,0x3F,0x3F,0x3F,0x21,0x01,0x03,0x07,0x2E,0x3C,0x30,0x20
	},/*"K"43*/
	{
		0x08,0xF8,0xF8,0xF8,0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x20,0x3F,0x3F,0x3F,0x20,0x20,0x20,0x20,0x20,0x30,0x38,0x18
	},/*"L"44*/
	{
		0x08,0xF8,0xF8,0xF8,0xF8,0xC0,0x00,0xC0,0xF8,0xF8,0xF8,0xF8,0x20,0x3F,0x3F,0x21,0x0F,0x3F,0x3E,0x0F,0x21,0x3F,0x3F,0x3F
	},/*"M"45*/
	{
		0x08,0xF8,0xF8,0x38,0x70,0xE0,0xC0,0x80,0x00,0x08,0xF8,0xF8,0x20,0x3F,0x3F,0x20,0x00,0x01,0x03,0x07,0x0E,0x1C,0x3F,0x3F
	},/*"N"46*/
	{
		0x00,0xC0,0xF0,0xF0,0x38,0x08,0x08,0x08,0x38,0xF0,0xF0,0xC0,0x00,0x07,0x1F,0x1F,0x38,0x20,0x20,0x20,0x38,0x1F,0x1F,0x07
	},/*"O"47*/
	{
		0x08,0xF8,0xF8,0xF8,0x08,0x08,0x08,0x08,0x98,0xF8,0xF0,0x60,0x20,0x3F,0x3F,0x3F,0x21,0x01,0x01,0x01,0x01,0x01,0x00,0x00
	},/*"P"48*/
	{
		0x00,0xC0,0xF0,0xF0,0x38,0x08,0x08,0x08,0x18,0xF0,0xF0,0xC0,0x00,0x07,0x1F,0x1F,0x3C,0x24,0x2C,0x3C,0x78,0x7F,0x6F,0x27
	},/*"Q"49*/
	{
		0x08,0xF8,0xF8,0xF8,0x88,0x88,0x88,0x88,0xD8,0xF8,0x70,0x70,0x20,0x3F,0x3F,0x3F,0x20,0x00,0x01,0x07,0x0F,0x1C,0x38,0x20
	},/*"R"50*/
	{
		0x00,0x60,0xF0,0xF8,0x98,0x88,0x88,0x08,0x08,0x18,0x38,0x38,0x00,0x38,0x38,0x30,0x20,0x21,0x21,0x21,0x33,0x3F,0x1E,0x0C
	},/*"S"51*/
	{
		0x10,0x18,0x08,0x08,0x08,0xF8,0xF8,0xF8,0x08,0x08,0x08,0x18,0x00,0x00,0x00,0x00,0x20,0x3F,0x3F,0x3F,0x20,0x00,0x00,0x00
	},/*"T"52*/
	{
		0x08,0xF8,0xF8,0xF8,0x08,0x00,0x00,0x00,0x00,0x08,0xF8,0xF8,0x00,0x1F,0x1F,0x3F,0x30,0x20,0x20,0x20,0x20,0x30,0x1F,0x1F
	},/*"U"53*/
	{
		0x08,0x08,0x78,0xF8,0xF8,0x88,0x00,0x00,0x88,0xF8,0x78,0x08,0x00,0x00,0x00,0x03,0x1F,0x3F,0x3C,0x3C,0x1F,0x03,0x00,0x00
	},/*"V"54*/
	{
		0x08,0x78,0xF8,0xF8,0x88,0xF8,0xF8,0xF8,0x88,0xE8,0xF8,0x18,0x00,0x00,0x0F,0x3F,0x3F,0x0F,0x0F,0x3F,0x3F,0x0F,0x01,0x00
	},/*"W"55*/
	{
		0x00,0x08,0x18,0x38,0xF8,0xC8,0x80,0xC8,0x78,0x38,0x08,0x08,0x00,0x20,0x20,0x38,0x3C,0x27,0x07,0x2F,0x3C,0x38,0x30,0x20
	},/*"X"56*/
	{
		0x08,0x18,0x38,0x78,0xF8,0xE8,0x80,0x80,0xC8,0x78,0x38,0x08,0x00,0x00,0x00,0x20,0x21,0x3F,0x3F,0x3F,0x21,0x20,0x00,0x00
	},/*"Y"57*/
	{
		0x00,0x10,0x18,0x18,0x08,0x08,0x88,0xC8,0x68,0x38,0x18,0x08,0x00,0x20,0x30,0x38,0x2C,0x27,0x23,0x20,0x20,0x30,0x38,0x08
	},/*"Z"58*/
	{
		0x00,0x00,0x00,0x00,0x00,0xFE,0xFE,0x02,0x02,0x02,0x02,0x02,0x00,0x00,0x00,0x00,0x00,0x7F,0x7F,0x40,0x40,0x40,0x40,0x40
	},/*"["59*/
	{
		0x00,0x00,0x0C,0x1C,0x78,0xE0,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x03,0x0F,0x3C,0x70,0xE0,0x80
	},/*"\"60*/
	{
		0x00,0x00,0x02,0x02,0x02,0x02,0x02,0xFE,0xFE,0x00,0x00,0x00,0x00,0x00,0x40,0x40,0x40,0x40,0x40,0x7F,0x7F,0x00,0x00,0x00
	},/*"]"61*/
	{
		0x00,0x00,0x00,0x04,0x04,0x06,0x02,0x02,0x06,0x04,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
	},/*"^"62*/
	{
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80
	},/*"_"63*/
	{
		0x00,0x00,0x00,0x02,0x02,0x06,0x04,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
	},/*"`"64*/
	{
		0x00,0x00,0x00,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x00,0x00,0x00,0x18,0x3D,0x3D,0x27,0x26,0x22,0x22,0x3F,0x3F,0x3F,0x20
	},/*"a"65*/
	{
		0x00,0x08,0xF8,0xFC,0xFC,0x80,0x80,0x80,0x80,0x80,0x80,0x00,0x00,0x00,0x3F,0x3F,0x3F,0x31,0x20,0x20,0x31,0x3F,0x1F,0x0F
	},/*"b"66*/
	{
		0x00,0x00,0x00,0x80,0x80,0x80,0x80,0x80,0x80,0x00,0x00,0x00,0x00,0x0E,0x1F,0x3F,0x31,0x20,0x20,0x21,0x21,0x31,0x10,0x00
	},/*"c"67*/
	{
		0x00,0x00,0x00,0x80,0x80,0x80,0x80,0x88,0xF8,0xFC,0xFC,0x00,0x00,0x0E,0x1F,0x3F,0x31,0x20,0x20,0x30,0x3F,0x3F,0x3F,0x20
	},/*"d"68*/
	{
		0x00,0x00,0x00,0x00,0x80,0x80,0x80,0x80,0x80,0x80,0x00,0x00,0x00,0x00,0x0E,0x1F,0x3F,0x32,0x22,0x22,0x22,0x23,0x33,0x13
	},/*"e"69*/
	{
		0x00,0x80,0x80,0x80,0xF0,0xF0,0xF8,0x88,0x88,0x98,0x98,0x10,0x00,0x00,0x20,0x20,0x3F,0x3F,0x3F,0x20,0x20,0x20,0x00,0x00
	},/*"f"70*/
	{
		0x00,0x00,0x00,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x00,0x60,0xEB,0xFF,0x9F,0x94,0x94,0x94,0x97,0xF7,0xF3,0x60
	},/*"g"71*/
	{
		0x00,0x08,0xF8,0xFC,0xFC,0x80,0x80,0x80,0x80,0x80,0x00,0x00,0x00,0x20,0x3F,0x3F,0x3F,0x21,0x00,0x20,0x3F,0x3F,0x3F,0x20
	},/*"h"72*/
	{
		0x00,0x00,0x80,0x80,0x80,0x98,0x98,0x98,0x00,0x00,0x00,0x00,0x00,0x00,0x20,0x20,0x20,0x3F,0x3F,0x3F,0x20,0x20,0x20,0x00
	},/*"i"73*/
	{
		0x00,0x00,0x00,0x00,0x80,0x80,0x80,0x98,0x98,0x98,0x00,0x00,0x00,0x00,0xC0,0xC0,0xC0,0x80,0x80,0xFF,0x7F,0x7F,0x00,0x00
	},/*"j"74*/
	{
		0x00,0x08,0xF8,0xFC,0xFC,0x00,0x80,0x80,0x80,0x80,0x80,0x00,0x00,0x20,0x3F,0x3F,0x3F,0x26,0x06,0x2F,0x39,0x30,0x20,0x20
	},/*"k"75*/
	{
		0x00,0x00,0x08,0x08,0x08,0xF8,0xF8,0xF8,0x00,0x00,0x00,0x00,0x00,0x00,0x20,0x20,0x20,0x3F,0x3F,0x3F,0x20,0x20,0x20,0x00
	},/*"l"76*/
	{
		0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x20,0x3F,0x3F,0x3F,0x20,0x3F,0x3F,0x3F,0x20,0x3F,0x3F,0x3F
	},/*"m"77*/
	{
		0x00,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x00,0x00,0x00,0x20,0x3F,0x3F,0x3F,0x21,0x00,0x20,0x3F,0x3F,0x3F,0x20
	},/*"n"78*/
	{
		0x00,0x00,0x00,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x00,0x00,0x00,0x0E,0x1F,0x3F,0x31,0x20,0x20,0x20,0x31,0x3F,0x1F,0x0E
	},/*"o"79*/
	{
		0x00,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x00,0x00,0x00,0x80,0xFF,0xFF,0xFF,0xB1,0xA0,0x20,0x31,0x3F,0x1F,0x0F
	},/*"p"80*/
	{
		0x00,0x00,0x00,0x80,0x80,0x80,0x80,0x80,0x00,0x80,0x80,0x00,0x00,0x0E,0x1F,0x3F,0x31,0x20,0xA0,0xB1,0xFF,0xFF,0xFF,0x80
	},/*"q"81*/
	{
		0x80,0x80,0x80,0x80,0x80,0x80,0x00,0x80,0x80,0x80,0x80,0x80,0x20,0x20,0x20,0x3F,0x3F,0x3F,0x23,0x21,0x20,0x01,0x01,0x01
	},/*"r"82*/
	{
		0x00,0x00,0x00,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x00,0x00,0x33,0x33,0x27,0x26,0x24,0x24,0x2C,0x3C,0x39,0x19
	},/*"s"83*/
	{
		0x00,0x80,0x80,0x80,0xC0,0xE0,0xE0,0x80,0x80,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x1F,0x3F,0x3F,0x20,0x20,0x30,0x10,0x00
	},/*"t"84*/
	{
		0x00,0x80,0x80,0x80,0x80,0x00,0x00,0x80,0x80,0x80,0x80,0x00,0x00,0x00,0x1F,0x3F,0x3F,0x20,0x20,0x30,0x3F,0x3F,0x3F,0x20
	},/*"u"85*/
	{
		0x00,0x80,0x80,0x80,0x80,0x80,0x80,0x00,0x80,0x80,0x80,0x80,0x00,0x00,0x00,0x03,0x0F,0x1F,0x3E,0x38,0x1C,0x0F,0x03,0x00
	},/*"v"86*/
	{
		0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x00,0x03,0x1F,0x3F,0x3E,0x1E,0x07,0x1F,0x3F,0x3C,0x1F,0x03
	},/*"w"87*/
	{
		0x00,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x00,0x20,0x20,0x31,0x3B,0x3F,0x0E,0x2E,0x3B,0x31,0x20,0x20
	},/*"x"88*/
	{
		0x00,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x00,0x00,0x81,0x83,0x8F,0xFF,0x7C,0x3C,0x0F,0x03,0x00,0x00
	},/*"y"89*/
	{
		0x00,0x00,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x00,0x00,0x00,0x21,0x31,0x38,0x2C,0x26,0x22,0x23,0x31,0x38,0x18
	},/*"z"90*/
	{
		0x00,0x00,0x00,0x00,0x00,0x80,0x80,0xFC,0x7E,0x02,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3F,0x7F,0x40,0x40,0x00
	},/*"{"91*/
	{
		0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00
	},/*"|"92*/
	{
		0x00,0x00,0x02,0x02,0x7E,0xFC,0x80,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x40,0x40,0x7F,0x3F,0x00,0x00,0x00,0x00,0x00,0x00
	},/*"}"93*/
	{
		0x00,0x04,0x07,0x03,0x01,0x03,0x03,0x06,0x06,0x04,0x06,0x07,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
	},/*"~"94*/
};

/* Line Wrap is used for ASCII 12x16, row is 0~1, column is 0~8 (in unit of character ) */
void lcdDrawAscii12x16Char(INT8U row, INT8U column, INT8U digitOrChar)
{
	digitOrChar = digitOrChar - ASCII_OFFSET;

	if(digitOrChar < 0 || digitOrChar > 94)
	{
		digitOrChar = 0;
	}

	lcdDrawData(row*LCD_PAGES_PER_ROW+LCD_DRAW_AREA_ROW_ASCII12X16, 
		column*LCD_CHAR_DEFAULT_WIDTH, LCD_PAGES_PER_ROW, LCD_CHAR_DEFAULT_WIDTH, &ascii12x16[digitOrChar][0]);
}

void lcdDrawAscii12x16String(INT8U row, INT8U column, INT8U *digitOrChar)
{
	INT8U data i = 0, value;

	while( (digitOrChar[i] != 0) && ( column < 2*LCD_DRAW_AREA_MAX_ASCII12X16) )
	{
		value = digitOrChar[i];
		value = value - ASCII_OFFSET;

		if(value < 0 || value > 94)
		{
			value = 0;
		}	
		
		if( column >= LCD_DRAW_AREA_MAX_ASCII12X16)
		{
			lcdDrawData((row+1)*LCD_PAGES_PER_ROW+LCD_DRAW_AREA_ROW_ASCII12X16, 
				(column%LCD_DRAW_AREA_MAX_ASCII12X16)*LCD_CHAR_DEFAULT_WIDTH, LCD_PAGES_PER_ROW, LCD_CHAR_DEFAULT_WIDTH, &ascii12x16[value][0]);
		}
		else
		{
			lcdDrawData(row*LCD_PAGES_PER_ROW+LCD_DRAW_AREA_ROW_ASCII12X16, 
				column*LCD_CHAR_DEFAULT_WIDTH, LCD_PAGES_PER_ROW, LCD_CHAR_DEFAULT_WIDTH, &ascii12x16[value][0]);
		}

		i++;
		column ++;
	};
}


