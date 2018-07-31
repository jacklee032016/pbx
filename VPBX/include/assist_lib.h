/*
 * $Log: assist_lib.h,v $
 * Revision 1.1.1.1  2006/11/30 16:26:29  lizhijie
 * AS800 VPBX
 *
 * Revision 1.1  2006/06/03 12:15:31  lizhijie
 * no message
 *
 * Revision 1.10  2006/05/30 09:56:50  lizhijie
 * add some new features, refer to changeLog
 *
 * Revision 1.30  2006/05/30 03:04:27  lizhijie
 * optimzed DTMF decoder and add DSP configuration interface
 *
 * Revision 1.29  2006/05/28 10:14:58  lizhijie
 * dd
 *
 * Revision 1.28  2006/05/27 10:52:58  lizhijie
 * dd
 *
 * Revision 1.27  2006/05/25 03:19:41  lizhijie
 * add pstn_standard
 *
 * Revision 1.26  2006/05/23 02:54:17  lizhijie
 * dd
 *
 * Revision 1.25  2006/05/19 08:46:59  lizhijie
 * add fsk demodulation in buffer
 *
 * Revision 1.24  2006/05/19 08:46:02  lizhijie
 * no message
 *
 * Revision 1.23  2006/05/17 10:27:07  lizhijie
 * *** empty log message ***
 *
 * Revision 1.22  2006/05/09 04:02:25  lizhijie
 * no message
 *
 * Revision 1.21  2006/04/30 01:30:25  wangwei
 * no message
 *
 * Revision 1.20  2006/04/27 05:21:53  wangwei
 * no message
 *
 * Revision 1.19  2006/04/25 06:44:03  wangwei
 * no message
 *
 * Revision 1.18  2006/04/20 05:17:24  lizhijie
 * add Call Progress Tone Detect
 *
 * Revision 1.17  2006/04/19 12:28:39  lizhijie
 * add CAR support and others
 *
 * Revision 1.16  2006/04/19 12:23:03  lizhijie
 * no message
 *
 * Revision 1.15  2006/04/05 09:31:20  wangwei
 * no message
 *
 * Revision 1.14  2006/03/30 07:24:56  wangwei
 * no message
 *
 * Revision 1.13  2006/03/25 11:27:28  lizhijie
 * add FXO line status detect
 *
 * Revision 1.12  2006/03/25 11:25:57  lizhijie
 * no message
 *
 * Revision 1.11  2006/03/24 11:07:46  lizhijie
 * add a C-language file for DTMF and silence data and compile it into libpstn.so
 *
 * Revision 1.10  2006/03/24 02:37:50  lizhijie
 * Add device pending state
 *
 * Revision 1.9  2006/03/23 09:36:03  lizhijie
 * no message
 *
 * Revision 1.8  2006/03/21 09:50:58  lizhijie
 * debug with vpbx program
 *
 * Revision 1.7  2006/03/21 02:13:42  lizhijie
 * add functions called by vpbx
 *
 * Revision 1.6  2006/03/20 06:46:37  lizhijie
 * add and debug some. refer to ChangeLog
 *
 * Revision 1.5  2006/03/17 07:58:10  lizhijie
 * add busy tone detect and debug problem of DTMF detect
 *
 * Revision 1.4  2006/03/16 08:41:29  lizhijie
 * add FSK support and FSK test programs in IXP4xx platform
 *
 * Revision 1.3  2006/03/13 02:50:20  lizhijie
 * add tone defination for IIT,SDT,NFT,CPT and play mechanism for tone
 *
 * Revision 1.2  2006/03/11 10:18:34  lizhijie
 * dd
 *
 * Revision 1.1.1.1  2006/03/10 09:31:43  lizhijie
 * PSTN APIs library for Gatewat project
 *
 * $Id: assist_lib.h,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#ifndef __ASSIST_LIB_H__
#define __ASSIST_LIB_H__

#ifdef  __cplusplus
	extern "C"
	{
#endif

#define trace		printf("%s[%d]\r\n", __FUNCTION__, __LINE__);

#define FALSE   							0
#define TRUE    							(!FALSE)

#ifndef AS_OK
#define AS_OK   							0
#endif
#ifndef AS_FAIL
#define AS_FAIL    						-1
#endif

#define AS_MAX_NAME_LENGTH 			128


#ifndef AS_ELAST
#define AS_ELAST 500
#endif

#ifndef  AS_EVENT_ONHOOK
#define	AS_EVENT_ONHOOK 				1
#endif

#ifndef  AS_EVENT_RINGOFFHOOK
#define	AS_EVENT_RINGOFFHOOK 		2
#endif

#ifndef  AS_EVENT_WINKFLASH
#define	AS_EVENT_WINKFLASH 			3
#endif

#ifndef  AS_EVENT_RINGERON
#define	AS_EVENT_RINGERON 			10
#endif

#ifndef  AS_EVENT_RINGEROFF
#define	AS_EVENT_RINGEROFF 			11
#endif

#ifndef  AS_EVENT_POLARITY
#define	AS_EVENT_POLARITY 			17
#endif

#ifndef  AS_EVENT_CAR
#define 	AS_EVENT_CAR					18
#endif

enum AS_DEVICE_STATE
{
	AS_DEVICE_STATE_INVALIDATE 		= -1, 	/* file descriptor is not initted */
	AS_DEVICE_STATE_ONHOOK,					/* available for device monitor thread */
	AS_DEVICE_STATE_OFFHOOK,					/* used by calling or callee thread */
	AS_DEVICE_STATE_WINKFALSH,

	/* for FXO, this used as state between First and Second Ring, FXS, used as debounce from OFFHOOK-->ONHOOK */
	AS_DEVICE_STATE_PENDING,

	AS_DEVICE_STATE_IDLE,
	AS_DEVICE_STATE_BUSY
};

typedef enum AS_DEVICE_STATE  as_state_t;

#define AS_DRIVER_BUFFER_LENGTH		160

#define AS_DEVICE_NAME					"/dev/astel"
#define AS_DEVICE_MAX					16

#define AS_SPAN_DEV					"/dev/astel/0"


/* Dial frequency tables */
typedef struct
{
	char	chr;	/* character representation */
	float	f1;	/* first freq */
	float	f2;	/* second freq */
}AS_DIAL;


/*********************************************************************
 *  Following are utils function used in driver 
*********************************************************************/
extern	int 		as_error_msg(char *fmt, ...);


/*********************************************************************
 *  Following are library function used for PSTN device 
*********************************************************************/
extern	int 		as_ring_with_dtmf_caller_id( int  fd  , unsigned char *call_id);
extern	int 		as_ring_on_hook( int  fd );
extern	int 		as_stop_ring(int fd);
extern	int 		as_car_on_fxs( int  fd );


/* set device to ONHOOK status, eg. reset the device */
extern	int 		as_lib_onhook(int fd);
/* set the device to OFFHOOK status, eg. begin to communicate */
extern	int 		as_lib_offhook(int fd);
/* get an event of the device */
extern	int 		as_lib_event_get(int fd);
/* wait RINGOFF event, 
 * when it used on a FXO device, this is first RINGEROFF
 * when is used on a FXS device, this is OFFHOOK
*/
extern	void 	as_lib_wait_offhook(int fd);
extern	void 	as_lib_wait_onhook(int fd);

/* from as_lib_tones.c */
extern	int 		as_tone_play_stop( int fd );
extern	int 		as_tone_play_dial( int fd );
extern	int 		as_tone_play_busy( int  fd ) ;
extern	int 		as_tone_play_ring( int  fd ) ;
extern	int 		as_tone_play_ringback( int  fd ) ;
extern	int 		as_tone_play_congestion( int  fd ) ;
extern	int 		as_tone_play_callwait( int  fd ) ;
extern	int 		as_tone_play_dialrecall( int  fd ) ;
extern	int 		as_tone_play_record(  int  fd  ) ;
extern	int 		as_tone_play_info(  int  fd  ) ;
extern	int 		as_tone_play_custom_1(  int  fd ) ;
extern	int 		as_tone_play_notify_tone( int  fd ) ;
extern	int 		as_tone_play_accept_tone( int  fd ) ;
extern	int 		as_tone_play_income_id_tone( int  fd ) ;

extern	int 		as_tone_play_custom_2( int  fd ) ;
extern	int 		as_tone_play_stutter(  int  fd ) ;

extern	int 		as_lib_wait_ringer_off(int fd);


/* device number is return in this pointer.
* return :
* <0, error, no hardware is detected or driver is not load
*/
extern	int 			as_get_device_num();

/* get status(SLIC or DAA, Off-Hook or On-Hook) of the device with index of 'dev_number' */
extern	struct as_channel_state *as_get_dev_state(int dev_number);
extern	as_state_t  	as_device_check_state_on_startup(int fd, const char *name);

 /* this function is used to check whether a line connect to FXO port, so it is only used in FXO device */
extern	int  			as_device_check_fxo_online(int fd, const char *name);


#define 	AS_TXSTATE_ONHOOK 		0
#define	AS_TXSTATE_OFFHOOK		1

typedef enum
{
	AS_CHANNEL_TYPE_FXS,			/* FXS device channel */
	AS_CHANNEL_TYPE_FXO			/* FXO device channel */
} as_channel_type_t;


extern	int 			as_dtmf_is_ready( int fd );
extern	unsigned char as_get_dtmf_4_fxs(int fd);

/* return <0: no DIAL PULSE digit is ready ;  >= 0 : a DP digit is ready */
extern	int 			as_get_dp_signal( int fd );


/* from and for as_lib_dtmf_detect.c */
#define DTMF_DETECT_STEP_SIZE			102

/* 38.25ms, nearly 40ms, for Console DTMF decoder */
#define	AS_DTMF_DECOCDER_BUFFER_LENGTH		320

/* from and for as_lib_dtmf_generator.c */
#define	AS_DTMF_BLOCKSIZE 			204	/* perfect for goertzel */

/*add wangwei , 2006.02.22*/
//#define	AS_DEFAULT_DTMFLENGTH 	100 /* default tone length -- 100 ms */
//#define	AS_DEFAULT_DTMFLENGTH 		50 /* default tone length -- 50 ms */

/* refer to NTT specs. Ver 5 P.19 */
//#define 	DTMF_TONE_DURATION			200		/* ms for every DTMF character */
//#define 	DTMF_TONE_MARK				100		/* ms mark data for every DTMF character */
/* DSP DTMF call only for FXO, so it must delay for more time 
* following are experience value tested in CNC(China Network Communication) device, LiZhijie,2006.05.27 
*/
#if 0
#define 	DTMF_TONE_DURATION			250		/* ms for every DTMF character */
#define 	DTMF_TONE_MARK				100		/* ms mark data for every DTMF character */
#else
#define 	DTMF_TONE_DURATION_CHINA			500		/* ms for every DTMF character */
#define 	DTMF_TONE_MARK_CHINA				100		/* ms mark data for every DTMF character */
#define 	DTMF_TONE_DURATION_JAPAN			500		/* ms for every DTMF character */
#define 	DTMF_TONE_MARK_JAPAN				80		/* ms mark data for every DTMF character */
#endif

/*******************************************
* interface exposed by dsp module 
********************************************/
typedef enum g711_law
{
	A_LAW_CODE	=	0,
	U_LAW_CODE	=	1  		/* default code method of our driver */
}law_t;

#define	FSK_CALL_ID_MAX_LENGTH			32

typedef enum
{
	PSTN_STANDARD_CHINA	= 0,
	PSTN_STANDARD_JAPAN	
}pstn_standard_t;

/* config driver for Chinese/Japanese PSTN Standard */
extern 	int 	as_device_standard_config(int fd, pstn_standard_t standard);

typedef struct
{
	pstn_standard_t  	standard;
	
	law_t			law;
	int 				toneLength;					/* tone data length stored in dsp, in unit of ms  */
	
	int				dtmfMarkTime;				/* for DTMF play out in FXO */
	int				dtmfDuration;
}as_dsp_config_t;

typedef struct
{
	void				*priv;
	as_dsp_config_t	config;
}as_dsp_t;

#define 	AS_CALL_PARAM_CALLERID				0x00000001		/* Caller ID detect */
#define 	AS_CALL_PARAM_CALLED					0x00000002		/* Called number detect, for DID */
#define 	AS_CALL_PARAM_DATE					0x00000004

#define	AS_PSTN_MAX_PHONE_NUMBER			32
/* FSK or DTMF detect result from switch */
typedef struct
{
	int				result;
	unsigned char		caller[AS_PSTN_MAX_PHONE_NUMBER];
	unsigned char		called[AS_PSTN_MAX_PHONE_NUMBER];	/* for MDMF(chinese format), it is username */
	unsigned char		date[AS_PSTN_MAX_PHONE_NUMBER];	/* for MDMF(chinese format), it is date */

}pstn_param_t;

/* result of call progress detect */
#define	AS_PROGRESS_UNKNOWN		0
#define	AS_PROGRESS_DIALTONE		1
#define	AS_PROGRESS_RINGBACK		2
#define	AS_PROGRESS_2DIALTONE	3
#define	AS_PROGRESS_BUSYTONE		4
#define	AS_PROGRESS_TALKING		5


extern	as_dsp_t *as_dsp_init(as_dsp_config_t *dspConfig );
extern	void 	as_dsp_destroy(as_dsp_t *dsp);
extern	as_dsp_t *as_dsp_init_no_cfg(law_t law, pstn_standard_t standard, int ms);

/* play DT(Dial Tone) */
extern	int 		as_dsp_readDialTone(as_dsp_t *dsp, char *buffer);

/* play RBT(Ring Back Tone) */
extern	int 		as_dsp_readRingBackTone(as_dsp_t *dsp, char *buffer);
extern	int 		as_dsp_readBusyTone(as_dsp_t *dsp, char *buffer);
/* for both Notification(NFT) and Acceptance(CPT) */
extern	int 		as_dsp_readNFTTone(as_dsp_t *dsp, char *buffer);
extern	int 		as_dsp_readSilence(as_dsp_t *dsp, char *buffer);
/* for SDT(Second Dial Tone) */
extern	int 		as_dsp_readSDTone(as_dsp_t *dsp, char *buffer);
/* for IIT(Incoming Identification Tone) */
extern	int 		as_dsp_readIITone(as_dsp_t *dsp, char *buffer);

extern	int 		as_dsp_readDtmf_1(as_dsp_t *dsp, char *buffer);
extern	int 		as_dsp_readDtmf_2(as_dsp_t *dsp, char *buffer);
extern	int 		as_dsp_readDtmf_3(as_dsp_t *dsp, char *buffer);
extern	int 		as_dsp_readDtmf_A(as_dsp_t *dsp, char *buffer);
extern	int 		as_dsp_readDtmf_4(as_dsp_t *dsp, char *buffer);
extern	int 		as_dsp_readDtmf_5(as_dsp_t *dsp, char *buffer);
extern	int 		as_dsp_readDtmf_6(as_dsp_t *dsp, char *buffer);
extern	int 		as_dsp_readDtmf_B(as_dsp_t *dsp, char *buffer);
extern	int 		as_dsp_readDtmf_7(as_dsp_t *dsp, char *buffer);
extern	int 		as_dsp_readDtmf_8(as_dsp_t *dsp, char *buffer);
extern	int 		as_dsp_readDtmf_9(as_dsp_t *dsp, char *buffer);
extern	int 		as_dsp_readDtmf_C(as_dsp_t *dsp, char *buffer);
extern	int 		as_dsp_readDtmf_asterisk(as_dsp_t *dsp, char *buffer);
extern	int 		as_dsp_readDtmf_0(as_dsp_t *dsp, char *buffer);
extern	int 		as_dsp_readDtmf_shark(as_dsp_t *dsp, char *buffer);
extern	int 		as_dsp_readDtmf_D(as_dsp_t *dsp, char *buffer);


extern	int 		as_dsp_tone_play_dial(as_dsp_t *dsp,  int fd, int ms ) ;
extern 	int 		as_dsp_tone_play_busy(as_dsp_t *dsp,  int  fd , int ms) ;
extern	int 		as_dsp_tone_play_ringback(as_dsp_t *dsp,  int  fd, int ms ) ;
/* NFT(Notification Tone) */
extern	int 		as_dsp_tone_play_notify_tone(as_dsp_t *dsp,  int  fd, int ms ) ;
/* CPT(Acceptance Tone) */
extern	int 		as_dsp_tone_play_accept_tone(as_dsp_t *dsp,  int  fd, int ms ) ;
/* SDT */
extern	int 		as_dsp_tone_play_second_dial_tone(as_dsp_t *dsp,  int  fd, int ms ) ;
/* IIT */
extern	int 		as_dsp_tone_play_incoming_id_tone(as_dsp_t *dsp,  int  fd, int ms ) ;

extern	int 		as_dsp_play_dtmf(as_dsp_t *dsp, int fd, char *callerId);


/* return 0 : not detect; 1 : detect BT */
extern	int 		as_dsp_busydetect(as_dsp_t *dsp, unsigned char *buff,int length);
extern	int 		as_dsp_dtmf_detect(as_dsp_t *dsp, unsigned char *sample, int length, char *callId, int max);

extern	void		as_dsp_dtmf_decoder_reset(as_dsp_t *dsp);
extern	int 		as_dsp_dtmf_console_decode(as_dsp_t *dsp, unsigned char *sample, int length, char *callId, int max);

/* return 1 : FSK modem data, used in decode; 0 : silence */
int as_dsp_fsk_isVAD(as_dsp_t *dsp, unsigned char *buf, int length);

/* return NULL : not detect; not NULL : check result field of pstn_param_t 
* this pointer is allocated statically, so not free it  */
extern	pstn_param_t *as_dsp_fsk_decode(as_dsp_t *dsp, unsigned char *rawdata, int length);
extern	int 			as_dsp_fsk_decode_fd(as_dsp_t *dsp, int fd, char *number, char *name );


/* encode FSK data into a buffer which is allocated statically, so no free is must. Length of this buffer is 'returnLength' 
* When Japan (NTT) standard is used, parameter name is not used.
*/
extern	unsigned char *as_dsp_fsk_encode(as_dsp_t *dsp, char *number , char *name , int *returnLength );
extern	int 			as_dsp_fsk_ring_with_callerid(as_dsp_t *dsp, int fd , char *number , char *name );

/* return 0 : not detect; other AS_PROGRESS_XXX */
extern	int 		as_dsp_progress_detect(as_dsp_t *dsp, unsigned char *buff,int length);

/***DP****/
typedef enum
{
	AS_DP_10PPS,
	AS_DP_20PPS
}pps_type;

extern	int 		assist_dsp_fxo_send_pps(int fd,char *digitstring,pps_type pps);


typedef enum
{
	DELAY_SWITCH_2_FXS = 0,
	DELAY_SWITCH_2_LINE
}delay_switch_chan_tpye;

typedef enum 
{
	G_PHONE_OFFHOOK = 0,
	G_PHONE_ONHOOK
}phone_hook_status;


int as_delay_chan_ctl(delay_switch_chan_tpye status);
phone_hook_status as_hook_check(void);


unsigned short as_fsk_crc(unsigned char *msg, int nBytes);

#ifdef __cplusplus
	};
#endif


#endif

