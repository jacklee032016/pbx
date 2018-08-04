#ifndef	__VAR_DECLARATIONS_H__
#define	__VAR_DECLARATIONS_H__

/* data : directly access internal memory, 128 byte */
extern	data	INT8U		uartCmdSeq;

extern	data	INT8U		cutresult;
extern	data	INT8U		combyte;
extern	data	INT8U		backsoundbuf;
extern	INT8U	data		cutbuf[PBX_FXO_COUNT];
extern	INT16U	xdata	TrackVoice_jishi[PBX_FXO_COUNT];	/*中继无声音计时*/
	
extern	data	INT8U		ipGetConfigIndex;
extern	data	INT8U		ipGetConfigItemIndex;
extern	data		INT8U			uartCmdSeq;
extern	data		INT8U			uartTxInIndex;				/* used by Main Loop */
extern	data		INT8U			uartTxOutIndex;				/* used by ISR */
extern	data		INT8U			uartTxBuffersPosition;	

extern	data		INT8U			uartRxInIndex;
extern	data		INT8U			uartRxOutIndex;
extern	data		INT8U			uartRxBuffersPosition;
extern	bdata	INT8U 			pbxTimerFlags;		/* which uart rx buffer is OK, every bit represent a buffer  */

extern	xdata		uart_cmd_t		uartTxCmds[UART_TX_BUFFER_COUNT];
extern	xdata		uart_cmd_t		uartRxCmds[UART_BUFFER_COUNT];

extern	device_status_t	xdata	deviceStatus[PBX_DEVICE_COUNT];
extern	INT8U			xdata	ringerTimer[PBX_FXS_COUNT];
extern	INT8U			xdata	holdId[PBX_FXS_COUNT];	/* for CallWait/CallKeep for every EXT */
extern	INT32U			xdata	conferenceMember;
extern	INT8U			xdata	pstnPending[PBX_FXO_COUNT];
#if 0
extern	INT8U			xdata	pstnRingerOnTimer[PBX_FXO_COUNT];
extern	INT8U			xdata	pstnRingerOffTimer[PBX_FXO_COUNT];
#else
extern	INT8U			xdata	pstnRingerTimeout[PBX_FXO_COUNT];
extern	INT8U			xdata	pstnRingerCounter[PBX_FXO_COUNT];
#endif
//extern	INT8U			xdata	connectTables[SWITCH_ARRAY_WIDTH];		/* switch array width */

/* Paged External memory, 256 bytes */
/* EXTs 0~3 has connected to decoding channel 4~7 permenently(not reallocated)
* IDLE : first byte is 0XFF; second byte is length of decoded phone number
*/

extern	encode_channel_t		xdata	encodingChannels[ENCODE_CHANNEL_NUMBER];

extern	phone_digits_t			xdata	callerIdData[PBX_FXO_COUNT];
extern	decode_channel_t		xdata	decodingChannels[2*DECODE_CHANNEL_NUMBER];

extern	xdata	INT8U		zhanghao_jishi[2];//

/* xdata : external data memory, 64k bytes */

//extern	xdata	INT8U			meeting_reg[PBX_DEVICE_COUNT+1];
//extern	xdata	INT8U		group_call[4][9];


extern	INT8U		data		columnFrees;
extern	INT16U		data		rowFrees;


/* bit access data memory, 16 bytes */
extern	bdata	INT8U		sign2;
extern	bdata	INT8U		hasDecodedNumberFlags; /* whether decoding channel has phone number, 1 bit for a decoding channel */


extern	bdata	INT8U		dtmfCodingChannelsFree;	/*高4位未用,已分配给其他位用*/

extern	bit	dtmfCodingChanneFree_1;
extern	bit	dtmfCodingChanneFree_2;
extern	bit	dtmfCodingChanneFree_3;
extern	bit	dtmfCodingChanneFree_4;
extern	bit	bool25msFlag;
extern	bit	bool50msFlag;
extern	bit	bool100msFlag;
extern	bit	bool400msFlag;
extern	bit	bool1MinuteFlag;
extern	bit	flagIpRecord;
extern	bit	flag_jo;

extern	bit	nowIsNightServiceTime;
extern	bit	callerIdSendFlags;

extern	bit	boolCallIdMode;		/* FSK : 1; DTMF : 0 */
extern	bit	boolNoDialAction;	/* zzj ? what is zzj */
extern	bit	boolTrunkMode;
extern	bit	boolGroupCall;
extern	bit	boolDifferRinging;
extern	bit	boolLine2Line;
extern	bit	boolSpecialFirst;
extern	bit	boolMechanism;

extern	bit	fskFirstByteFlag;
extern	bit	fskWaitStopFlag;

//#define	variable_initial
extern	data	INT8U		buzzerTimeout;//蜂鸣器响停间隔计时

extern	data	INT8U		pbxCurrentTime[8];		//时间缓冲区
extern	data	INT8U		ringerOffPending;

extern	pbx_base_t			xdata		baseConfigs;
extern	pbx_ext_base_t		xdata		extConfigs;
extern	pbx_ext_transfer_t		xdata		extTransfers;

extern	pbx_base_t			code			defaultBaseConfigs;
extern	pbx_ext_base_t		code			defaultExtConfigs;

extern	ext_transfer_digits 		code			defaultTransferIp;
extern	ext_transfer_digits 		code			defaultTransferPstn;
extern	ext_transfer_digits 		code			defaultTransferUnknown;

extern	pstn_param_t			code			phoneParams[2];
extern	pbx_ver_t			code			verInfo;

extern	pbx_hw_status		data			pbxHwStatus;

extern	fsk_decoder			data			fskDecoder;

#if 0
extern	bit        boolFm2408SCL;
extern	bit        boolFm2408SDA;
#endif


#if WITH_PSTN_FSK_DATA_DEBUG
extern	INT8U	xdata d[16];
#endif

#endif

