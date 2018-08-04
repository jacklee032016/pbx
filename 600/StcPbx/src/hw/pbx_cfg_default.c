/*
* $Id: pbx_cfg_default.c,v 1.27 2007/07/21 21:26:28 lizhijie Exp $
*/

#pragma	ot(9,speed)
#pragma small

#include <REGSTCRD+.H>
#include "function_declarations.h"


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

	sign2 = baseConfigs.globalOptions;

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

	sign2 = baseConfigs.globalOptions;
}		

void	saveNvram(void )
{
	if(NVRAM_CHECK_GLOBAL() )
	{
//		pbx_uart_debug((INT8U xdata *)&baseConfigs, 13);
		if(eeprom_write_buffer((INT8U xdata *)&baseConfigs, NVRAM_PAGE_OFFSET_GLOBAL, sizeof(pbx_base_t) )== FALSE)
			pbx_OK();
//		pbx_uart_debug((INT8U xdata *)&baseConfigs, 13);
		
		NVRAM_CLEAR_GLOBAL();
	}
	
	if(NVRAM_CHECK_EXT() )
	{
//		pbx_uart_debug((INT8U xdata *)&extConfigs, 13);
		if(eeprom_write_buffer((INT8U xdata *)&extConfigs, NVRAM_PAGE_OFFSET_EXT, sizeof(pbx_ext_base_t) )==FALSE)
			pbx_OK();
//		pbx_uart_debug((INT8U xdata *)&extConfigs, 13);
		
		NVRAM_CLEAR_EXT();
	}
	
	if(NVRAM_CHECK_LEFT() )
	{
//		pbx_uart_debug((INT8U xdata *)&extTransfers.leftTransfers[0], 13);
		if(eeprom_write_buffer((INT8U xdata *)&extTransfers.leftTransfers[0], NVRAM_PAGE_OFFSET_LEFT, sizeof(ext_transfer_digits)*PBX_FXS_COUNT )==FALSE)
			pbx_OK();
//		pbx_uart_debug((INT8U xdata *)&extTransfers.leftTransfers[0], 13);
		
		NVRAM_CLEAR_LEFT();
	}
	
	if(NVRAM_CHECK_UNCONDITIONAL() )
	{
//		pbx_uart_debug((INT8U xdata *)&extTransfers.unconditionalTransfers[0], 13);
		if(eeprom_write_buffer((INT8U xdata *)&extTransfers.unconditionalTransfers[0], NVRAM_PAGE_OFFSET_UNCONDITIONAL, sizeof(ext_transfer_digits)*PBX_FXS_COUNT )==FALSE)
			pbx_OK();
//		pbx_uart_debug((INT8U xdata *)&extTransfers.unconditionalTransfers[0], 13);
		
		NVRAM_CLEAR_UNCONDITIONAL();
	}
	
	if(NVRAM_CHECK_NIGHT() )
	{
//		pbx_uart_debug((INT8U xdata *)&extTransfers.nightTransfers[0], 13);
		if(eeprom_write_buffer((INT8U xdata *)&extTransfers.nightTransfers[0], NVRAM_PAGE_OFFSET_NIGHT, sizeof(ext_transfer_digits)*PBX_FXS_COUNT )==FALSE)
			pbx_OK();
//		pbx_uart_debug((INT8U xdata *)&extTransfers.nightTransfers[0], 13);
		
		NVRAM_CLEAR_NIGHT();
	}

}


void	power_on_rst(void)
{
	INT8U data i;
	PX0=1;		/*;设置中断优先级INT0、INT1，T1为高优先级*/
	PT1=1;
	EX0=1;		/*;设置中断允许INT0、INT1允许中断*/
	EA=1;	
	
#if WITH_FSK_MT88E39	
	IT0 = FALSE;		/* trigger with falling edge for MT88E39 */
	MT88E39_DCLK = FALSE;
#else
	IT0 = FALSE;		/* trigger in low level for SM8823AP */
#endif
	CTS = UART_CTS_ENABLE;
	
	uartCmdSeq=0;

	uartRxInIndex = 0;
	uartRxOutIndex = PBX_VALUE_INVALIDATE;
	uartRxBuffersPosition = 0;
	uartTxInIndex = 0;
	uartTxOutIndex = PBX_VALUE_INVALIDATE;
	uartTxBuffersPosition = 0;

	pbxTimerFlags&=0x00;
	rowFrees = 0x0FFF;
	columnFrees = 0xFF;
	
	cutresult=0;
	ipGetConfigIndex = PBX_VALUE_INVALIDATE;
	ipGetConfigItemIndex=0;

	buzzerTimeout = 0;

	pbxHwStatus.trackFreeStatus = 0x0F;
	pbxHwStatus.dtmfDecodingStatus = 0;

	pbxHwStatus.Counter50ms = 0;
	pbxHwStatus.Counter100ms = 0;
	pbxHwStatus.Counter400ms = 0;
	pbxHwStatus.CounterMinutes = 0;
	pbxHwStatus.nvRamWriteFlags = 0;
	PBX_PROGRAM_MODE_CANCEL();

	dtmfCodingChannelsFree|=0x0f; 

	nowIsNightServiceTime = FALSE;
	flagIpRecord =FALSE;
	
	fskDecoder.currentChannel = PBX_VALUE_INVALIDATE;
//	fskFirstByteFlag = TRUE;
//	fskWaitStopFlag = FALSE;
	
	cutbuf[0] = cutbuf[1] = cutbuf[2] = cutbuf[3] = 0x01 ;
	backsoundbuf = 0x00;

	
	for(i=0;i< sizeof(deviceStatus)/sizeof(device_status_t); i++)
	{
		deviceStatus[i].status =STATE_IDLE;
		deviceStatus[i].peerId =PBX_VALUE_INVALIDATE;
		deviceStatus[i].flashPeerId = PBX_VALUE_INVALIDATE;
		
		deviceStatus[i].timeout = 0;
		deviceStatus[i].timeoutCounter = 0;
		deviceStatus[i].currentEvent = PBX_EVENT_UNKNOWN;

		deviceStatus[i].phyPeerId = PBX_VALUE_INVALIDATE;
	}
	

	ringerOffPending=0;
	for(i=0; i< PBX_FXS_COUNT; i++)
	{
		holdId[i] = PBX_VALUE_INVALIDATE;
		ringerTimer[i] = 0;
	}
	conferenceMember = 0;

	for(i=0; i< PBX_FXO_COUNT; i++)
	{
		pstnPending[i] = PSTN_RINGER_DISABLE;
#if 0		
		pstnRingerOnTimer[i] = 0;
		pstnRingerOffTimer[i] = 0;
#else
		pstnRingerTimeout[i] = 0;
		pstnRingerCounter[i] = 0;
#endif
		TrackVoice_jishi[i]=0;	/* 中继无声音计时 初始化为0*/

		callerIdData[i].length = 0;
	}

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
}

