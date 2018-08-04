/*
* $Id: strings.c,v 1.10 2007/09/26 19:48:08 lizhijie Exp $
*/

#pragma	ot(9,speed)
#pragma small

#include "lcdDeclarations.h"

INT8U code assist[5][128] =
{/* BMP : 5 rows, 128 columns */
	{
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0xC0,0xE0,0xF0,0xF8,0xF8,0xF8,0xF8,0xF0,0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
	},

	{
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0xC0,0xE0,0xF0,0xF8,0xFC,0xFE,0xFF,0xFF,0x7F,0x3F,0x9F,0x8F,0x87,0x83,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
	},

	{
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0xC0,0xE0,0xF0,0xF8,0xFC,0xFE,0xFF,0xFF,0x7F,0x3F,0x1F,0x0F,0x07,0x03,0x01,0x00,0x00,0x00,0x07,0x7F,0xFF,0xFF,0xFC,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x80,0xC0,0xC0,0xC0,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0xC0,0xC0,0xC0,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0xC0,0xC0,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0xC0,0xC0,0xC0,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x00,0x00,0x00,0x00
	},

	{
		0x00,0x00,0x00,0x00,0xE0,0xF0,0xF0,0xF8,0xF8,0xF8,0xF8,0xF8,0xF8,0xF8,0xF8,0xF8,0xF8,0xF8,0xF8,0xF8,0xF8,0xF8,0xF8,0xF8,0xF8,0xF8,0xFC,0xFE,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFB,0xF9,0xF8,0xF8,0xF8,0xF0,0xE0,0xC0,0x00,0x00,0x00,0xC0,0xE0,0xF0,0xF8,0xFF,0xFF,0xFF,0xFF,0xF8,0xF8,0xF8,0xF0,0xE0,0xC0,0x00,0x00,0x00,0x0C,
		0x0E,0x1F,0x3F,0x7F,0x7F,0xFB,0xF3,0xE0,0xE0,0xC0,0x80,0x0C,0x0E,0x1F,0x3F,0x7F,0x7F,0xFB,0xF3,0xE0,0xE0,0xC0,0x80,0x00,0x00,0x00,0x00,0xF1,0xFB,0xFB,0xF1,0x00,0x00,0x00,0x0C,0x0E,0x1F,0x3F,0x7F,0x7F,0xFB,0xF3,0xE0,0xE0,0xC0,0x80,0x00,0x00,0x03,0x07,0x07,0x07,0x07,0xF7,0xF7,0xF7,0xF7,0x07,0x07,0x07,0x03,0x00,0x00,0x00
	},

	{
		0x00,0x00,0x00,0x00,0x01,0x03,0x07,0x07,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x6F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x7F,0x3F,0x1F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x07,0x07,0x07,0x03,0x01,0x00,0x00,0x00,0x01,0x07,0x07,0x0F,0x0F,0x1F,0x1F,0xFF,0xFF,0xFF,0xFF,0x0F,0x07,0x01,0x00,0x00,0x00,0x1C,
		0x3C,0x3E,0x1E,0x1E,0x0F,0x0F,0x0F,0x07,0x07,0x03,0x03,0x1C,0x3C,0x3E,0x1E,0x1E,0x0F,0x0F,0x0F,0x07,0x07,0x03,0x03,0x00,0x00,0x00,0x00,0x3F,0x3F,0x3F,0x3F,0x00,0x00,0x00,0x00,0x1C,0x3C,0x3E,0x1E,0x1F,0x0F,0x0F,0x07,0x07,0x03,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7F,0x7F,0x7F,0x7F,0x00,0x00,0x00,0x00,0x00,0x00,0x00
	}
};

#if 0
INT8U code welcome[4][720] =
{
	{
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x40,0xC0,0xC0,0xC0,0x40,0x40,0x40,0xC0,0xC0,0xC0,0x40,0x00,0x40,0xC0,0xC0,0xC0,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x40,0x40,0x40,0x40,0xC0,0xE0,0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x40,0x40,0x40,0x40,0xC0,0xC0,0xC0,0x40,0x40,0x40,0x40,0x00,0x00,0x00,0x40,0x40,0xC0,0xC0,0xC0,0x40,0x40,0x40,0x40,0x40,0xC0,0xC0,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x40,0x40,0xC0,0xC0,0xC0,0x40,0x40,0x40,0x40,0x40,0xC0,0xC0,0x80,0x00,0x00,
		0x00,0x40,0x40,0xC0,0xC0,0xC0,0x40,0x40,0x40,0x40,0xC0,0xC0,0x80,0x80,0x00,0x00,0x00,0x40,0x40,0xC0,0xC0,0xC0,0x40,0x40,0x00,0x40,0x40,0xC0,0xC0,0x40,0x40,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0xE0,0xE0,0xE0,0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
	},

	{
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0xFF,0xFF,0xFC,0x00,0x00,0xF7,0xFF,0xFF,0xF0,0x00,0xE0,0xFF,0x1F,0x00,
		0x00,0x00,0x00,0x80,0xC0,0xC0,0x60,0x20,0x20,0x20,0x60,0xE0,0xC0,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0xC0,0xC0,0x60,0x60,0x20,0x20,0x60,0xE0,0xC0,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0xC0,0xC0,0x60,0x60,0x20,0x20,0x60,0x60,0xC0,0xC0,0x80,0x00,0x00,0x20,0xE0,0xF0,0xF0,0x60,0x20,0xE0,
		0xE0,0xC0,0x60,0x20,0xE0,0xE0,0xC0,0x00,0x00,0x00,0x00,0x80,0xC0,0xC0,0x60,0x20,0x20,0x20,0x60,0xE0,0xC0,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x20,0x20,0x20,0x30,0xF8,0xFF,0xFF,0x20,0x20,0x20,0x20,0x20,0x00,0x00,0x00,0x00,0x00,0x00,0xC0,0xC0,0x60,0x60,0x20,0x20,0x60,0x60,0xC0,0xC0,0x80,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x20,0x20,0xE0,0xF0,0xF0,0x00,0x00,0x00,0x00,0x20,0x20,0xE0,0xF0,0xF0,0x00,0x00,0x00,0x00,0x80,0xC0,0xE0,0x60,0x20,0x20,0x20,0x20,0x60,0xC0,0xE0,0xE0,0x00,0x00,0x00,0x00,0x80,0xC0,0xC0,0x60,0x20,0x20,0x20,0x60,0xE0,0xC0,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x80,0xC1,0xFF,0x7F,0x3E,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x80,0xC1,0xFF,0x7F,0x3E,
		0x00,0x00,0x00,0xFF,0xFF,0xFF,0x80,0x80,0x80,0x80,0xC0,0xE1,0x7F,0x3F,0x1F,0x00,0x00,0x00,0x00,0x01,0x0F,0x3F,0xFF,0xF8,0xE0,0xF8,0x1E,0x07,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0xC0,0xE0,0x60,0x20,0x20,0x20,0x20,0x60,0xC0,0xE0,0xE0,0x00,0x00,0x20,0x20,0xE0,0xE0,0xE0,0x20,0x20,
		0x00,0x00,0x20,0xE0,0xE0,0x60,0x20,0x20,0x00,0x00,0x00,0x80,0xC0,0xE0,0x60,0x20,0x20,0x20,0x20,0x60,0xC0,0xE0,0xE0,0x00,0x00,0x00,0x20,0x20,0x20,0x30,0xF8,0xFF,0xFF,0x20,0x20,0x20,0x20,0x20,0x00,0x00,0x00,0x00,0x00,0x80,0xC0,0xC0,0x60,0x20,0x20,0x20,0x60,0xE0,0xC0,0x80,0x00,0x00,0x00,0x20,0xE0,0xF0,0xF0,0x60,0x20,0xE0,0xE0,0xC0,0x60,0x20,0xE0,0xE0,0xC0,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x0F,0xFF,0xFF,0x0F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
	},

	{
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x7F,0xFF,0xFE,0xFF,0x0F,0x01,0x7F,0xFF,0xFC,0xFF,0x07,0x00,0x00,
		0x00,0x00,0x7E,0xFF,0xFF,0x88,0x08,0x08,0x08,0x08,0x08,0x08,0x0F,0x8F,0x8E,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7E,0xFF,0xFF,0x81,0x00,0x00,0x00,0x00,0x00,0x03,0x03,0xC3,0xC0,0x00,0x00,0x00,0x7E,0xFF,0xFF,0x81,0x00,0x00,0x00,0x00,0x00,0x00,0x81,0xFF,0xFF,0x7E,0x00,0x00,0xFF,0xFF,0xFF,0x00,0x00,0xFF,
		0xFF,0xFF,0x00,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x7E,0xFF,0xFF,0x88,0x08,0x08,0x08,0x08,0x08,0x08,0x0F,0x8F,0x8E,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x80,0x80,0x00,0x00,0x00,0x7E,0xFF,0xFF,0x81,0x00,0x00,0x00,0x00,0x00,0x00,0x81,0xFF,0xFF,0x7E,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0xC0,0xC3,0x07,0x0F,0x0C,0x1C,0x18,0x18,0x38,0x30,0xF0,0xE1,0xC1,0x00,0x00,0x00,0x7E,0xFF,0xFF,0x88,0x08,0x08,0x08,0x08,0x08,0x08,0x0F,0x8F,0x8E,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x00,0x00,0x00,0x00,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x00,0x00,0x00,0xFF,0xFF,0xFF,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x00,0x00,0x00,
		0x00,0x00,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x01,0x03,0xFF,0xFE,0xF8,0x00,0x00,0x00,0x00,0xC0,0xF0,0x3C,0x0F,0x0F,0x3F,0xFE,0xF8,0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xC0,0xC3,0x07,0x0F,0x0C,0x1C,0x18,0x18,0x38,0x30,0xF0,0xE1,0xC1,0x00,0x00,0x00,0x00,0x00,0x07,0x1F,0xFF,0xF8,
		0xE0,0xE0,0xFC,0x1F,0x03,0x00,0x00,0x00,0x00,0x00,0xC0,0xC3,0x07,0x0F,0x0C,0x1C,0x18,0x18,0x38,0x30,0xF0,0xE1,0xC1,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x80,0x80,0x00,0x00,0x00,0x7E,0xFF,0xFF,0x88,0x08,0x08,0x08,0x08,0x08,0x08,0x0F,0x8F,0x8E,0x00,0x00,0x00,0xFF,0xFF,0xFF,0x00,0x00,0xFF,0xFF,0xFF,0x00,0x00,0xFF,0xFF,0xFF,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x87,0x87,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
	},

	{
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0x07,0x00,0x00,0x00,0x00,0x07,0x07,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x01,0x03,0x03,0x06,0x06,0x04,0x04,0x04,0x06,0x03,0x03,0x00,0x00,0x00,0x00,0x00,0x04,0x04,0x04,0x04,0x07,0x07,0x07,0x04,0x04,0x04,0x04,0x00,0x00,0x00,0x00,0x00,0x01,0x03,0x03,0x06,0x04,0x04,0x04,0x04,0x06,0x03,0x01,0x00,0x00,0x00,0x00,0x00,0x01,0x03,0x03,0x06,0x04,0x04,0x04,0x04,0x06,0x03,0x03,0x01,0x00,0x00,0x04,0x07,0x07,0x07,0x04,0x04,0x07,
		0x07,0x07,0x04,0x04,0x07,0x07,0x07,0x04,0x00,0x00,0x00,0x01,0x03,0x03,0x06,0x06,0x04,0x04,0x04,0x06,0x03,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x03,0x07,0x06,0x04,0x04,0x06,0x03,0x01,0x00,0x00,0x00,0x00,0x01,0x03,0x03,0x06,0x04,0x04,0x04,0x04,0x06,0x03,0x03,0x01,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x03,0x07,0x06,0x04,0x04,0x04,0x06,0x03,0x07,0x07,0x03,0x02,0x00,0x00,0x07,0x07,0x07,0x06,0x04,0x04,0x04,0x04,0x04,0x06,0x07,0x03,0x01,0x00,0x00,0x00,0x00,0x01,0x03,0x03,0x06,0x06,0x04,0x04,0x04,0x06,0x03,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x04,0x04,0x04,0x07,0x07,0x07,0x04,0x04,0x04,0x04,0x00,0x00,0x00,0x04,0x04,0x07,0x07,0x07,0x04,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x04,0x07,0x07,0x07,0x04,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x04,0x04,0x07,0x07,0x07,0x04,0x04,0x04,0x04,0x04,0x06,0x07,0x03,0x01,0x00,0x00,0x04,0x04,0x07,0x07,0x04,0x04,0x00,0x00,0x00,0x04,0x07,0x07,0x07,0x04,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0x07,0x07,0x06,0x04,0x04,0x04,0x04,0x04,0x06,0x07,0x03,0x01,0x00,0x00,0x00,0xC0,0xC0,0xC0,0x80,0xC0,0x7F,
		0x3F,0x07,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0x07,0x07,0x06,0x04,0x04,0x04,0x04,0x04,0x06,0x07,0x03,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x03,0x07,0x06,0x04,0x04,0x06,0x03,0x01,0x00,0x00,0x00,0x00,0x01,0x03,0x03,0x06,0x06,0x04,0x04,0x04,0x06,0x03,0x03,0x00,0x00,0x00,0x04,0x07,0x07,0x07,0x04,0x04,0x07,0x07,0x07,0x04,0x04,0x07,0x07,0x07,0x04,
		0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x07,0x07,0x07,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
	}/*"C:\Documents and Settings\lijishuang\����\IP-PBX\welcome.BMP.BMP"0*/
};	
#endif
									
/*INT8U code close[16]={
                         0x00,0x00,0x00,0x70,0x00,0x88,0x01,0x04,0x01,0x04,0x01,0x04,0x00,0x88,0x00,0x00,//"c",0

                        /* 0x00,0x00,0x10,0x04,0x10,0x04,0x1F,0xFC,0x00,0x04,0x00,0x04,0x00,0x00,0x00,0x00,//"l",1

                         0x00,0x00,0x00,0xF8,0x01,0x04,0x01,0x04,0x01,0x04,0x01,0x04,0x00,0xF8,0x00,0x00,//"o",2

                         0x00,0x00,0x00,0xCC,0x01,0x24,0x01,0x24,0x01,0x24,0x01,0x24,0x01,0x98,0x00,0x00,//"s",3

                         0x00,0x00,0x00,0xF8,0x01,0x44,0x01,0x44,0x01,0x44,0x01,0x44,0x00,0xC8,0x00,0x00,//"e",4

                         0x00,0x00,0x00,0x00,0x00,0x00,0x1F,0xCC,0x00,0x0C,0x00,0x00,0x00,0x00,0x00,0x00,//"!",5 
					 };	*/	
									
									
/*INT8U code close[24]={0x00,0x00,0x00,0x80,0x80,0x80,0x80,0x80,0x80,0x00,0x00,0x00,0x00,0x0E,0x1F,0x3F,0x31,0x20,0x20,0x21,0x21,0x31,0x10,0x00};*/								

lcd_service_info_t 		code		defaultServiceInfos[LCD_SERVICE_MAX_NUMBER] =
{
	{
		{
			LCD_DEFAULT_NAME_WEB,
		},
		{
			"WWW service for Admin",
		},
	},/* WebServer*/
	
	{
		{
			LCD_DEFAULT_NAME_FILE,
		},
		{
			"File&Resource Shared",
		},
	},/*File     Server*/
	
	{
		{
			LCD_DEFAULT_NAME_MAIL,
		},
		{
			"SMTP/POP3/IMAPServer"
		}	
	},/* Mail     Server*/
	
	{
		{
			LCD_DEFAULT_NAME_PRINTER,
		},
		{
			"Printer Shared in LAN",
		},
	},/* Printer  Server*/

	{
		{
	/*		'D', 'H', 'C', 'P', ' ', ' ',' ',' ',' ', 'S', 'e', 'r', 'v', 'e', 'r',*/
			LCD_DEFAULT_NAME_DHCP,
		},
		{
			"Camera&Remote Monitor",
		},
	},/* Camera     Server*/

	{
		{
			LCD_DEFAULT_NAME_LAN,
		},
		{
			"IP Address for LAN",
		},
	},/*LAN */

	{
		{
			LCD_DEFAULT_NAME_WAN,
		},
		{
			"IP Address for WAN",
		},
	},/* WAN */

	{
		{
			LCD_DEFAULT_NAME_PROTOCOL,
		},
		{
			"Protocol of WAN Port",
		},
	},/* */
};

INT8U	code		defaultStateNames[3][LCD_DRAW_AREA_MAX_ASCII5X8] =
{
	{
		"      (Stopped)",
	},/* stopped */
	{
		"      (Running)",
	},/* running */
	{
		"      (Unknown)",
	},/* Static */
};


INT8U	code		netModeNames[3][LCD_DRAW_AREA_MAX_COLUMN+1] =
{
	{
		"  PPPOE  ",
	},/* PPPOE */
	{
		"   DHCP  ",
	},/* DHCP */
	{
		" Static  ",
	},/* Static */
};

INT8U		code				verInfo[] =
{
		' ', 'A','s','s','i','s','t',' ','@',
#if 0			
		LCD_VER_MAJOR_0,
		LCD_VER_MAJOR_1,

		'.',

		LCD_VER_MINOR_0,
		LCD_VER_MINOR_1,
#endif
		' ',

		{
			__DATE__,
		},
};


INT8U	code		stringReboot[] =
{
	"AS600 rebooting.....",
};

INT8U	code		stringWaiting[] =
{
	"Waiting response.....",
};

INT8U	code		stringWanError[] =
{
	"WAN is not static",
};

/* Bitmap�������ݱ�  
* size : 100 * 48 
*/
INT8U 	code		eyes[] =
{
      0xD6,0xE9,0xD4,0xD2,0xE4,0xEA,0xD0,0xEA,
      0xD0,0xEA,0xC0,0xAA,0xC0,0xAA,0x80,0xAA,
      0x00,0x52,0x84,0x20,0x8A,0x20,0x84,0x20,
      0x0A,0x40,0x08,0x24,0x00,0x24,0x00,0x24,
      0x00,0x80,0x24,0x00,0x00,0x10,0x00,0x00,
      0x00,0x10,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x20,0x40,0x00,0xA0,0x48,0xA0,0x48,0xA0,
      0x74,0xA8,0x5A,0xB4,0x5A,0xAD,0x5E,0x2F,
      0x5D,0x1E,0x1D,0x0E,0x05,0x0B,0x05,0x06,
      0x01,0x00,0x00,0x01,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x9F,0x47,0x2F,0x87,
      0x2F,0x57,0x0F,0xAF,0x5F,0x0F,0x5F,0x1F,
      0xBF,0x1F,0xBE,0x3F,0xBF,0x7D,0xBE,0x7F,
      0xBC,0x7E,0xFA,0x7C,0xF5,0xFC,0xF5,0xF8,
      0x65,0xD8,0xA1,0x48,0x50,0x84,0x20,0x82,
      0x10,0x00,0x00,0x01,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x02,0x00,0x02,0x09,0x04,
      0x02,0x09,0x02,0x05,0x00,0x82,0x01,0x10,
      0x42,0x00,0x28,0x82,0xA0,0xC8,0xA4,0xE1,
      0xE8,0xF4,0xE0,0xF8,0xF4,0xF8,0xF0,0xF8,
      0xF8,0xFC,0xF8,0xF0,0xF8,0xF0,0xF8,0xF0,
      0xE0,0xF0,0xC0,0xC0,0xC0,0x80,0x00,0x00,
      0x54,0x95,0x20,0x4A,0x90,0xED,0x90,0xEA,
      0xF5,0xE8,0xF5,0xE8,0xF2,0xE8,0xF2,0xF8,
      0xF2,0xE8,0xE2,0xF8,0xE2,0xE8,0xE2,0xD5,
      0xE0,0xCA,0x82,0xA9,0x83,0x2A,0x05,0xAB,
      0x15,0x4A,0xAA,0x50,0xAA,0x01,0xA8,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x88,0x04,0x21,0x88,0xEA,0xF9,0xFF,0xFE,
      0x1F,0x5F,0x0F,0x27,0x83,0x01,0x2B,0x7F,
      0xFF,0xFF,0xFF,0xFF,0xFF,0xEF,0xF7,0xC3,
      0xEB,0xE7,0x77,0xE1,0x33,0xAB,0x5F,0x4F,
      0x57,0x06,0x0E,0x2D,0xB5,0x2A,0x5D,0xAB,
      0x7E,0xFE,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
      0xCF,0x87,0xEB,0x15,0x41,0x1F,0xFF,0xFF,
      0xFF,0xFF,0xFF,0xFF,0xF7,0xFF,0xE7,0xFF,
      0xFF,0xFF,0xDF,0xCC,0xFE,0xF9,0xD2,0x55,
      0x0A,0x55,0xAA,0x90,0x45,0x10,0x40,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x22,0x08,0xBF,
      0x1F,0x4F,0x1F,0x4E,0x15,0x4E,0x15,0x0E,
      0x24,0x0E,0x46,0x0A,0x16,0x07,0x07,0x03,
      0x07,0x03,0x03,0x01,0x01,0x00,0x01,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x54,0x05,0x51,0x04,0x29,0x42,0x0A,0x52,
      0x05,0xAB,0x15,0x83,0x57,0x0B,0x27,0x4B,
      0x17,0x47,0x16,0x47,0x0B,0xA7,0x03,0x4B,
      0x05,0xA3,0x04,0x21,0x82,0x09,0xA0,0x09,
      0x04,0x51,0x05,0xA9,0x45,0x10,0xA6,0x08,
      0xA2,0x15,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x1D,0x24,0x11,0x04,
      0x21,0x14,0x01,0x08,0x12,0x00,0x08,0x22,
      0x00,0x15,0x00,0x05,0x10,0x05,0x00,0x11,
      0x04,0x00,0x12,0x00,0x02,0x10,0x01,0x04,
      0x10,0x02,0x08,0x01,0x10,0x05,0x20,0x12,
      0x05,0x10,0x0A,0x20,0x0A,0x10,0x01,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};

#if 0
INT8U code tiger[] =
{
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x30,0x34,
      0x60,0xB4,0x74,0xF4,0xF4,0xF4,0xF4,0xEC,
      0xD4,0x68,0xD0,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x40,0x40,0x40,
      0x40,0x40,0x40,0x40,0x60,0x60,0xA0,0xB0,
      0xB0,0xD0,0xD0,0xD0,0xD0,0xD0,0xD0,0xD8,
      0xE8,0xD8,0xB0,0xA0,0xB0,0xA0,0xA0,0xA0,
      0xB0,0xB0,0xA0,0xE0,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x80,
      0x80,0xC0,0xC0,0x40,0x40,0x40,0x40,0x40,
      0x40,0x40,0x80,0x40,0x40,0x40,0x40,0x40,
      0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x60,
      0x60,0xA0,0xB0,0xA0,0xA1,0xD6,0xEF,0xF6,
      0xFF,0xFF,0x7F,0xBF,0x6F,0xB5,0x5F,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x0D,0xD7,0x7D,0xCF,0xFF,0xFF,0xFF,0xFF,
      0xFF,0xFF,0xFF,0xFF,0xFD,0xF6,0xF7,0xF7,
      0xFF,0xFF,0xDF,0xF3,0xFB,0xDF,0xCF,0xCF,
      0xCF,0xD7,0xD7,0xEF,0xEF,0xDF,0xF7,0x37,
      0x00,0x00,0xC0,0x70,0xD4,0xEA,0xF4,0xFB,
      0x7B,0xBD,0xDD,0x5D,0x5E,0x6E,0x6E,0x2F,
      0x2F,0x27,0x37,0x37,0x37,0x37,0x36,0x27,
      0x37,0x37,0x37,0x37,0x37,0x37,0x37,0x37,
      0x37,0x37,0x17,0x17,0x17,0x17,0x17,0x1B,
      0x1B,0x0B,0x0D,0x0D,0x06,0x06,0x01,0x02,
      0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x01,0xFF,0xD7,0xFF,
      0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
      0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
      0xFF,0xFF,0xB7,0x7B,0xDB,0x4B,0x1B,0x1B,
      0x17,0x1B,0x03,0x16,0x00,0x00,0x0F,0x58,
      0xAF,0x5F,0xBF,0xBF,0x7F,0x7B,0xFB,0xFA,
      0xF6,0xF4,0xF4,0x6C,0xEC,0xEC,0xEC,0xEC,
      0xEC,0xEC,0xEC,0xF4,0xF4,0xF4,0xF4,0xF4,
      0xEC,0xF4,0xF4,0xF4,0xF4,0xF4,0xEC,0xEC,
      0xEC,0xEC,0xF4,0xF4,0xF4,0xF4,0xF4,0xF4,
      0xF4,0xF4,0xF4,0xF6,0xF4,0xF4,0xF4,0xF4,
      0xF4,0xF4,0xF4,0xEC,0xEC,0xEC,0xE8,0xE8,
      0xE8,0xD8,0xE8,0xD8,0xD0,0xD8,0xD8,0xC8,
      0xE8,0xE8,0xEB,0xEE,0xFF,0xDE,0xFF,0xFF,
      0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
      0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
      0xFF,0xF7,0xEE,0xE8,0xD4,0x68,0xD0,0x00,
      0x00,0x00,0x00,0x00,0x80,0x81,0x81,0x81,
      0x01,0x03,0x02,0x02,0x02,0x02,0x02,0x01,
      0x02,0x82,0xC2,0xB6,0xDA,0xFE,0xF6,0xFA,
      0xFC,0xFE,0xFE,0xFF,0xFF,0xFF,0xFF,0xFF,
      0xFF,0xFF,0xFF,0xFF,0x7F,0xFF,0xFF,0xFF,
      0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xAF,0xFF,
      0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
      0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
      0xFF,0xFF,0xFF,0xFF,0xDF,0x7E,0xEF,0xFF,
      0xFF,0xFF,0xFF,0xDF,0xBF,0xFF,0xEF,0xFF,
      0xBF,0xFE,0xBD,0xDB,0xFF,0xFF,0xFF,0xFF,
      0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
      0xFF,0xFE,0x5B,0x7E,0x00,0x10,0x34,0x1E,
      0x2E,0x3E,0x3E,0x3E,0x3A,0x36,0x34,0x34,
      0x34,0x2C,0x28,0x2C,0x2C,0x2F,0x36,0x3F,
      0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,
      0x3F,0x3F,0x1F,0x3F,0x3F,0x3F,0x3F,0x3E,
      0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,
      0x3F,0x3E,0x3E,0x3F,0x3F,0x3F,0x3F,0x1F,
      0x3F,0x2F,0x2F,0x2F,0x1F,0x1F,0x1F,0x1F,
      0x1F,0x1F,0x1F,0x2F,0x3F,0x3F,0x3F,0x3F,
      0x3F,0x3F,0x3E,0x3F,0x2F,0x2E,0x3F,0x3F,
      0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,
      0x3F,0x3F,0x3F,0x1F,0x1F,0x2F,0x37,0x3F,
      0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F
};
#else
/////////////////////////////////////////////////////////////////////////
// Bitmap�������ݱ�                                                    //
// ͼƬ: C:\pic0325.bmp,����ȡģ�¸�λ,��������:�����Ҵ��ϵ���       //
// ͼƬ�ߴ�: 126 * 64                                                  //
/////////////////////////////////////////////////////////////////////////
INT8U code tiger[] =
{
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x30,0x34,
      0x60,0xB4,0x74,0xF4,0xF4,0xF4,0xF4,0xEC,
      0xD4,0x68,0xD0,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x40,0x40,0x40,
      0x40,0x40,0x40,0x40,0x60,0x60,0xA0,0xB0,
      0xB0,0xD0,0xD0,0xD0,0xD0,0xD0,0xD0,0xD8,
      0xE8,0xD8,0xB0,0xA0,0xB0,0xA0,0xA0,0xA0,
      0xB0,0xB0,0xA0,0xE0,0xA0,0x80,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x80,0x80,0x80,0xC0,0xC0,0x40,0x40,0x40,
      0x40,0x40,0x40,0x40,0x80,0x40,0x40,0x40,
      0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,
      0x40,0x60,0x60,0xA0,0xB0,0xA0,0xA1,0xD6,
      0xEF,0xF6,0xFF,0xFF,0x7F,0xBF,0x6F,0xB5,
      0x5F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x0D,0xD7,0x7D,0xCF,0xFF,0xFF,
      0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFD,0xF6,
      0xF7,0xF7,0xFF,0xFF,0xDF,0xF3,0xFB,0xDF,
      0xCF,0xCF,0xCF,0xD7,0xD7,0xEF,0xEF,0xDF,
      0xF7,0x37,0xD6,0x03,0x06,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0xC0,0x70,
      0xD4,0xEA,0xF4,0xFB,0x7B,0xBD,0xDD,0x5D,
      0x5E,0x6E,0x6E,0x2F,0x2F,0x27,0x37,0x37,
      0x37,0x37,0x36,0x27,0x37,0x37,0x37,0x37,
      0x37,0x37,0x37,0x37,0x37,0x37,0x17,0x17,
      0x17,0x17,0x17,0x1B,0x1B,0x0B,0x0D,0x0D,
      0x06,0x06,0x01,0x02,0x01,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x01,0xFF,0xD7,0xFF,0xFF,0xFF,0xFF,0xFF,
      0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
      0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xB7,0x7B,
      0xDB,0x4B,0x1B,0x1B,0x17,0x1B,0x03,0x16,
      0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x0F,0x58,0xAF,0x5F,
      0xBF,0xBF,0x7F,0x7B,0xFB,0xFA,0xF6,0xF4,
      0xF4,0x6C,0xEC,0xEC,0xEC,0xEC,0xEC,0xEC,
      0xEC,0xF4,0xF4,0xF4,0xF4,0xF4,0xEC,0xF4,
      0xF4,0xF4,0xF4,0xF4,0xEC,0xEC,0xEC,0xEC,
      0xF4,0xF4,0xF4,0xF4,0xF4,0xF4,0xF4,0xF4,
      0xF4,0xF6,0xF4,0xF4,0xF4,0xF4,0xF4,0xF4,
      0xF4,0xEC,0xEC,0xEC,0xE8,0xE8,0xE8,0xD8,
      0xE8,0xD8,0xD0,0xD8,0xD8,0xC8,0xE8,0xE8,
      0xEB,0xEE,0xFF,0xDE,0xFF,0xFF,0xFF,0xFF,
      0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
      0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xF7,
      0xEE,0xE8,0xD4,0x68,0xD0,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x80,0x81,0x81,0x81,
      0x01,0x03,0x02,0x02,0x02,0x02,0x02,0x01,
      0x02,0x82,0xC2,0xB6,0xDA,0xFE,0xF6,0xFA,
      0xFC,0xFE,0xFE,0xFF,0xFF,0xFF,0xFF,0xFF,
      0xFF,0xFF,0xFF,0xFF,0x7F,0xFF,0xFF,0xFF,
      0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xAF,0xFF,
      0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
      0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
      0xFF,0xFF,0xFF,0xFF,0xDF,0x7E,0xEF,0xFF,
      0xFF,0xFF,0xFF,0xDF,0xBF,0xFF,0xEF,0xFF,
      0xBF,0xFE,0xBD,0xDB,0xFF,0xFF,0xFF,0xFF,
      0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
      0xFF,0xFE,0x5B,0x7E,0xC0,0xC0,0x80,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x80,0x00,0x00,0x00,0x00,0x00,0x00,0xD0,
      0xF4,0xDE,0xEE,0xFE,0xFE,0xFE,0xFA,0xF6,
      0xB4,0xF4,0xF4,0xEC,0xE8,0xEC,0xEC,0xEF,
      0xF6,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
      0xFF,0x7F,0xFF,0xBF,0xDF,0xFF,0xFF,0xFF,
      0xFF,0xFE,0xFF,0xFF,0xBF,0xFF,0xFF,0xFF,
      0xFF,0xFF,0xFF,0xFE,0xFE,0xFF,0xFF,0x7F,
      0xBF,0x5F,0xFF,0x6F,0x6F,0x6F,0x5F,0x5F,
      0x5F,0x5F,0x5F,0x5F,0x5F,0x2F,0xBF,0xBF,
      0xBF,0xBF,0xBF,0x7F,0x7E,0x7F,0x6F,0x6E,
      0x7F,0x7F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
      0xFF,0xFF,0xBF,0xBF,0xBF,0xDF,0x5F,0x6F,
      0x77,0x7F,0xBF,0xBF,0xBF,0xBF,0x7F,0x7F,
      0x7F,0x7F,0x7E,0x7F,0x7D,0xFD,0xFB,0xFB,
      0xFA,0xFA,0xFA,0xFA,0xFA,0xFA,0xFA,0xFA,
      0xFB,0xFA,0xFB,0xFD,0xFD,0xFD,0xFD,0xFD,
      0xBD,0x7D,0xDD,0x7D,0x3A,0x0E,0xFB,0xDD,
      0x7F,0x7F,0xFF,0xFF,0xFB,0xFB,0xFD,0xEE,
      0xED,0xEB,0xDB,0xE5,0xF7,0x17,0x17,0x37,
      0x37,0x37,0x37,0x37,0x17,0x17,0x17,0x17,
      0x17,0x17,0x1B,0x1B,0x0B,0x0B,0x05,0x0D,
      0x0B,0x1B,0x2B,0x57,0x37,0x6F,0xDF,0xBF,
      0x7F,0x7F,0xFF,0xFE,0xFC,0x79,0x7D,0x7B,
      0x7A,0x7A,0xB6,0xB6,0x78,0xB4,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x01,0x01,0x01,0x01,0x01,0x01,0x05,0x0F,
      0xFD,0xAF,0x7F,0xFF,0xFF,0xFF,0xFF,0xFF,
      0xEB,0xEF,0xE8,0xE8,0xE9,0xE9,0xD1,0xA1,
      0xD1,0x81,0x01,0x01,0x01,0x01,0x01,0x03,
      0x03,0x03,0x03,0x02,0x02,0x02,0x02,0x02,
      0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,
      0x02,0x02,0x02,0x02,0x02,0x02,0x03,0x01,
      0x00,0x01,0x00,0x00,0x01,0x00,0x03,0x01,
      0x06,0x06,0x05,0x06,0x05,0x05,0x05,0x05,
      0x02,0x05,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x01,0x00,0x03,0x03,
      0x02,0x02,0x02,0x03,0x03,0x01,0x01,0x01,
      0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x05,
      0x03,0x0F,0x0B,0x0B,0x1B,0x1B,0x17,0x17,
      0x17,0x1B,0x1B,0x1B,0x0D,0x05,0x03,0x05,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};

#endif

void lcdScreenProtected(void)
{
#define	SCREEN_MAX_NUMBER		4
	SCREEN_CLEAN();

	stcLcd.screenIndex++;
	
	if( (stcLcd.screenIndex%(SCREEN_MAX_NUMBER))==0)
		lcdDrawData(0x01,0x00,0x05,0x80,&assist[0][0]);
	else if( (stcLcd.screenIndex%(SCREEN_MAX_NUMBER))==1)
	{
		lcdDrawData(0x01, 10, 6, 100, &eyes[0] );
	}
	else if( (stcLcd.screenIndex%(SCREEN_MAX_NUMBER))==2)
	{
		lcdDrawData(0x01, 0, 7, 126, &tiger[0] );
	}
	else if( (stcLcd.screenIndex%(SCREEN_MAX_NUMBER))== 3)
	{
		SCREEN_BLINK();
	}
}

