/*
* $Id: pbx_voice_channels.c,v 1.6 2007/07/22 20:48:13 lizhijie Exp $
*/
/*
* Build Voice Channel between Caller and Callee
*/

#pragma	ot(9,speed)
#pragma small

#include <REGSTCRD+.H>
#include "function_declarations.h"


#define	_CALL_LOCAL_2_LOCAL	0	/* LOCAL : exts and lines */
#define	_CALL_LOCAL_2_IP		1	/* LOCAL-->IP and IP-->LOCAL */
#define	_CALL_IP_2_IP			2	/* IP <--> IP*/

static INT8U	_find_call_mode(INT8U x, INT8U y)
{
	if( IS_ROW_DEVICE(x))
	{
		if( IS_ROW_DEVICE(y) )
			return _CALL_LOCAL_2_LOCAL;
		else
			return _CALL_LOCAL_2_IP;
	}
	else
	{
		if( IS_ROW_DEVICE(y) )
			return _CALL_LOCAL_2_IP;
		else
			return _CALL_IP_2_IP;
	}
	return PBX_VALUE_INVALIDATE;
}

/* */
BOOL	pbx_voice_channel_connect(INT8U x, INT8U y)
{
	INT8U data mode, peer = PBX_VALUE_INVALIDATE;
	BOOL	isOK = FALSE;

	mode = _find_call_mode( x, y);
	if(mode == _CALL_LOCAL_2_LOCAL)
	{/* x and y are both in row, so find a free column to connect them */
		peer = pbx_find_free_column();
		if(peer != PBX_VALUE_INVALIDATE )
		{
			CONNECT(x, peer);
			CONNECT(y, peer);

			PBX_COLUMN_BUSY(peer);
#if UART_DEBUG
			uart_debug_2_int_value(UART_DEBUG_COL_RESOURCE, peer, 1);
#endif
			isOK = TRUE;
		}
	}
	else if(mode == _CALL_LOCAL_2_IP)
	{/* x and y are in row and column */
		if( IS_ROW_DEVICE(x) )
			CONNECT(x, GET_IP_CHANNEL_INDEX(y) );
		else
			CONNECT(y, GET_IP_CHANNEL_INDEX(x) );
		
		isOK = TRUE;
	}
	else	/*ip-ip*/
	{
		peer = pbx_find_free_row();
		if(peer != PBX_VALUE_INVALIDATE)
		{
			CONNECT(peer, GET_IP_CHANNEL_INDEX(x) );
			CONNECT(peer, GET_IP_CHANNEL_INDEX(y) );

			PBX_ROW_BUSY(peer);
			isOK = TRUE;
		}
	}

#if 1	
//	if(peer != PBX_VALUE_INVALIDATE)
	{
		deviceStatus[x].phyPeerId = peer;
		deviceStatus[y].phyPeerId = peer;
	}
#endif

	return isOK;
}

void	pbx_voice_channel_broken(INT8U x, INT8U y)
{
	INT8U data mode, peer = PBX_VALUE_INVALIDATE;
	
	peer = deviceStatus[x].phyPeerId;
	mode = _find_call_mode(x, y);
	if( (mode == _CALL_LOCAL_2_LOCAL) && (peer!= PBX_VALUE_INVALIDATE) )
	{
		DISCONNECT(x, peer);
		DISCONNECT(y, peer);
		PBX_COLUMN_FREE(peer);
#if UART_DEBUG
		uart_debug_2_int_value(UART_DEBUG_COL_RESOURCE, peer, 0);
#endif
	}
	else if(mode == _CALL_LOCAL_2_IP)
	{
		if( IS_ROW_DEVICE(x) )
			DISCONNECT(x, GET_IP_CHANNEL_INDEX(y) );
		else
			DISCONNECT(y, GET_IP_CHANNEL_INDEX(x) );
	}
	else	 if(peer != PBX_VALUE_INVALIDATE )
	{
		DISCONNECT(peer, GET_IP_CHANNEL_INDEX(x) );
		DISCONNECT(peer, GET_IP_CHANNEL_INDEX(y) );
		PBX_ROW_FREE(peer);
	}

#if 1	
	deviceStatus[x].phyPeerId = PBX_VALUE_INVALIDATE;
	deviceStatus[y].phyPeerId = PBX_VALUE_INVALIDATE;
#endif	
}

#if 0
//恢复断开的通话连接
void resume_talk(INT8U pdata *x,INT8U pdata *y)/*x 原通话方状态寄存器地址*/
{
	switch(x[2])
	{
		case 05:
			x[0]=05;
			y[0]=06;	
			break;
		case 06:
			x[0]=06;
			y[0]=05;	
			break;
		case 31:
		case 92:
			x[0]=x[2];
			y[0]=8;
			break;
		case 32:
		case 97:
			x[0]=x[2];
			y[0]=07;
		default:
			break;
	}
	x[2]=0xff;
}
#endif

