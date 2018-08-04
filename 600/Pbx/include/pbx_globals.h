#ifndef  __PBX_GLOBAL_H__
#define __PBX_GLOBAL_H__
/*
 * encapsulate all interface with hardware device drivers for C++ environments
 * Li Zhijie , 2006.02.04
*/

#ifdef  __cplusplus
	extern "C"
	{
#endif

#include "common_globals.h"
#include "as_dev_ctl.h"

typedef enum	
{
	assist_app_unknow = -1,
	assist_app_slic = 0,
	assist_app_pcm =1
}assist_app_type;


#if  __ARM_IXP__
	#define 	IPPBX_ROOT_DIR			"/usr/"
#else
	#define 	IPPBX_ROOT_DIR			"releases/usr/"
#endif

#define IPPBX_WORK_ROOT      		"/tmp/pbx"


#define IPPBX_CONFIG_FILE			IPPBX_ROOT_DIR"etc/ua.cfg"		


#define PBX_TONES_DIR				IPPBX_ROOT_DIR"pbx/"

#define IPPBX_PID_FILE				IPPBX_WORK_ROOT"/pbx.pid"

#define DEBUG_ASSIST_DEVICE		1	/* debug info for /dev/astel and the associated */			

#define PSTN_DEVICES_EPOCH			0x80
#define VIRTUAL_DEVICES_EPOCH		1000

#define 	DTMF_PARSE_LENGTH		(DTMF_DETECT_STEP_SIZE*10)

// #define ULAW_PAYLOAD 1012
#define ULAW_SAMPLE_RATE 			160
#define RESID_RTP_RATE 				160
#define NETWORK_RTP_RATE 			320
#define FLASH_TIMEOUT 				1000000

#define AS_RTP_JITTER_NEW			-1 /* bypass jitter buffer */

enum AS_DEVICE_STATE
{
	AS_DEVICE_STATE_INVALIDATE = -1, 	/* file descriptor is not initted */
	AS_DEVICE_STATE_ONHOOK,			/* available for device monitor thread */
	AS_DEVICE_STATE_OFFHOOK,			/* used by calling or callee thread */
	AS_DEVICE_STATE_WINKFALSH,
/*added by lijie*/	
	AS_DEVICE_STATE_IDLE,
	AS_DEVICE_STATE_BUSY
};

typedef enum AS_DEVICE_STATE  as_state_t;


#define AS_DEVICE_NAME_LENGTH			32

struct AS_SPAN;
struct AS_DEVICE;

struct AS_DEV_OPERATIONS
{
	int (* ring_start)(struct AS_DEVICE *dev);
	int (* ring_stop)(struct AS_DEVICE *dev);
	int (* tone_dial_start)(struct AS_DEVICE *dev);
	int (* tone_dial_stop)(struct AS_DEVICE *dev);
	int (* tone_ringback_start)(struct AS_DEVICE *dev);
	int (* tone_ringback_stop)(struct AS_DEVICE *dev);
	int (* tone_busy_start)(struct AS_DEVICE *dev);
	int (* tone_busy_stop)(struct AS_DEVICE *dev);
	int (* tone_fastbusy_start)(struct AS_DEVICE *dev);
	int (* tone_fastbusy_stop)(struct AS_DEVICE *dev);
	int (* tone_callwait_start)(struct AS_DEVICE *dev);
	int (* tone_callwait_stop)(struct AS_DEVICE *dev);
	
	as_state_t  (*check_status)(struct AS_DEVICE *dev);
	unsigned char (*report_dtmf_digit)(struct AS_DEVICE *dev);

	int (*getPcmChannelId)(struct AS_DEVICE *dev );
	
	//added by lijie to query device status
	int  (*query_status)(struct AS_DEVICE *dev);
	//added by lijie to get device status
	as_state_t  (*get_status)(struct AS_DEVICE *dev);
	int (*trans_telnumber)(struct AS_DEVICE *dev, const char *bufTelNumber);
};

typedef struct AS_DEV_OPERATIONS as_operations_t;

typedef enum
{
	as_device_unknown = -1 ,
	as_device_slic	= 0,
	as_device_pcm	= 1
}as_device_type_t;
typedef enum
{
	as_hardware_device = 1,
	as_virtual_device ,
	as_pstn_device,
	as_invalid_device,
	as_other_device
}device_type_t;


typedef enum
{
	as_pbx_dtmf_invalide = -1,
	as_pbx_dtmf_rfc2883 = 0,
	as_pbx_dtmf_audio  = 1	
}as_pbx_dtmf_mode_t;

struct AS_DEVICE;

struct AS_SPAN
{
	char name[AS_DEVICE_NAME_LENGTH];
	int timeout; /* in ms */
	as_operations_t		*ops;
	int 					count;		/* count of hardware device such as SLIC or PCM  */
	struct AS_DEVICE			*devs[AS_DEVICE_MAX];
	int					pbx_count;	/* count of virtual pbx devoce */
	void 				*pbx_devs[AS_DEVICE_MAX];		/* point to AssistPbxDevice */
	void 				*private_data;
	void					*invalid_dev;
};

typedef struct AS_SPAN as_span_t;
	
#define	MAX_CALLERID_LENGTH		32
	
struct AS_DEVICE
{
	char 				name[AS_DEVICE_NAME_LENGTH];
	int 					fd;
	as_state_t			state;
	as_device_type_t		type;
	int 					lock;	/* 1: locked, 0:unlocked */
	int 					id; /* position in the devices array of span */
	/*added by lijie on 2005-02-25*/
	int					phyId; /*phyId*/
	/* end of added */
	as_span_t			*span;
	void 				*private_data;	/* point to AssistDeviceInfo */
	device_type_t			phyType;
	/* lizhijie added 2007.09.02 */
	char					callerId[MAX_CALLERID_LENGTH];
};

typedef struct AS_DEVICE as_device_t;


int as_set_law(int fd, int law, const char *devicename);


int as_setup_onhook(int fd);
int as_lib_offhook(int fd);

/* return 0: no dtmf digit is ready ;  not 0 : a dtmf digit is ready */
int as_dtmf_is_ready( int fd , const char *devicename);
unsigned char  as_dtmf_get_digit(as_device_t *dev);


int as_tone_play_stop( int fd ) ;
int as_tone_play_dial( int fd ) ;
int as_tone_play_busy( int  fd ) ;
int as_tone_play_ringback( int  fd ) ;
int as_tone_play_congestion( int  fd ) ;
int as_tone_play_callwait( int  fd ) ;
int as_tone_play_dialrecall( int  fd ) ;
int as_tone_play_record(  int  fd  ) ;
int as_tone_play_info(  int  fd  ) ;
int as_tone_play_custom_1(  int  fd ) ;
int as_tone_play_custom_2( int  fd ) ;
int as_tone_play_stutter(  int  fd ) ;

int as_lib_onhook(int fd);
/* device must be in ON_HOOK for ring */
int as_ring_start( int  fd, const char *devicename );
/* device must be in ON_HOOK for ring */
int as_ring_stop( int  fd, const char *devicename );


int as_tone_dev_play_dial(as_device_t *dev);
int as_tone_dev_play_ringback(as_device_t *dev);
int as_tone_dev_play_busy(as_device_t *dev);
int as_tone_dev_play_fastbusy(as_device_t *dev);
int as_tone_dev_play_callwait(as_device_t *dev);
int as_tone_dev_play_stop(as_device_t *dev);

int as_dev_ring_start(as_device_t *dev);
int as_dev_ring_stop(as_device_t *dev);

//added by lijie
as_state_t as_device_get_status(as_device_t *dev);
int as_device_query_status(as_device_t *dev);
int pcm_dev_trans_telnumber(as_device_t *dev, const char *bufTelNumber);
int as_device_trans_telnumber(as_device_t *dev, const char *bufTelNumber);
int as_get_channel_state(struct as_channel_state *channel_states);
//added ends here


/*added by lijie on 2005-02-25*/
int pcm_set_law(int fd, int law, const char *devicename);

int pcm_setup_onhook(int fd);



int pcm_dev_tone_dial_play(as_device_t *dev);
int pcm_dev_tone_dial_stop(as_device_t *dev);
int pcm_dev_tone_ringback_play(as_device_t *dev);
int pcm_dev_tone_ringback_stop(as_device_t *dev);
int pcm_dev_tone_busy_play(as_device_t *dev);
int pcm_dev_tone_fastbusy_play(as_device_t *dev);
int pcm_dev_tone_callwait_play(as_device_t *dev);
int pcm_dev_tone_play_stop(as_device_t *dev);

	
int pcm_dev_ring_start(as_device_t *dev);
int pcm_dev_ring_stop(as_device_t *dev);

unsigned char  pcm_dev_dtmf_get_digit(as_device_t *dev);


as_state_t  pcm_device_check_state_on_startup(int devId, const char *devicename);
as_state_t pcm_dev_check_status(as_device_t *dev );
int  pcm_dev_query_status(as_device_t *dev);
as_state_t  pcm_dev_get_status(as_device_t *dev);

void *pcm_Thread(void *P);
void *slic_Thread(void *P);
void slic_init(as_span_t  *span);


int pcm_dev_dial_code(as_device_t *dev, const char *callerId);
int pcm_ip_onhook(as_device_t *dev );

int pcm_dev_get_channel_id(as_device_t *dev);

/*added ends here*/



as_span_t  *as_check_hardware(const char *name, assist_app_type apptype, int law,  int ipstnCount, int level);

#define GAIN_NO_CHANGE			0.0		
#define GAIN_2_TIMES			6.0  		/* 6 DB is 'volume*2' */
#define GAIN_3_TIMES			9.542
#define GAIN_4_TIMES			12.0		/* 12 DB is 'volume*4' */
#define GAIN_8_TIMES			18.06
/* law is defined in  common_globals.h */
void  as_dsp_set_gain(int fd, int law, float rx_gain_db, float tx_gain_gb );

as_state_t  as_device_check_state_on_startup(int fd, const char *devicename);
as_state_t as_device_check_status(as_device_t *dev );


struct AS_DSP;

struct AS_DSP_OPS
{
#if 0
	/* return the length of callerId */
	int (*dtmf_decode)(struct AS_DSP *dsp, char *buf, int length, char *callerId, int max);
	int (*dtmf_decode_step)(struct AS_DSP *dsp, char *buf, int length);
	int (*dtmf_decode_get)(struct AS_DSP *dsp, char *callerId, int max);
#endif	
	/* return the result of ioctl */
	int (*gain_setup)(struct AS_DSP *dsp, int fd);
};

typedef struct AS_DSP_OPS assist_dsp_ops_t;


#include "assist_dsp_dtmf.h"

struct AS_DSP
{
#if 0
	as_dsp_config				config;
	as_tone_calculator			cal;
	fsk_gen					fsk;
	as_fsk_decode_engine	fsk_decoder;
#endif
	
	as_law_type_t				law;

	int 						rxGain;
	int						txGain;

#if 0	
	assist_dtmf_detector_t		*dtmf;
#endif
	assist_dsp_ops_t				*ops;
};

typedef struct AS_DSP assist_dsp_t;


assist_dsp_t *assist_dsp_init(as_law_type_t law , int rxGain, int txGain, int dtmf_decode_buf_length );
void assist_dsp_destory(assist_dsp_t *dsp);

#define LCD_SCREEN_WRITE(str) do {printf(str);}while(0)
#define LCD_STATUS_WRITE(str) do {printf(str);}while(0)

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>
#include <errno.h>

//Add by chensir at 2005.12.01
// msleep() like sleep(), expect unit is millisecond;
void msleep(int m_sec);
/*
  * Clear LCD's screen.
  * if line_num > 0,clear the line of line_num.
  * if ine_num =0,clear all.
  * RETURNS: 
     	success > 0
  	failure < 0
  */
int as_lcd_clear(unsigned int line_num);
#ifdef __cplusplus
	};
#endif


#endif

