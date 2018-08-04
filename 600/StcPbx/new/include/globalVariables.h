/*
* $Id: globalVariables.h,v 1.2 2007/08/05 21:11:38 lizhijie Exp $
* Variables definations which can be only included in main.c 
*/
#ifndef	__GLOBAL_VARIABLES_H__
#define	__GLOBAL_VARIABLES_H__
/* data : directly access internal memory, 128 byte */
uart_cmd_t			xdata			uartTxCmds[UART_TX_BUFFER_COUNT];
uart_cmd_t			xdata			uartRxCmds[UART_BUFFER_COUNT];
uart_t				data				uart;

pbx_call_device_t		xdata			callDevices[PBX_DEVICE_COUNT];
pbx_ext_device_t		xdata			extDevices;
pbx_pstn_device_t		xdata			pstnDevices;

pbx_hw_status		data				pbxHwStatus;


INT32U				xdata			conferenceMember;


/* Paged External memory, 256 bytes */

/* xdata : external data memory, 64k bytes */
encode_channel_t		xdata			encodingChannels[ENCODE_CHANNEL_NUMBER];
	
/* EXTs 0~3 has connected to decoding channel 4~7 permenently(not reallocated)
* IDLE : first byte is 0XFF; second byte is length of decoded phone number
*/
decode_channel_t		xdata			decodingChannels[2*DECODE_CHANNEL_NUMBER];
#if 0
phone_digits_t			xdata			callerIdData[PBX_FXO_COUNT];
#else
decode_channel_t		xdata			callerIdData[PBX_FXO_COUNT];
#endif

pbx_base_t			xdata			baseConfigs;
pbx_ext_base_t		xdata			extConfigs;
pbx_ext_transfer_t		xdata			extTransfers;

INT8U				xdata			zhanghao_jishi[2];//
fsk_decoder			data				fskDecoder;



/*************************** bit access data memory, 16 bytes *******************/
INT8U				bdata			globalConfig;
INT8U 				bdata			pbxTimerFlags;
INT8U				bdata			dtmfCodingChannelsFree;


sbit	boolTrunkMode				=	globalConfig^MISC_BIT_TRUNK_MODE;
sbit	boolNoDialAction				=	globalConfig^MISC_BIT_NODIAL;
sbit	boolLine2Line				=	globalConfig^MISC_BIT_LINE2LINE;
sbit	boolDifferRinging			=	globalConfig^MISC_BIT_DIFFER_RINGING;
sbit	boolCallIdMode				=	globalConfig^MISC_BIT_CALL_ID;
sbit	boolGroupCall				=	globalConfig^MISC_BIT_GROUP_CALL;
sbit	boolSpecialFirst				=	globalConfig^MISC_BIT_SPECIAL_FIRST;
sbit	boolMechanism				=	globalConfig^MISC_BIT_RESERVED;

sbit	bool25msFlag				=	pbxTimerFlags^0;
sbit	bool50msFlag				=	pbxTimerFlags^1;
sbit	bool100msFlag				=	pbxTimerFlags^2;
sbit	bool500msFlag				=	pbxTimerFlags^3;
sbit	bool1MinuteFlag				=	pbxTimerFlags^4;
sbit	flagIpRecord				=	pbxTimerFlags^5;
sbit	fskWaitStopFlag				=	pbxTimerFlags^6;
sbit	fskFirstByteFlag				=	pbxTimerFlags^7;

sbit	nowIsNightServiceTime		=	dtmfCodingChannelsFree^4;
sbit	callerIdSendFlags			=	dtmfCodingChannelsFree^5;

/* compiler directive : be careful */
#define	variable_initial

#endif

