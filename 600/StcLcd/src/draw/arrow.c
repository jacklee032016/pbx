/*
* $Id: arrow.c,v 1.6 2007/05/24 17:23:35 lizhijie Exp $
*/

#pragma	ot(9,speed)
#pragma small

#include "lcdDeclarations.h"

/*
* Up Arrow : HxW = 16x27, 432 bits/Arrow, 54 bytes/arrow (4x4)
* Up Arrow : HxW = 16*25, 400 bits/Arrow, 50 bytes/Arrow (5x3)
* Right Arrow : HxW = 16x20, 320 bits/arrow, 40 bytes/arrow, 
*/

#define		WITH_ARROW		1

INT8U code ArrowUpMark[]=
{
/*
*  Width x Height : 27��16
*/
#if WITH_ARROW
	0x00,0x00,0x00,0x40,		0x60,0x70,0xF8,0xFC,
	0xFE,0xFC,0xF8,0x70,		0x60,0x40,0x00,0x00,

	0x00,0x00,0x00,0x00,		0x00,0x00,0x7F,0x7F,
	0x7F,0x7F,0x7F,0x00,		0x00,0x00,0x00,0x00, /* arrow */
#else
	0x00,0xE0,0x10,0x08,		0xC4,0xE2,0xF2,0xF2,
	0xF2,0xE2,0xC4,0x08,		0x10,0xE0,0x00,0x00,/* first Line : 27x8 */
	
	0x00,0x03,0x04,0x08,		0x11,0x23,0x27,0x27,
	0x27,0x23,0x11,0x08,		0x04,0x03,0x00,0x00,/* second line : 27x8, circle */
#endif
};

INT8U code ArrowUpSpare[]=
{
#if WITH_ARROW
	0x00,0x00,0x00,0x40,		0x60,0x50,0xC8,0x04,
	0x02,0x04,0xC8,0x50,		0x60,0x40,0x00,0x00,

	0x00,0x00,0x00,0x00,		0x00,0x00,0x7F,0x40,
	0x40,0x40,0x7F,0x00,		0x00,0x00,0x00,0x00,
#else
	0x00,0xE0,0x10,0x08,		0xC4,0x22,0x12,0x12,
	0x12,0x22,0xC4,0x08,		0x10,0xE0,0x00,0x00,
      
	0x00,0x03,0x04,0x08,		0x11,0x22,0x24,0x24,
	0x24,0x22,0x11,0x08,		0x04,0x03,0x00,0x00,
#endif
};

INT8U code ArrowUpUnknown[]=
{/* circle with a crossbar */
	0x00,0xE2,0x14,0x08,		0x14,0x22,0x42,0x82,
	0x42,0x22,0x14,0x08,		0x14,0xE2,0x00,0x00,

      	0x00,0x23,0x14,0x08,		0x14,0x22,0x21,0x20,
      	0x21,0x22,0x14,0x08,		0x14,0x23,0x00,0x00,
};

INT8U code ArrowUpNull[]=
{
	0x00,0x00,0x00,0x00,		0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,		0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,		0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,		0x00,0x00,0x00,0x00,
};

INT8U code ArrowRightSpare[]=
{/* width x height : 20 x16 */
#if WITH_ARROW
	0x00,0xE0,0x20,0x20,		0x20,0x20,0x20,0x20,
	0x20,0x3C,0x08,0x10,		0x20,0x40,0x80,0x00,		
	0x00,0x07,0x04,0x04,		0x04,0x04,0x04,0x04,
	0x04,0x3C,0x10,0x08,		0x04,0x02,0x01,0x00,	/* Right Arrow */
#else
	0x00,0xE0,0x10,0x08,		0xC4,0x22,0x12,0x12,
	0x12,0x22,0xC4,0x08,		0x10,0xE0,0x00,0x00,	/* circle : the first line, 20x8 */
      
	0x00,0x00,0x00,0x00,		0x00,0x00,0x00,0x00,
	0x00,0x03,0x04,0x08,		0x11,0x22,0x24,0x24,	/* Circle : the second line */
#endif
};

INT8U code ArrowRightMark[]=
{
#if WITH_ARROW
	0x00,0xE0,0xE0,0xE0,	0xE0,0xE0,0xE0,0xE0,
	0xE0,0xFC,0xF8,0xF0,	0xE0,0xC0,0x80,0x00,
	0x00,0x07,0x07,0x07,	0x07,0x07,0x07,0x07,
	0x07,0x3F,0x1F,0x0F,	0x07,0x03,0x01,0x00,
#else
	0x00,0xE0,0x10,0x08,		0xC4,0xE2,0xF2,0xF2,
	0xF2,0xE2,0xC4,0x08,		0x10,0xE0,0x00,0x00,
	
	0x00,0x03,0x04,0x08,		0x11,0x23,0x27,0x27,
	0x27,0x23,0x11,0x08,		0x04,0x03,0x00,0x00,
#endif
};

INT8U code ArrowRightUnknown[]=
{/* 40 bytes, null, eg. nothing is draw */
	0x00,0xE2,0x14,0x08,0x14,0x22,0x42,0x82,
	0x42,0x22,0x14,0x08,0x14,0xE2,0x00,0x00,

	0x00,0x23,0x14,0x08,0x14,0x22,0x21,0x20,
	0x21,0x22,0x14,0x08,0x14,0x23,0x00,0x00
};

INT8U code ArrowRightNull[]=
{/* 40 bytes, null, eg. nothing is draw */
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
};


lcd_arrow_t	code		ARROW_UP =
{
	&ArrowUpSpare[0],
	&ArrowUpMark[0],
	&ArrowUpUnknown[0],
	&ArrowUpNull[0],
};

lcd_arrow_t	code		ARROW_RIGHT =
{
	&ArrowRightSpare[0],
	&ArrowRightMark[0],
	&ArrowRightUnknown[0],
	&ArrowRightNull[0],
};


void lcdDrawArrow(INT8U index, INT8U serviceState)
{
	if(serviceState < LCD_SERVICE_STATE_DISABLE ||serviceState > (LCD_SERVICE_STATE_NULL+1) )
	{
		serviceState = LCD_SERVICE_STATE_DISABLE;
	}

	if(index<4)
	{
		lcdDrawData(0, (index+1)*9+index*16, 2, 16, ARROW_UP.arrow[serviceState] );
	}
	else
	{
		lcdDrawData((index-4)*2, 112, 2, 16, ARROW_RIGHT.arrow[serviceState] );
	}
	
//	lcdDisplayData(&LCD_ARROWS[index].position, LCD_ARROWS[index].arrows->arrow[serviceState]);
//	lcdDisplayData(&LCD_ARROWS[index].position, LCD_ARROWS[index].arrows->arrow[serviceState]);
}


void lcdDrawArrows(void)
{
	INT8U data i;

	for(i= 0; i<LCD_SERVICE_MAX_NUMBER; i++)
	{
		lcdDrawArrow(i, stcLcd.services[i].state);
	}
}

