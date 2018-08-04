/*
* $Id: globalVariables.h,v 1.22 2007/07/21 21:26:08 lizhijie Exp $
* Variables definations which can be only included in main.c 
*/
#ifndef	__GLOBAL_VARIABLES_H__
#define	__GLOBAL_VARIABLES_H__

/* data : directly access internal memory, 128 byte */

INT8U	data		cutresult;
INT8U	data		combyte;
INT8U	data		backsoundbuf;
INT8U	data		cutbuf[PBX_FXO_COUNT];
INT16U	xdata	TrackVoice_jishi[PBX_FXO_COUNT];	/*中继无声音计时*/
	
INT8U	data			ipGetConfigIndex;
INT8U	data			ipGetConfigItemIndex;

INT8U	data			uartCmdSeq;
INT8U	data			uartTxInIndex;				/* used by Main Loop */
INT8U	data			uartTxOutIndex;				/* used by ISR */
INT8U	data			uartTxBuffersPosition;

/* low nibble is index for ISR, high nibble is index for main loop */
INT8U	data			uartRxInIndex;				/* used by ISR */
INT8U	data			uartRxOutIndex;				/* used by Main Loop */
INT8U	data			uartRxBuffersPosition;

uart_cmd_t	xdata			uartTxCmds[UART_TX_BUFFER_COUNT];
uart_cmd_t	xdata			uartRxCmds[UART_BUFFER_COUNT];


device_status_t	xdata	deviceStatus[PBX_DEVICE_COUNT];
//INT8U	xdata			connectTables[SWITCH_ARRAY_WIDTH];		/* switch array width */
/* used as Flash Timer( in state of IN_CALL ) and Ringer Off Timer( in state of RINGING) 
* bit 7 is used as type for Flash(1) or Ringer(0)
*/
INT8U			xdata	ringerTimer[PBX_FXS_COUNT];
INT8U			xdata	holdId[PBX_FXS_COUNT];	/* for CallWait/CallKeep for every EXT */
INT32U			xdata	conferenceMember;

INT8U			xdata	pstnPending[PBX_FXO_COUNT];
#if 0
INT8U			xdata	pstnRingerOnTimer[PBX_FXO_COUNT];
INT8U			xdata	pstnRingerOffTimer[PBX_FXO_COUNT];
#else
INT8U			xdata	pstnRingerTimeout[PBX_FXO_COUNT];
INT8U			xdata	pstnRingerCounter[PBX_FXO_COUNT];
#endif

/* Paged External memory, 256 bytes */

/* xdata : external data memory, 64k bytes */
encode_channel_t		xdata	encodingChannels[ENCODE_CHANNEL_NUMBER];
	
phone_digits_t			xdata		callerIdData[PBX_FXO_COUNT];
/* EXTs 0~3 has connected to decoding channel 4~7 permenently(not reallocated)
* IDLE : first byte is 0XFF; second byte is length of decoded phone number
*/
decode_channel_t		xdata	decodingChannels[2*DECODE_CHANNEL_NUMBER];

//phone_digits_t			xdata	dialingDigits[4];

INT8U		xdata	zhanghao_jishi[2];//


/* time length of call connection */

//INT8U		xdata		meeting_reg[PBX_DEVICE_COUNT+1];
//INT8U		xdata		group_call[4][9];

INT8U		data			columnFrees;
/* only 12 bits are used to represent 15 rows which from 0 to 11 */
INT16U		data			rowFrees;


/*************************** bit access data memory, 16 bytes *******************/
INT8U	bdata		hasDecodedNumberFlags; /* whether decoding channel has phone number, 1 bit for a decoding channel */


INT8U	bdata		sign2;
INT8U	bdata		dtmfCodingChannelsFree;	/*高4位未用,已分配给其他位用*/
INT8U 	bdata		pbxTimerFlags;

sbit	dtmfCodingChanneFree_1	=	dtmfCodingChannelsFree^0;
sbit	dtmfCodingChanneFree_2	=	dtmfCodingChannelsFree^1;
sbit	dtmfCodingChanneFree_3	=	dtmfCodingChannelsFree^2;
sbit	dtmfCodingChanneFree_4	=	dtmfCodingChannelsFree^3;
sbit	nowIsNightServiceTime		=	dtmfCodingChannelsFree^4;
sbit	callerIdSendFlags			=	dtmfCodingChannelsFree^5;

sbit	bool25msFlag				=	pbxTimerFlags^0;
sbit	bool50msFlag				=	pbxTimerFlags^1;
sbit	bool100msFlag				=	pbxTimerFlags^2;
sbit	bool400msFlag				=	pbxTimerFlags^3;
sbit	bool1MinuteFlag				=	pbxTimerFlags^4;
sbit	flagIpRecord				=	pbxTimerFlags^5;
sbit	fskWaitStopFlag				=	pbxTimerFlags^6;
sbit	fskFirstByteFlag				=	pbxTimerFlags^7;


sbit	boolTrunkMode				=	sign2^MISC_BIT_TRUNK_MODE;
sbit	boolNoDialAction				=	sign2^MISC_BIT_NODIAL;
sbit	boolLine2Line				=	sign2^MISC_BIT_LINE2LINE;
sbit	boolDifferRinging			=	sign2^MISC_BIT_DIFFER_RINGING;
sbit	boolCallIdMode				=	sign2^MISC_BIT_CALL_ID;
sbit	boolGroupCall				=	sign2^MISC_BIT_GROUP_CALL;
sbit	boolSpecialFirst				=	sign2^MISC_BIT_SPECIAL_FIRST;
sbit	boolMechanism				=	sign2^MISC_BIT_RESERVED;



pbx_base_t			xdata		baseConfigs;
pbx_ext_base_t		xdata		extConfigs;
#if 1
pbx_ext_transfer_t		xdata		extTransfers;
#else
ext_transfer_digits		xdata	leftTransfers[PBX_FXS_COUNT];
ext_transfer_digits		xdata	denyCallInTransfers[PBX_FXS_COUNT];
ext_transfer_digits		xdata	nightTransfers[PBX_FXS_COUNT];
ext_transfer_digits		xdata	unconditionalTransfers[PBX_FXS_COUNT];
#endif

pbx_hw_status		data			pbxHwStatus;

/* compiler directive : be careful */
#define	variable_initial

INT8U	data		buzzerTimeout;		/* in unit of 50ms */

INT8U	data		pbxCurrentTime[8];		//时间缓冲区
INT8U	data		ringerOffPending;

fsk_decoder	data		fskDecoder;

#endif

