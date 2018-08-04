/*
* $Id: pbx_channel_if.c,v 1.12 2007/07/25 17:53:55 lizhijie Exp $
*/

#pragma	ot(9,speed)
#pragma small

#include <REGSTCRD+.H>

#include "function_declarations.h"

/* look for a free column, always return 4~7  */
INT8U	pbx_find_free_column(void )
{
	INT8U	data i=0x80;
	
	if( (columnFrees&0xF0)==0)
		return PBX_VALUE_INVALIDATE;

	for(i=4; i < 8; i++)
	{
		if(PBX_CHECK_FLAGS(columnFrees, i))
			return i;
	}
	return PBX_VALUE_INVALIDATE;
}

/* always return 0~3 */
INT8U	pbx_ip_find_free_device(void)
{
	INT8U	data i;
	
	if( (columnFrees&0x0F) == 0 )
		return PBX_VALUE_INVALIDATE;
	
	for(i=0; i<4; i++)
	{
		if(PBX_CHECK_FLAGS(columnFrees,i))
			return i;
	}
	
	return PBX_VALUE_INVALIDATE;
}

/* find dynamic row which can be only found in PSTN line Channels */
INT8U	pbx_find_free_row(void)
{
	INT8U	data x;

	if(rowFrees!=0)/* 0: all are busy */
	{
		for(x=STATE_IP_OFFSET-1; x>= PBX_FXS_COUNT; x--)
		{
			if( (rowFrees&SET_BIT(1, x) ) !=0 )
			{
				return x;
			}
		}
	}

	return PBX_VALUE_INVALIDATE;
}

/* find a row between 0 ~7 which is for a EXT */
INT8U	pbx_find_free_row_ext(void)
{
	INT8U	data x, y=0x01;

	if(rowFrees!=0)/* 0: all are busy */
	{
		for(x=0; x< PBX_FXS_COUNT; x++)
		{
			if((rowFrees&y) !=0 )
			{
				return x;
			}
			y=y<<1;
		}
	}

	return PBX_VALUE_INVALIDATE;
}

#if 0
typedef	enum
{
	ROW_2_ROW,
	ROW_2_COL,
	COL_2_ROW,
	COL_2_COL
}CONNECT_TYPE;

static INT8U	_find_connect_mode(INT8U x, INT8U y)
{
	if(x<CHANNEL_COL_OFFSET)
	{
		if(y<CHANNEL_COL_OFFSET)
			return ROW_2_ROW;
		else
			return ROW_2_COL;
	}
	else
	{
		if(y<CHANNEL_COL_OFFSET)
			return COL_2_ROW;
		else
			return COL_2_COL;
	}
	return PBX_VALUE_INVALIDATE;
}

void	pbx_channel_connect(INT8U rowOrCol, INT8U colOrRow)
{
	INT8U 	data	mode = _find_connect_mode(rowOrCol, colOrRow);
	INT8U	data	peer;

	if(mode== ROW_2_ROW)
	{
		peer = pbx_find_free_column();
		CONNECT(rowOrCol, peer);
		CONNECT(colOrRow, peer);
		connectTables[rowOrCol] = peer;
		connectTables[colOrRow] = peer;
	}
	else if(mode == ROW_2_COL)
	{
		CONNECT(rowOrCol, colOrRow-CHANNEL_COL_OFFSET);
		connectTables[rowOrCol] = PBX_VALUE_INVALIDATE;
	}
	else if(mode == COL_2_ROW)
	{
		CONNECT(colOrRow, rowOrCol-CHANNEL_COL_OFFSET);
		connectTables[colOrRow] = PBX_VALUE_INVALIDATE;
	}
	else if(mode == COL_2_COL )
	{
		peer = pbx_find_free_row();
		CONNECT(peer, rowOrCol-CHANNEL_COL_OFFSET);
		CONNECT(peer, colOrRow-CHANNEL_COL_OFFSET);
		connectTables[peer] = SET_BIT(rowOrCol, 4)|colOrRow);
	}
	else
	{
	}
		
}

void	pbx_channel_disconnect(INT8U rowOrCol, INT8U colOrRow)
{
	INT8U 	data mode = _find_connect_mode(rowOrCol, colOrRow);
	INT8U	data	x, y, i;

	if(mode== ROW_2_ROW)
	{
		x = connectTables[rowOrCol];
		DISCONNECT(rowOrCol, x);
		DISCONNECT(colOrRow, y);
		connectTables[rowOrCol] = PBX_VALUE_INVALIDATE;
		connectTables[colOrRow] = PBX_VALUE_INVALIDATE;
	}
	else if(mode == ROW_2_COL)
	{
		DISCONNECT(rowOrCol, colOrRow);
		connectTables[rowOrCol] = PBX_VALUE_INVALIDATE;
	}
	else if(mode == COL_2_ROW)
	{
		CONNECT(colOrRow, rowOrCol);
		connectTables[colOrRow] = PBX_VALUE_INVALIDATE;
	}
	else if(mode == COL_2_COL )
	{
//		for(i= STATE_IP_OFFSET; i< SWITCH_ARRAY_WIDTH; i++)
		for(i= 0; i< SWITCH_ARRAY_WIDTH; i++)
		{
			x = PBX_GET_HIGH_NIBBLE(connectTables[i]);
			y = PBX_GET_LOW_NIBBLE(connectTables[i]);
			if(x== rowOrCol || y == rowOrCol)
			{
				break;
			}
		}

		CONNECT(i, rowOrCol);
		CONNECT(i, colOrRow);
		connectTables[i] = PBX_VALUE_INVALIDATE;
	}
	else
	{
	}
		
}
#endif

