#ifndef  __PSTN_HXX__
#define __PSTN_HXX__
/*
 * encapsulate all interface with hardware device drivers for C++ environments
 * Li Zhijie , 2006.02.04
*/
#include "PstnEvent.hxx"

namespace Assist
{

#define DTMF_DETECT_STEP_SIZE			102

#define AS_DEVICE_NAME					"/dev/astel"
#define AS_DEVICE_MAX					16

/* why this structure is defined for SLIC device, lizhijie, 2005.12.17 */
#define AS_DIALING_MAX_NUM 					16

/* duration between PrimaryAnswer and IncomingSuccess for FSK demodulation */
#define	NTT_FSK_DURATION_3_SECONDS			150		/* in unit of 20 ms */


typedef enum	
{
	assist_app_unknow = -1,
	assist_app_slic = 0,
	assist_app_pcm =1
}assist_app_type;


#define DEBUG_ASSIST_DEVICE			1	/* debug info for /dev/astel and the associated */			

#define PSTN_DEVICES_EPOCH				0x80
#define VIRTUAL_DEVICES_EPOCH			1000

#define 	DTMF_PARSE_LENGTH			(DTMF_DETECT_STEP_SIZE*10)

#define FLASH_TIMEOUT 					1000000

#define AS_RTP_JITTER_NEW				-1 /* bypass jitter buffer */

#define AS_DEVICE_NAME_LENGTH			32

struct AS_SPAN;
struct AS_DEVICE;

struct AS_DEV_OPERATIONS
{
	/* for FXS, callerId is Caller Phone number, ring the phone; 
	for FXO, callerId is Callee Phone Number, init a new call */
	int (* ring_start)(struct AS_DEVICE *dev, const char *callerId);
	int (* ring_stop)(struct AS_DEVICE *dev);

	int (* send_callid)(struct AS_DEVICE *dev, char *phone);	
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

	unsigned char (*report_dtmf_digit)(struct AS_DEVICE *dev);
	
	/* state define : refer to as_state_t , enum defined in assist_lib.h */
	void  (*check_status)(struct AS_DEVICE *dev);

 	int  (*query_status)(struct AS_DEVICE *dev);

 	void    (*get_status)(struct AS_DEVICE *dev);

	/* following are only used in PCM device */
	int (*getPcmChannelId)(struct AS_DEVICE *dev );
	int (*query_channel)(struct AS_DEVICE* dev);

	int (*send_primary_answer)(struct AS_DEVICE *dev);
	int (*send_incoming_success)(struct AS_DEVICE *dev);
	
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
	as_fxs_device = 1,
	as_virtual_device ,
	as_fxo_device,
	as_invalid_device,
	as_other_device
}device_type_t;


typedef enum
{
	PSTN_TASK_UNINIT					= 0,
	PSTN_TASK_FXO_OUT_DT_DETECT,				/* used to dial dtmf calling phone number */
	PSTN_TASK_FXO_IN_DETECT_FSK,					/* used to detect caller ID */
	PSTN_TASK_FXO_BT_DETCET,						/* BT detect on a FXO port */
	PSTN_TASK_FXS_ENCODE_FSK,					/* FSK modem data calculated in FXS port */
	PSTN_TASK_FXS_PLAY_FSK,						/* play FSK modem data just encoded */
	PSTN_TASK_UNKNOWN							/* this must be the last item */
}pstn_task_type_t;


struct AS_DEVICE;

struct AS_SPAN
{
	char 				name[AS_DEVICE_NAME_LENGTH];
	int 					timeout; 						/* in ms */
	
	int 					count;							/* count of hardware device such as SLIC or PCM  */
	struct AS_DEVICE		*devs[AS_DEVICE_MAX];
	
	int					fxoCount;						/* count of FXO devoce */
	struct AS_DEVICE		*fxoDevs[AS_DEVICE_MAX];		/* point to AssistPbxDevice */
	
	void 				*private_data;
	void					*invalid_dev;

	pthread_mutex_t 		spanMutex;
};

typedef	enum
{
	BT_DETECT_STAGE_INVALIDATE = 0,
	BT_DETECT_STAGE_INPUT,
	BT_DETECT_STAGE_CHECK
}bt_detect_stage_t;

typedef struct AS_SPAN as_span_t;
	
struct AS_DEVICE
{
	char 				name[AS_DEVICE_NAME_LENGTH];
	int 					fd;
	as_state_t			state;
	as_device_type_t		type;
	int 					lock;						/* 1: locked, 0:unlocked */
	int 					id; 							/* position in the devices array of span */

	int					phyId;						/*phyId*/

	as_span_t			*span;
	void 				*private_data;				/* point to AssistDeviceInfo */
	device_type_t			phyType;

	int					carCount;					/* count of FXO CAR signal */
	unsigned char			calledPhoneNumber[32];		/* keep called phone number get from FSK for FXO device */

	as_operations_t		*ops;

	int					isQuery;

	as_dsp_t				*dsp;
	pstn_task_type_t		taskType;
	int					timeout;							/* for timeout when in PstnTask */
	
	char 				callingBuf[AS_DIALING_MAX_NUM];/* peer phone number for call originate from our FXO port */

	char					*busyData;
	bt_detect_stage_t		btStage;
	int					btBufCount;
	int					btBufIndex;
};

typedef struct AS_DEVICE as_device_t;



#ifdef  __cplusplus
	extern "C"
	{
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>
#include <errno.h>

#ifdef __cplusplus
	};
#endif

}

#endif

