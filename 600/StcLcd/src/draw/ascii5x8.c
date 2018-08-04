/*
* $Id: ascii5x8.c,v 1.2 2007/05/23 21:15:30 lizhijie Exp $
*/

#pragma	ot(9,speed)
#pragma small

#include "lcdDeclarations.h"

// ------------------  ASCII��ģ�����ݱ� ------------------------ //
// �����0x20~0x7e                                                //
// �ֿ�: Z:\2007\05\lcd\fontools\lcd����ȡģ����\Asc5x8E.dot ����ȡģ�¸�λ//
// -------------------------------------------------------------- //
INT8U code ascii5x8[96][5] =              // ASCII
{
	{
		0x00,0x00,0x00,0x00,0x00
	},//0: - -
	{
		0x00,0x00,0x5F,0x00,0x00
	},// 1: -!-
	{
		0x00,0x07,0x00,0x07,0x00
	},// -"-
	{
		0x14,0x7F,0x14,0x7F,0x14
	},// 2: -#-
	{
		0x24,0x2E,0x7B,0x2A,0x12
	},// 3 : -$-
	{
		0x23,0x13,0x08,0x64,0x62
	},// 4 -%-
	{
		0x36,0x49,0x56,0x20,0x50
	},// 5 -&-
	{
		0x00,0x04,0x03,0x01,0x00
	},// 6 -'-
	{
		0x00,0x1C,0x22,0x41,0x00
	},// 7 -(-
	{
		0x00,0x41,0x22,0x1C,0x00
	},// 8 -)-
	{
		0x22,0x14,0x7F,0x14,0x22
	},// 9 -*-
	{
		0x08,0x08,0x7F,0x08,0x08
	},// 10 -+-
	{
		0x40,0x30,0x10,0x00,0x00
	},// 11 -,-
	{
		0x08,0x08,0x08,0x08,0x08
	},// 12 ---
	{
		0x00,0x60,0x60,0x00,0x00
	},// 13 -.-
	{
		0x20,0x10,0x08,0x04,0x02
	},// 14 -/-
	{
		0x3E,0x51,0x49,0x45,0x3E
	},// 15 -0-
	{
		0x00,0x42,0x7F,0x40,0x00
	}, // 16 -1-
	{
		0x62,0x51,0x49,0x49,0x46
	}, // 17 -2-
	{
		0x21,0x41,0x49,0x4D,0x33
	}, // 18 -3-
	{
		0x18,0x14,0x12,0x7F,0x10
	}, // 19 -4-
	{
		0x27,0x45,0x45,0x45,0x39
	}, // 20 -5-
	{
		0x3C,0x4A,0x49,0x49,0x31
	}, // 21 -6-
	{
		0x01,0x71,0x09,0x05,0x03
	}, // 22 -7-
	{
		0x36,0x49,0x49,0x49,0x36
	}, // 23 -8-
	{
		0x46,0x49,0x49,0x29,0x1E
	}, // 24 -9-
	{
		0x00,0x36,0x36,0x00,0x00
	}, // 25 -:-
	{
		0x40,0x36,0x36,0x00,0x00
	}, // 26 -;-
	{
		0x08,0x14,0x22,0x41,0x00
	}, // 27 -<-
	{
		0x14,0x14,0x14,0x14,0x14
	}, // 28 -=-
	{
		0x00,0x41,0x22,0x14,0x08
	}, // 30 ->-
	{
		0x02,0x01,0x59,0x05,0x02
	}, // 31 -?-
	{
		0x3E,0x41,0x5D,0x55,0x5E
	}, // 32 -@-
	{
		0x7C,0x12,0x11,0x12,0x7C
	}, // 33 -A-
	{
		0x7F,0x49,0x49,0x49,0x36
	}, // 34 -B-
	{
		0x3E,0x41,0x41,0x41,0x22
	}, // 35 -C-
	{
		0x7F,0x41,0x41,0x41,0x3E
	}, // 36 -D-
	{
		0x7F,0x49,0x49,0x49,0x41
	}, // 37 -E-
	{
		0x7F,0x09,0x09,0x09,0x01
	}, // 38 -F-
	{
		0x3E,0x41,0x51,0x51,0x72
	}, // 39 -G-
	{
		0x7F,0x08,0x08,0x08,0x7F
	}, // 40 -H-
	{
		0x00,0x41,0x7F,0x41,0x00
	}, // 41 -I-
	{
		0x20,0x40,0x41,0x3F,0x01
	}, // 42 -J-
	{
		0x7F,0x08,0x14,0x22,0x41
	}, // 43 -K-
	{
		0x7F,0x40,0x40,0x40,0x40
	}, // 44 -L-
	{
		0x7F,0x02,0x0C,0x02,0x7F
	}, // 45 -M-
	{
		0x7F,0x04,0x08,0x10,0x7F
	}, // 46 -N-
	{
		0x3E,0x41,0x41,0x41,0x3E
	}, // 47 -O-
	{
		0x7F,0x09,0x09,0x09,0x06
	}, // 48 -P-
	{
		0x3E,0x41,0x51,0x21,0x5E
	}, // 49 -Q-
	{
		0x7F,0x09,0x19,0x29,0x46
	}, // 50 -R-
	{
		0x26,0x49,0x49,0x49,0x32
	}, // 51 -S-
	{
		0x01,0x01,0x7F,0x01,0x01
	}, // 52 -T-
	{
		0x3F,0x40,0x40,0x40,0x3F
	}, // 53 -U-
	{
		0x1F,0x20,0x40,0x20,0x1F
	}, // 54 -V-
	{
		0x7F,0x20,0x18,0x20,0x7F
	}, // 55 -W-
	{
		0x63,0x14,0x08,0x14,0x63
	}, // 56 -X-
	{
		0x03,0x04,0x78,0x04,0x03
	}, // 57 -Y-
	{
		0x61,0x51,0x49,0x45,0x43
	}, // 58 -Z-
	{
		0x7F,0x7F,0x41,0x41,0x00
	}, // 59 -[-
	{
		0x02,0x04,0x08,0x10,0x20
	}, // 60 -\-
	{
		0x00,0x41,0x41,0x7F,0x7F
	}, // 61 -]-
	{
		0x04,0x02,0x7F,0x02,0x04
	}, // 62 -^-
	{
		0x08,0x1C,0x2A,0x08,0x08
	}, // 63 -_-
	{
		0x00,0x00,0x01,0x02,0x04
	}, // 64 -`-
	{
		0x24,0x54,0x54,0x38,0x40
	}, // 65 -a-
	{
		0x7F,0x28,0x44,0x44,0x38
	}, // 66 -b-
	{
		0x38,0x44,0x44,0x44,0x08
	}, // 67 -c-
	{
		0x38,0x44,0x44,0x28,0x7F
	}, // 68 -d-
	{
		0x38,0x54,0x54,0x54,0x08
	}, // 69 -e-
	{
		0x08,0x7E,0x09,0x09,0x02
	}, // 70 -f-
	{
		0x98,0xA4,0xA4,0xA4,0x78
	}, // 71 -g-
	{
		0x7F,0x08,0x04,0x04,0x78
	}, // 72 -h-
	{
		0x00,0x00,0x79,0x00,0x00
	}, // 73 -i-
	{
		0x00,0x80,0x88,0x79,0x00
	}, // 74 -j-
	{
		0x7F,0x10,0x28,0x44,0x40
	}, // 75 -k-
	{
		0x00,0x41,0x7F,0x40,0x00
	}, // 76 -l-
	{
		0x78,0x04,0x78,0x04,0x78
	}, // 77 -m-
	{
		0x04,0x78,0x04,0x04,0x78
	}, // 78 -n-
	{
		0x38,0x44,0x44,0x44,0x38
	}, // 79 -o-
	{
		0xFC,0x24,0x24,0x24,0x18
	}, //  80 -p-
	{
		0x18,0x24,0x24,0x24,0xFC
	}, // 81 -q-
	{
		0x04,0x78,0x04,0x04,0x08
	}, // 82 -r-
	{
		0x48,0x54,0x54,0x54,0x24
	}, // 83 -s-
	{
		0x04,0x3F,0x44,0x44,0x24
	}, // 84 -t-
	{
		0x3C,0x40,0x40,0x3C,0x40
	}, // 85 -u-
	{
		0x1C,0x20,0x40,0x20,0x1C
	}, // 86 -v-
	{
		0x3C,0x40,0x3C,0x40,0x3C
	}, // 87 -w-
	{
		0x44,0x28,0x10,0x28,0x44
	}, // 88 -x-
	{
		0x9C,0xA0,0xA0,0x90,0x7C
	}, // 89 -y-
	{
		0x44,0x64,0x54,0x4C,0x44
	}, // 90 -z-
	{
		0x08,0x36,0x41,0x00,0x00
	}, // 91 -{-
	{
		0x00,0x00,0x77,0x00,0x00
	}, // 92 -|-
	{
		0x00,0x00,0x41,0x36,0x08
	}, // 93 -}-
	{
		0x08,0x04,0x08,0x10,0x08
	}, // 94 -~-
	{
		0x55,0x2A,0x55,0x2A,0x55
	}, // 95 --
};

/* No Line Wrap is used for ASCII 5x8, row is 0~1, column is 0~20 (in unit of character ) */
void lcdDrawAscii5x8String(INT8U row, INT8U column, INT8U *digitOrChar)
{
	INT8U data i = 0;
	INT8U data value;
	
	while( (digitOrChar[i] != 0) && ( (column) < LCD_DRAW_AREA_MAX_ASCII5X8) )
	{
		value = digitOrChar[i];
		value = value - ASCII_OFFSET;

		if(value < 0 || value > 94)
		{
			value = 0;
		}	
		
		lcdDrawData(row+LCD_DRAW_AREA_ROW_ASCII5X8, column*LCD_CHAR_5X8_WIDTH, 1, LCD_CHAR_5X8_WIDTH, &ascii5x8[value][0]);

		i++;
		column++;
	};
	
}

