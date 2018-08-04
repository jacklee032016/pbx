/*
* $Id: function_declarations.h,v 1.26 2007/07/25 17:48:34 lizhijie Exp $
*/
#ifndef	__FUNCTION_DECLARATIONS_H__
#define	__FUNCTION_DECLARATIONS_H__

#include <string.h>

#include "datatype.h"
#include "macros.h"
#include "pbx_macros.h"
#include "uart_cmd.h"
#include "pbx_types.h"
#include "pbx_hw.h"
#include "pbx_states.h"
#include "pbx_pstn.h"
#include "pbx_cfg.h"

#include "var_declarations.h"

/********************   Assemble Functions decalrations *********************/
/* ptr: dest;Address of EEROM; length of read/write  */
#if 0
extern	BOOL	rd24c08_multi(INT8U data *ptr,INT16U epAddr, INT8U length);	
extern	BOOL	wr24c08_multi(INT8U data *ptr,INT16U epAddr,INT8U length);	
extern	BOOL	rd24c08_single(INT8U data *ptr,INT16U address);
extern	BOOL	wr24c08_single(INT8U value,INT16U address);
#endif



extern	void		ALL_ICS_RESET(void);

/*
extern	void		CONNECT(INT8U row, INT8U col);
extern	void		DISCONNECT(INT8U row, INT8U col);
extern	void		BUZZER_ON(void);
extern	void		BUZZER_OFF(void);
extern	void		RST_MT8816(void);
extern	void		EXT_RELAY_CONTROL(INT8U);
extern	void		LINES_RELAY_CONTROL(INT8U);
extern	void		DTMF_OE_ENABLE(INT8U);		
extern	void		DTMF_OE_DISABLE(void );			
extern	void		DTMF_ENCODE_FIRST_ENABLE(INT8U digit);
extern	void		DTMF_ENCODE_FIRST_DISABLE(void);
extern	void		DTMF_ENCODE_SECOND_ENABLE(INT8U digit);
extern	void		DTMF_ENCODE_SECOND_DISABLE(void);
extern	void		START_RECORD(void );
extern	void		STOP_RECORD(void );

extern	void		PLAY_USER(void);
extern	void		STOP_USER(void);
extern	void		START_DIAL_TONE(INT8U);
extern	void		STOP_DIAL_TONE(INT8U);


extern	void		send_busycome(INT8U);
extern	void		RD_EXTSTATUS(INT8U data *);
extern	void		RD_TRKSTATUS(INT8U data *,INT8U data*);
*/


extern	void		TIME_BYTES_WRITE(INT8U cmd, INT8U data *ptr, INT8U length);	/*写万年历，参数命令字，写入数据地址，写入字节数*/
extern	void		TIME_BYTES_READ(INT8U cmd,INT8U data *ptr,INT8U length);/*读万年历，参数命令字，读入数据地址，读入字节数*/

extern	void	delay5ms(void);
extern	void	DELAY5US(void);


/************************** C function declarations ***********************/
void	pbx_mdelay(INT8U ms);
void	pbx_OK(void);
void	pbx_2_beep(void);

void pbxBuzzer( INT8U ms50);

void	pbx_init(void );
void	pbx_reset_systime(void);

void	pbx_reset_factory_default(void);
void	restoreFromNvram(void );
void	power_on_rst(void);

/*****  hardware interface functions ************/
void	extension_ringback_tone_play(INT8U devIndex, BOOL isEnable);
void pbx_if_play_record(INT8U rowDev, BOOL isPlay);
void	pbx_if_ringback_tone_play(INT8U devIndex, BOOL isPlay);
void	pbx_if_start_music(INT8U x, BOOL isPlay);
void	pbx_if_start_ringing(INT8U x, BOOL isPlay);

void	pbx_read_dtmf_digits(void);


BOOL	pbx_voice_channel_connect(INT8U x, INT8U y);
void		pbx_voice_channel_broken(INT8U x, INT8U y);


BOOL	decodingchannel_allocate(INT8U);
void		decodingchannel_free(INT8U devIndex);
INT8U	decodingchannel_find(INT8U devIndex);
void decodingchannel_add_digits(INT8U index, INT8U digit);	

INT8U	pbx_find_free_column(void );
INT8U	pbx_ip_find_free_device(void);

INT8U	pbx_find_free_row(void);
INT8U	pbx_find_free_row_ext(void);

void		pbx_row_changed(INT8U x);

void		callerid_play_start(INT8U extLineIndex);
void		callerid_play_step(INT8U index);
INT8U	callerid_allocate_channel(INT8U extLineIndex, INT8U xdata *digitsValue, INT8U length );
INT8U	callerid_find_channel(INT8U extLineIndex);
void		callerid_free_channel(INT8U extLineIndex);

void		pbx_line_onhook(INT8U x);
void		pbx_line_offhook(INT8U x);
INT8U 	pbx_line_allocate_channel(void );
void 	pbx_line_free_channel(INT8U lineIndex );

void		pstn_callerid_begin(INT8U x);
void		pstn_callerid_end(INT8U x);
void		pstn_callerid_free(INT8U x);	

/* independent on  hardware functions declaration */
INT8U pbx_find_ext_by_number(INT8U  xdata *phoneNumber, INT8U length );
void	pbx_check_night_service_time(void);
void	pbx_timer_task(void);

void	pbx_statemachine_handle(void);


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
#if UART_DEBUG
void pbx_uart_debug(INT8U xdata *buf, INT8U length);
void uart_debug_int_value(INT8U type, INT8U value);
void uart_debug_2_int_value(INT8U type, INT8U value1, INT8U value2);
void uart_debug_3_int_value(INT8U type, INT8U value1, INT8U value2, INT8U value3);
#endif

void pbx_x_str_copy(INT8U *dest, INT8U *src, INT8U length );
void pbxdigit_odd_copy(INT8U xdata *destDigits, INT8U xdata *srcDigits,INT8U length );


void		ip_call_tx_dialing_signal(INT8U ipIndex, INT8U extLineIndex);
void		ip_call_tx_caller_digits_signal(INT8U  extLineIndex);
INT8U ip_call_tx_callee_digits_signal(INT8U extLineIndex, phone_digits_t xdata *digits);
void	ip_call_tx_cancel_signal(INT8U extLineIndex);
void ip_call_tx_busy_signal(INT8U ipIndex, INT8U extLineIndex);
void ip_call_tx_trying_signal(INT8U ipIndex, INT8U extLineIndex);
BOOL ip_call_tx_offhook_signal(INT8U ipIndex);
BOOL ip_call_tx_onhook_signal(INT8U ipIndex);

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

void		pbx_factory_default(void);

void nvram_save_int_value(INT8U cmd, INT8U value);
void nvram_save_2_int_value(INT8U cmd, INT8U index, INT8U value);
void nvram_save_values(INT8U cmd, INT8U xdata *value, INT8U length);
void nvram_save_values_4_program(INT8U cmd, INT8U xdata *value, INT8U length);
void uart_rx_no_value(INT8U cmd);

void	ht1381_enable(void);
void	pbx_systime_write(void);
void	pbx_systime_reload(void);


INT8U	pbx_conference_begin(INT8U master);
void 	pbx_conference_master_onhook(INT8U masterId);

BOOL pstnDidRinging(INT8U devIndex);

void	check_pstn_hook_event(void);


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

#endif

