/*
* $Id: cfgPbx.c,v 1.6 2007/09/08 18:55:47 lizhijie Exp $
*/

#pragma	ot(9,speed)
#pragma small

#include <REGSTCRD+.H>
#include "declarations.h"


void	pbx_reset_factory_default(void)
{
	INT8U	data i;
	pbx_x_str_copy((INT8U *)&baseConfigs, (INT8U *)&defaultBaseConfigs, sizeof(pbx_base_t) );
	pbx_x_str_copy((INT8U *)&extConfigs, 	(INT8U *)&defaultExtConfigs, sizeof(pbx_ext_base_t) );
	for(i=0;i<PBX_FXS_COUNT;i++)
	{
#if 0
		pbx_x_str_copy((INT8U *)&extTransfers.leftTransfers[i], (INT8U *)&defaultTransferIp, sizeof(ext_transfer_digits) );
		pbx_x_str_copy((INT8U *)&extTransfers.nightTransfers[i], (INT8U *)&defaultTransferPstn, sizeof(ext_transfer_digits) );
#else
		pbx_x_str_copy((INT8U *)&extTransfers.leftTransfers[i], (INT8U *)&defaultTransferUnknown, sizeof(ext_transfer_digits) );
		pbx_x_str_copy((INT8U *)&extTransfers.nightTransfers[i], (INT8U *)&defaultTransferUnknown, sizeof(ext_transfer_digits) );
#endif		
		pbx_x_str_copy((INT8U *)&extTransfers.unconditionalTransfers[i], (INT8U *)&defaultTransferUnknown, sizeof(ext_transfer_digits) );
	}

#if 0
	extTransfers.unconditionalTransfers[1].type = EXT_TRANSFER_TYPE_PSTN;
	extTransfers.unconditionalTransfers[1].digits.length = 4;
	extTransfers.unconditionalTransfers[1].digits.value[0] = SET_BIT(0X05,4)|0X04;
	extTransfers.unconditionalTransfers[1].digits.value[1] = SET_BIT(0x03,4)|0X06;
#endif

	globalConfig = baseConfigs.globalOptions;

	pbxHwStatus.nvRamWriteFlags = 0xFF;
	saveNvram();
	
	pbx_2_beep();
}

void	restoreFromNvram(void )
{
	eeprom_read_buffer((INT8U xdata *)&baseConfigs, NVRAM_PAGE_OFFSET_GLOBAL, sizeof(pbx_base_t) );
	eeprom_read_buffer((INT8U xdata *)&extConfigs, NVRAM_PAGE_OFFSET_EXT, sizeof(pbx_ext_base_t) );
	eeprom_read_buffer((INT8U xdata *)&extTransfers.leftTransfers[0], NVRAM_PAGE_OFFSET_LEFT, sizeof(ext_transfer_digits)*PBX_FXS_COUNT);
	eeprom_read_buffer((INT8U xdata *)&extTransfers.unconditionalTransfers[0], NVRAM_PAGE_OFFSET_UNCONDITIONAL, sizeof(ext_transfer_digits)*PBX_FXS_COUNT );
	eeprom_read_buffer((INT8U xdata *)&extTransfers.nightTransfers[0], NVRAM_PAGE_OFFSET_NIGHT, sizeof(ext_transfer_digits)*PBX_FXS_COUNT );

	if(baseConfigs.msgicString[0] != defaultBaseConfigs.msgicString[0] ||
		baseConfigs.msgicString[1] != defaultBaseConfigs.msgicString[1] )
	{
		pbx_reset_factory_default();
	}

	globalConfig = baseConfigs.globalOptions;
}		

void	saveNvram(void )
{
	if(NVRAM_CHECK_GLOBAL() )
	{
//		pbx_uart_debug((INT8U xdata *)&baseConfigs, 13);
		if(eeprom_write_buffer((INT8U xdata *)&baseConfigs, NVRAM_PAGE_OFFSET_GLOBAL, sizeof(pbx_base_t) )== FALSE)
			pbxBuzzer(4);
//		pbx_uart_debug((INT8U xdata *)&baseConfigs, 13);
		
		NVRAM_CLEAR_GLOBAL();
	}
	
	if(NVRAM_CHECK_EXT() )
	{
//		pbx_uart_debug((INT8U xdata *)&extConfigs, 13);
		if(eeprom_write_buffer((INT8U xdata *)&extConfigs, NVRAM_PAGE_OFFSET_EXT, sizeof(pbx_ext_base_t) )==FALSE)
			pbxBuzzer(4);
//		pbx_uart_debug((INT8U xdata *)&extConfigs, 13);
		
		NVRAM_CLEAR_EXT();
	}
	
	if(NVRAM_CHECK_LEFT() )
	{
//		pbx_uart_debug((INT8U xdata *)&extTransfers.leftTransfers[0], 13);
		if(eeprom_write_buffer((INT8U xdata *)&extTransfers.leftTransfers[0], NVRAM_PAGE_OFFSET_LEFT, sizeof(ext_transfer_digits)*PBX_FXS_COUNT )==FALSE)
			pbxBuzzer(4);
//		pbx_uart_debug((INT8U xdata *)&extTransfers.leftTransfers[0], 13);
		
		NVRAM_CLEAR_LEFT();
	}
	
	if(NVRAM_CHECK_UNCONDITIONAL() )
	{
//		pbx_uart_debug((INT8U xdata *)&extTransfers.unconditionalTransfers[0], 13);
		if(eeprom_write_buffer((INT8U xdata *)&extTransfers.unconditionalTransfers[0], NVRAM_PAGE_OFFSET_UNCONDITIONAL, sizeof(ext_transfer_digits)*PBX_FXS_COUNT )==FALSE)
			pbxBuzzer(4);
//		pbx_uart_debug((INT8U xdata *)&extTransfers.unconditionalTransfers[0], 13);
		
		NVRAM_CLEAR_UNCONDITIONAL();
	}
	
	if(NVRAM_CHECK_NIGHT() )
	{
//		pbx_uart_debug((INT8U xdata *)&extTransfers.nightTransfers[0], 13);
		if(eeprom_write_buffer((INT8U xdata *)&extTransfers.nightTransfers[0], NVRAM_PAGE_OFFSET_NIGHT, sizeof(ext_transfer_digits)*PBX_FXS_COUNT )==FALSE)
			pbxBuzzer(4);
//		pbx_uart_debug((INT8U xdata *)&extTransfers.nightTransfers[0], 13);
		
		NVRAM_CLEAR_NIGHT();
	}

}

void	pbx_reset_systime(void)
{
	pbxHwStatus.sysTime.second = 0x45;
	pbxHwStatus.sysTime.minute = 0x45;
	pbxHwStatus.sysTime.hour = 0x23;
	pbxHwStatus.sysTime.date = 0x23;
	pbxHwStatus.sysTime.month = 0x12;
	pbxHwStatus.sysTime.day= 0x07;
	pbxHwStatus.sysTime.year = 0x07;
	pbxHwStatus.sysTime.protect = 0;
	
#if 0//WITH_UART_DEBUG
	uartTxCmds[responseInIndex].value[0] = UART_DEBUG_STSTIME;
	uartTxCmds[responseInIndex].value[1] = pbxHwStatus.sysTime.second;
	uartTxCmds[responseInIndex].value[2] = pbxHwStatus.sysTime.minute;
	uartTxCmds[responseInIndex].value[3] = pbxHwStatus.sysTime.hour;
	uart_tx_cmd(CMD_UART_DEBUG, 4);
#endif
	pbx_systime_write();
}

void	resetAllDataStructures(void)
{
	INT8U data i;
	
	uart.ipGetConfigIndex = PBX_VALUE_INVALIDATE;
	uart.ipGetConfigItemIndex=0;
	uart.cmdSeq=0;
	uart.requestInIndex = 0;
	uart.requestOutIndex = PBX_VALUE_INVALIDATE;
	uart.requestBuffersPosition = 0;
	uart.responseInIndex = 0;
	uart.responseOutIndex = PBX_VALUE_INVALIDATE;
	uart.responseBuffersPosition = 0;


	pbxHwStatus.Counter50ms = 0;
	pbxHwStatus.Counter100ms = 0;
	pbxHwStatus.Counter500ms = 0;
	pbxHwStatus.CounterMinutes = 0;
	pbxHwStatus.dtmfDecodingStatus = 0;
	pbxHwStatus.buzzerTimeout = 0;
	pbxHwStatus.rowFrees = 0x0FFF;
	pbxHwStatus.columnFrees = 0xFF;
	pbxHwStatus.nvRamWriteFlags = 0;
	PBX_PROGRAM_MODE_CANCEL();

	for(i=0; i< PBX_FXS_COUNT; i++)
	{
		extDevices.hookTimer[i] = 0;
		extDevices.status[i] = EXT_STATE_IDLE;
		extDevices.timeout[i] = 0;
		extDevices.timeoutCounter[i] = 0;
		extDevices.holdId[i] = PBX_VALUE_INVALIDATE;
	}

	pstnDevices.busyDetected = 0x00;
	for(i=0; i< PBX_FXO_COUNT; i++)
	{
		pstnDevices.hwStatus[i] = PSTN_HW_STATE_ONHOOK;
		pstnDevices.timeout[i] = 0;
#if WITH_PSTN_HW_RINGER_DETECT
		pstnDevices.ringerOnCounter[i] = 0;
		pstnDevices.ringerOffCounter[i] = 0;
#else
		pstnDevices.ringerCounter[i] = 0;
#endif		
		pstnDevices.vadTimeCount[i]=0;
		pstnDevices.cutbuf[i] = 0;
		pstnDevices.busyNum[i] = 0;

	}

	for(i=0; i< sizeof(callerIdData)/sizeof(decode_channel_t); i++)
	{
		callerIdData[i].owner = PBX_VALUE_INVALIDATE;
		callerIdData[i].digits.length = 0;
	}
	
	for(i=0;i< sizeof(callDevices)/sizeof(pbx_call_device_t); i++)
	{
		callDevices[i].status =PBX_STATE_IDLE;
		CALL_MY_PEER(i) = PBX_VALUE_INVALIDATE;
		CALL_MY_FLASH(i) = PBX_VALUE_INVALIDATE;
		CALL_TIMER_START(i,  TIMEOUT_NEVER);

		callDevices[i].currentEvent = PBX_EVENT_UNKNOWN;
		callDevices[i].phyPeerId = PBX_VALUE_INVALIDATE;
#if 0		
		callDevices[i].devEvent = DEV_EVENT_UNKNOWN;
#endif
	}

	dtmfCodingChannelsFree = 0x00; 
	pbxTimerFlags = 0x00;
	
	fskDecoder.currentChannel = PBX_VALUE_INVALIDATE;
	
	conferenceMember = 0;


//	for(i=0; i<sizeof(encodingChannels)/sizeof(encode_channel_t) ;i++)
	for(i=0; i<ENCODE_CHANNEL_NUMBER; i++)
	{
		encodingChannels[i].owner = PBX_VALUE_INVALIDATE;
		encodingChannels[i].timeout = 0;
		encodingChannels[i].point = 0;
	}
	
//	for(i=0;i< sizeof(decodingChannels)/sizeof(decode_channel_t); i++)
	for(i=0;i< DECODE_CHANNEL_NUMBER; i++)
	{/* pre-allocate for the first DTMF decoder */
		decodingChannels[i].owner = PBX_VALUE_INVALIDATE;
		decodingChannels[i+DECODE_CHANNEL_NUMBER].owner = i;
		decodingChannels[i+DECODE_CHANNEL_NUMBER].digits.length = 0;
	}	

	restoreFromNvram();

	pbx_reset_systime();

}

