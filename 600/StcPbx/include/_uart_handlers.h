/*
* $Id: _uart_handlers.h,v 1.5 2007/06/22 20:23:09 lizhijie Exp $
*/
#ifndef	___UART_HANDLER_H__
#define	___UART_HANDLER_H__

#include "uart_cmd.h"


#define	UART_TX_CFG_BEGIN()	\
		uart_tx_cmd(CMD_CFG_PBX_IP_BEGIN,0)

#define	UART_TX_CFG_END()	\
		uart_tx_cmd(CMD_CFG_PBX_IP_END, 0)

#define	UART_TX_TRACK_MODE()	\
		uart_tx_int_value(CMD_CFG_TRACK_MODE, boolTrunkMode)

#define	UART_TX_NODIAL_ACTION()	\
		uart_tx_int_value(CMD_CFG_NODIAL_ACTION, boolNoDialAction)

#define	UART_TX_LINE_2_LINE()	\
		uart_tx_int_value(CMD_CFG_LINE2LINE, boolLine2Line)

#define	UART_TX_DIFFER_RINGING()	\
		uart_tx_int_value(CMD_CFG_DIFFER_RINGING, boolDifferRinging)

#define	UART_TX_CALLER_ID_MODE()	\
		uart_tx_int_value(CMD_CFG_CALLER_ID_MODE, boolCallIdMode )

#define	UART_TX_GROUP_CALL()	\
		uart_tx_int_value(CMD_CFG_GROUP_CALL, boolGroupCall)

#define	UART_TX_SPECIAL_SERVICE_FIRST()	\
		uart_tx_int_value(CMD_CFG_SPECIAL_FIRST, boolSpecialFirst)

BOOL	uart_call_rx_handle(uart_cmd_t xdata *value);

#endif

