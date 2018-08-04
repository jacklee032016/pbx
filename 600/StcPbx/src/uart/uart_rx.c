/*
* $Id: uart_rx.c,v 1.20 2007/07/25 17:56:35 lizhijie Exp $
*/

#pragma	ot(9,speed)
#pragma small

#include <REGSTCRD+.H>
#include "function_declarations.h"
#include "_uart_handlers.h"

/* 0x3d */
void	uart_ip_pbx_get_all_cfgs(void)
{
#if 1
	if(ipGetConfigIndex == PBX_VALUE_INVALIDATE)
	{
		ipGetConfigIndex = 0;
		ipGetConfigItemIndex = 0;//PBX_VALUE_INVALIDATE;
	}
#else	
	UART_TX_CFG_BEGIN();
	UART_TX_CFG_BEGIN();
#endif
}

void uart_rx_no_value(INT8U cmd)
{
#if TEST_NVRAM
	INT8U xdata p[32] = 
	{
		0x11, 0x22, 0x44, 0x88,
		0x33, 0x66, 0x99, 0xAA,
		0x55, 0x77, 0xbb, 0xEE,
		0x56, 0x67, 0x78, 0x89, 
		
		0x33, 0x66, 0x99, 0xAA,
		0x55, 0x77, 0xbb, 0xEE,
		0x55, 0x77, 0xbb, 0xEE, 
		0xDD, 0xCC, 0x11, 0X22
	};
#endif

	switch(cmd)
	{
		case CMD_CFG_IP_PBX_RESET_ISP:
			RESET_PBX();
			break;
		case CMD_CFG_IP_PBX_GET_ALL_CFGS:
			uart_ip_pbx_get_all_cfgs();
			
#if TEST_NVRAM
			eeprom_write_buffer(p, NVRAM_PAGE_OFFSET_GLOBAL, 25 );
			pbx_x_str_copy(uartTxCmds[uartTxInIndex].value, p+7 , 13);
			uart_tx_cmd((cmd), 13);
			
			eeprom_read_buffer((INT8U xdata *)&p, NVRAM_PAGE_OFFSET_GLOBAL, 25 );
			pbx_x_str_copy(uartTxCmds[uartTxInIndex].value, p+7 , 13);
			uart_tx_cmd((cmd), 13);
#endif			
			break;
		case CMD_CFG_IP_PBX_FACTORY_DEFAULT:
			pbx_factory_default();
			break;
			
		case CMD_RECORD_REQUEST:
			PBX_IP_RECORD_MODE_ENTER();
			/* need to process problem of timeout if no phone is offhook */
			break;
		case CMD_RECORD_END:
			PBX_IP_RECORD_MODE_CANCEL();
			break;
		default:
			break;
	}
}

#if 0
void _uart_rx(void)
{
	INT8U length = 0;
	for(length=0; length<CMD_VALUE_MAX;length++)
	{
		if(uartRxCmds[uartRxOutIndex].value[length] == PBX_VALUE_INVALIDATE)
			break;
	}

	switch(length)
	{
		case 0:
			uart_rx_no_value(uartRxCmds[uartRxOutIndex].cmd);
			return;
			break;
		case 1:
			nvram_save_int_value(uartRxCmds[uartRxOutIndex].cmd, uartRxCmds[uartRxOutIndex].value[0] );
			return;
			break;
		case 2:
			nvram_save_2_int_value(uartRxCmds[uartRxOutIndex].cmd, uartRxCmds[uartRxOutIndex].value[0], uartRxCmds[uartRxOutIndex].value[1] );
			return;
			break;
		default:
			break;
	}
		
}
#endif

void	uart_rx(void)
{
	INT8U xdata length = 0;
	
	if(uartRxOutIndex != PBX_VALUE_INVALIDATE )
	{
	
		if(uartRxCmds[uartRxOutIndex].cmd != PBX_VALUE_INVALIDATE)
		{
			if(uartRxCmds[uartRxOutIndex].cmd <= CMD_CALL_RESET_ALL)
			{
				uart_call_rx_handle(uartRxCmds+uartRxOutIndex);
			}
			else if(uartRxCmds[uartRxOutIndex].cmd <= CMD_SELFTEST_BUSY)
			{
				self_test_handle(uartRxCmds+uartRxOutIndex);
			}
			else
			{
				for(length=0; length<CMD_VALUE_MAX;length++)
				{
					if(uartRxCmds[uartRxOutIndex].value[length] == PBX_VALUE_INVALIDATE)
						break;
				}

				switch(length)
				{
					case 0:
						uart_rx_no_value(uartRxCmds[uartRxOutIndex].cmd);
						break;
					case 1:
						nvram_save_int_value(uartRxCmds[uartRxOutIndex].cmd, uartRxCmds[uartRxOutIndex].value[0] );
						break;
					case 2:
						nvram_save_2_int_value(uartRxCmds[uartRxOutIndex].cmd, uartRxCmds[uartRxOutIndex].value[0], uartRxCmds[uartRxOutIndex].value[1] );
						break;
					default:
						nvram_save_values(uartRxCmds[uartRxOutIndex].cmd, uartRxCmds[uartRxOutIndex].value, length);
						break;
				}

				if(NVRAM_CHECK_GLOBAL() )
				{
					sign2 = baseConfigs.globalOptions ;
				}

			}
			
			uartRxCmds[uartRxOutIndex].cmd = PBX_VALUE_INVALIDATE;
		}

		length = uartRxOutIndex;
		
		uartRxOutIndex++;
		if(uartRxOutIndex == UART_BUFFER_COUNT)
			uartRxOutIndex = 0;
		
		if(uartRxInIndex == uartRxOutIndex)
		{/*block myself(main loop) read */
			uartRxOutIndex = PBX_VALUE_INVALIDATE;
		}
		
		if(uartRxInIndex == PBX_VALUE_INVALIDATE)
		{/* if ISR is blocked, wake it up and assign index of just used  */
			uartRxInIndex = length;
			CTS = UART_CTS_ENABLE;
		}
	}
}

