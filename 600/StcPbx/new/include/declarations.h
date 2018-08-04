/*
* $Id: declarations.h,v 1.7 2007/09/03 21:09:19 lizhijie Exp $
*/
#ifndef	__FUNCTION_DECLARATIONS_H__
#define	__FUNCTION_DECLARATIONS_H__

#include "pbxHw.h"
#include "pbxMacros.h"
#include "uart_cmd.h"
#include "pbxTypes.h"

/* data : directly access internal memory, 128 byte */
extern	uart_cmd_t			xdata		uartTxCmds[UART_TX_BUFFER_COUNT];
extern	uart_cmd_t			xdata		uartRxCmds[UART_BUFFER_COUNT];
extern	uart_t				data			uart;

extern	pbx_call_device_t		xdata		callDevices[PBX_DEVICE_COUNT];
extern	pbx_ext_device_t		xdata		extDevices;
extern	pbx_pstn_device_t		xdata		pstnDevices;
extern	pbx_hw_status		data			pbxHwStatus;

extern	INT32U				xdata		conferenceMember;

/* Paged External memory, 256 bytes */
/* EXTs 0~3 has connected to decoding channel 4~7 permenently(not reallocated)
* IDLE : first byte is 0XFF; second byte is length of decoded phone number
*/

extern	encode_channel_t		xdata		encodingChannels[ENCODE_CHANNEL_NUMBER];

extern	decode_channel_t		xdata		callerIdData[PBX_FXO_COUNT];
extern	decode_channel_t		xdata		decodingChannels[2*DECODE_CHANNEL_NUMBER];

extern	INT8U				xdata		zhanghao_jishi[2];//

/* xdata : external data memory, 64k bytes */
extern	fsk_decoder			data			fskDecoder;


/* bit access data memory, 16 bytes */
extern	INT8U				bdata		globalConfig;
extern	INT8U 				bdata		pbxTimerFlags;		/* which uart rx buffer is OK, every bit represent a buffer  */
extern	INT8U				bdata		dtmfCodingChannelsFree;	/*高4位未用,已分配给其他位用*/

extern	bit	bool25msFlag;
extern	bit	bool50msFlag;
extern	bit	bool100msFlag;
extern	bit	bool500msFlag;
extern	bit	bool1MinuteFlag;
extern	bit	flagIpRecord;
extern	bit	flag_jo;

extern	bit	nowIsNightServiceTime;
extern	bit	callerIdSendFlags;

extern	bit	boolCallIdMode;		/* FSK : 1; DTMF : 0 */
extern	bit	boolNoDialAction;
extern	bit	boolTrunkMode;
extern	bit	boolGroupCall;
extern	bit	boolDifferRinging;
extern	bit	boolLine2Line;
extern	bit	boolSpecialFirst;
extern	bit	boolMechanism;

extern	bit	fskFirstByteFlag;
extern	bit	fskWaitStopFlag;

//#define	variable_initial

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




#if WITH_UART_DEBUG_PSTN_FSK_DATA
extern	INT8U	xdata d[16];
extern	INT8U	data	totalD;
#endif

/********************   Assemble Functions decalrations *********************/
extern	void	delay5ms(void);
extern	void	DELAY5US(void);


/************************** C function declarations ***********************/
void	pbx_mdelay(INT8U ms);
void	pbx_2_beep(void);

void pbxBuzzer( INT8U ms50);

void	pbx_reset_systime(void);

void	pbx_reset_factory_default(void);
void	restoreFromNvram(void );
void	resetAllDataStructures(void);

/*****  hardware interface functions ************/
void	extension_ringback_tone_play(INT8U devIndex, BOOL isEnable);
void devRecordPlay(INT8U rowDev, BOOL isPlay);
void	devToneRingbackPlay(INT8U devIndex, BOOL isPlay);
void	devMusicPlay(INT8U x, BOOL isPlay);

void	hwReadDtmfDecoders(void);


BOOL	voiceChannelConnect(INT8U x, INT8U y);
void		voiceChannelBroken(INT8U x, INT8U y);


BOOL	dtmfDecodingChannelAllocate(INT8U);
void		dtmfDecodingChannelFree(INT8U devIndex);
INT8U	dtmfDecodingChannelFind(INT8U devIndex);
void 	dtmfDecodingChannelAddDigit(INT8U index, INT8U digit);	

INT8U	pbx_find_free_column(void );
INT8U	findFreeIpChannel(void);

INT8U	findFreeRow4Ip(void);
INT8U	pbx_find_free_row_ext(void);

void		dtmfEncoderPlayStart(INT8U extLineIndex);
void		dtmfEncoderPlayStep(INT8U index);
INT8U	dtmfEncodeChannelAllocate(INT8U extLineIndex, INT8U xdata *digitsValue, INT8U length );
INT8U	dtmfEncodeChannelFind(INT8U extLineIndex);
void		dtmfEncodeChannelFree(INT8U extLineIndex);

void		pstnDeviceOnhook(INT8U x);
void		pstnDeviceOffhook(INT8U x);
INT8U 	pstnDeviceAllocate(void );
void 	pstnDeviceFree(INT8U lineIndex );

void		pstn_callerid_end(INT8U x);
INT8U 	CallerIdChannelFind(INT8U devIndex);
INT8U 	CallerIdChannelAllocate(INT8U devIndex);
void		CallerIdChannelFree(INT8U devIndex);

/* independent on  hardware functions declaration */
INT8U pbx_find_ext_by_number(INT8U  xdata *phoneNumber, INT8U length );
void	pbx_check_night_service_time(void);


void	pbxTimerTasks(void);
void pstnTimerTask50ms(void);
void extTimerTask50ms(void);

void	pbx_statemachine_handle(void);
void	pbx_dev_handle(INT8U i, INT8U event);

void	devIpEventHandler(INT8U devIndex, dev_event_t event);
void	devPstnEventHandler(INT8U devIndex, dev_event_t event);
void	devExtEventHandler(INT8U devIndex, dev_event_t event);


void	uart_rx(void);
void uart_tx_cmd(INT8U cmd, INT8U length );
void	net_service_config(void);



BOOL pbx_tx_call_onhook(INT8U ipIndex);
INT8U pbx_tx_call_offhook_as_caller(INT8U extLineIndex);//, INT8U xdata *y/*docoded phone number*/);
INT8U pbx_tx_call_digits_as_caller(INT8U extLineIndex, phone_digits_t xdata *digits);
BOOL pbx_tx_call_offhook_as_callee(INT8U ipIndex);


void ipdigit_2_pbxdigit(INT8U xdata *pbxData, INT8U xdata *ipDigit, INT8U phoneLength );
void pbxdigit_2_ipdigit(INT8U xdata *ipData, INT8U xdata *pbxDigit, INT8U phoneLength );
INT8U pbx_compare_digits(INT8U xdata *dest, INT8U xdata *src, INT8U length);

#if WITH_OPTIMIZE_SIZE
void uart_tx_int_value(INT8U cmd, INT8U value);
void uart_tx_2_int_value(INT8U cmd, INT8U value1, INT8U value2);
#endif
#if WITH_UART_DEBUG
void pbx_uart_debug(INT8U xdata *buf, INT8U length);
void uart_debug_int_value(INT8U type, INT8U value);
void uart_debug_2_int_value(INT8U type, INT8U value1, INT8U value2);
void uart_debug_3_int_value(INT8U type, INT8U value1, INT8U value2, INT8U value3);
void uart_debug_4_int_value(INT8U type, INT8U value1, INT8U value2, INT8U value3, INT8U value4);
#endif

void pbx_x_str_copy(INT8U *dest, INT8U *src, INT8U length );
void pbxdigit_odd_copy(INT8U xdata *destDigits, INT8U xdata *srcDigits,INT8U length );

INT8U	call_dialing_check_dest(INT8U srcDev, phone_digits_t xdata *dialingDigits);
ext_transfer_digits xdata *call_transfer_check(INT8U extIndex);

#if 0
INT8U eeprom_read_byte(INT16U address);
BOOL eeprom_write_byte(INT8U value, INT16U address);
#endif
BOOL eeprom_write_buffer(INT8U xdata *buf, INT8U pageNo, INT8U length);
BOOL eeprom_read_buffer(INT8U xdata *buf, INT8U pageNo, INT8U length);

void	saveNvram(void );

INT8U	program_cfg_global_options(INT8U extIndex,INT8U cmd, phone_digits_t xdata *dialingDigits);
INT8U	program_cfg_2_params(INT8U extIndex,INT8U cmd, phone_digits_t xdata *dialingDigits);
INT8U	program_cfg_passwd(INT8U extIndex,INT8U cmd,  phone_digits_t xdata *dialingDigits);
INT8U	program_cfg_index_value(INT8U extIndex,INT8U cmd,  phone_digits_t xdata *dialingDigits);
INT8U	program_cfg_index_time_value(INT8U extIndex,INT8U cmd,  phone_digits_t xdata *dialingDigits);
INT8U	program_cfg_night_service_time(INT8U extIndex,INT8U cmd,  phone_digits_t xdata *dialingDigits);

INT8U	pbx_program_enter_handle(INT8U extIndex, phone_digits_t xdata *dialingDigits);
INT8U	pbx_program_exit_handle(INT8U extIndex);//, phone_digits_t xdata *dialingDigits);
void 	programSucess(INT8U extIndex);

BOOL	self_test_handle(uart_cmd_t xdata *value);

INT8U	self_test_exit_handle(INT8U rowIndex, INT8U event);

void		pbx_ring_all(INT8U	 whichOne);
INT8U 	callTransfer(INT8U dest, INT8U  xdata *calleeDigits, INT8U length);
INT8U	callSpecialFirst(INT8U extIndex);

void nvram_save_int_value(INT8U cmd, INT8U value);
void nvram_save_2_int_value(INT8U cmd, INT8U index, INT8U value);
void nvram_save_values(INT8U cmd, INT8U xdata *value, INT8U length);
void nvram_save_values_4_program(INT8U cmd, INT8U xdata *value, INT8U length);
void uart_rx_no_value(INT8U cmd);
BOOL	uart_call_rx_handle(uart_cmd_t xdata *value);


void	ht1381_enable(void);
void	pbx_systime_write(void);
void	pbx_systime_reload(void);


INT8U	pbx_conference_begin(INT8U master);
void 	pbx_conference_master_onhook(INT8U masterId);

BOOL pstnDidRinging(INT8U devIndex);


void		cfg_cpld_cmd(INT8U cmdWrite);
void 	cfg_cpld_value_cmd(INT8U cmdWrite, INT8U valueP0);
void		dtfm_encoder_digit(INT8U index, INT8U digit);
void 	dtfm_decoder_enable(INT8U index);

void 	pbx_buzzer_ctrl(BOOL status);
void		reset_switch(void);

INT8U 	pbx_read_hw_status(INT8U type);

void 	hc138_enable(INT8U value);
void 	hc138_value_cmd(INT8U which, INT8U valueP0);
void 	switch_connect(INT8U row, INT8U col, BOOL isConnect);


/*********************  state handlers ****************/
INT8U	ext_idle_state_digits(INT8U devIndex);
INT8U	ext_idle_state_offhook(INT8U devIndex);
INT8U	ext_idle_state_ringing(INT8U devIndex);
INT8U	ext_idle_state_timeout(INT8U devIndex);
INT8U 	ext_idle_state_car(INT8U devIndex);
INT8U 	ext_idle_state_ringeron(INT8U devIndex);
INT8U 	ext_idle_state_ringeroff(INT8U devIndex);
void		ext_idle_state_enter(INT8U devIndex);

INT8U	ext_dialing_state_ringing(INT8U devIndex);
INT8U	ext_dialing_state_digits(INT8U devIndex);
INT8U	ext_dialing_state_onhook(INT8U devIndex);
INT8U	ext_dialing_state_timeout(INT8U devIndex);
void		ext_dialing_state_enter(INT8U devIndex);

INT8U	ext_redialing_state_onhook(INT8U devIndex);
INT8U	ext_redialing_state_timeout(INT8U devIndex);
void 	ext_redialing_state_enter(INT8U devIndex);


INT8U	ext_trying_state_timeout(INT8U devIndex);
INT8U	ext_trying_state_onhook(INT8U devIndex);
INT8U	ext_calling_state_OK(INT8U devIndex);
INT8U	ext_trying_state_flash(INT8U devIndex);

void		ext_trying_state_enter(INT8U devIndex);

INT8U	ext_ringing_state_ringer_off(INT8U devIndex);
INT8U	ext_ringing_state_offhook(INT8U devIndex);
INT8U	ext_ringing_state_timeout(INT8U devIndex);
INT8U	ext_ringing_state_cancel(INT8U devIndex);
void		ext_ringing_state_enter(INT8U devIndex);

INT8U	ext_incall_state_onhook(INT8U devIndex);
INT8U	ext_incall_state_timeout(INT8U devIndex);
INT8U	ext_incall_state_cancel(INT8U devIndex);
void		ext_incall_state_enter(INT8U devIndex);

INT8U	ext_incall_state_flash(INT8U devIndex);

INT8U	ext_busy_state_onhook(INT8U devIndex);
INT8U	ext_busy_state_timeout(INT8U devIndex);
void		ext_busy_state_enter(INT8U devIndex);


INT8U	ext_wait_state_onhook(INT8U devIndex);
INT8U	ext_wait_state_offhook(INT8U devIndex);
INT8U	ext_wait_state_ok(INT8U devIndex);
INT8U	ext_wait_state_cancel(INT8U devIndex);
void 	ext_wait_state_enter(INT8U devIndex);

INT8U	ext_wait_state_timeout(INT8U devIndex);

INT8U	ext_offhook_state_flash(INT8U devIndex);


INT8U	ext_programing_state_digits(INT8U devIndex);
INT8U	ext_programing_state_onhook(INT8U devIndex);
INT8U	ext_programing_state_timeout(INT8U devIndex);
void 	ext_programing_state_enter(INT8U devIndex);

INT8U	ext_rering_state_onhook(INT8U devIndex);

INT8U	ext_car_state_offhook(INT8U devIndex);
INT8U	ext_car_state_timeout(INT8U devIndex);
INT8U	ext_car_state_onhook(INT8U devIndex);
void 	ext_car_state_enter(INT8U devIndex);

#endif

