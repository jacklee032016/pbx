/*
* $Id: assist_init.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <getopt.h>
#include <sys/ioctl.h>

#include "TaskEvent.hxx"

#include "assist_lib.h"

using namespace Assist;

/* this struct is copy from assist_dev_ctrl.h */
struct as_channel_state
{
	int channel_no;
	int type;
	int available;//0 is available; -1 is the para over.
};
#include "cpLog.h"
#include "pstn.hxx"
#include "PstnEvent.hxx"
#include "PstnEndPoint.hxx"

#include "PbxConfiguration.hxx"

static as_span_t  master_span =
{
	name		:	"Assist devices span",
	timeout		:	 0,
//	spanMutex	:	PTHREAD_MUTEX_INITIALIZER,
};

void *fxsPlayFSK(void *param);
void *fxsEncodeFSK(void *param);
void *fxoDetectFSK(void *param);
void *fxoDetectDT(void *param);


/******** report Event into EndPoint of C++ environments ********/
void as_pstn_report_event(as_device_t *dev, DeviceEventType signal)
{
	PstnEndPoint *ep = static_cast<PstnEndPoint* >(dev->private_data); 
	assert(ep !=0 );
	ep->reportPEvent(signal);
}

static void __as_pstn_report_dtmf(as_device_t *dev, unsigned char digit)
{
	PstnEndPoint *ep = static_cast<PstnEndPoint* >(dev->private_data); 
	assert(ep !=0 );
	ep->genDtmfEvent(digit );
}

/******** report Event into EndPoint of C++ environments ********/
#if 0
static void __as_pstn_add_task(as_device_t *dev )
{
	PstnTaskEngine *taskEngine = static_cast<PstnTaskEngine* >(dev->span->private_data); 
	assert(taskEngine !=0 );
	taskEngine->addTask(dev);
}
#endif


static int as_pstn_dummy(as_device_t *dev)
{
	cpDebug(LOG_WARNING, "Dummy : not support dev operation\n");
	return 0;
}

static unsigned char as_fxo_get_dtmf_dummy(as_device_t *dev)
{
	cpDebug(LOG_WARNING, "Dummy : not support dev operation\n");
	return 0;
}

/* call this function in order to feedback IncomingSucess Signal */
static int as_fxo_dev_play_fastbusy(as_device_t *dev)
{
	dev->state = AS_DEVICE_STATE_ONHOOK;
	dev->carCount = PbxConfiguration::instance()->getCarCount( );
	return as_lib_onhook( dev->fd);
}

static int as_fxs_dev_play_dial(as_device_t *dev)
{
	return as_tone_play_dial( dev->fd );
}

static int as_fxs_dev_play_ringback(as_device_t *dev)
{
	return as_tone_play_ringback( dev->fd );
}

static int as_fxs_dev_play_busy(as_device_t *dev)
{
	return as_tone_play_busy( dev->fd );
}

static int as_fxs_dev_play_fastbusy(as_device_t *dev)
{
//	return as_tone_play_congestion( dev->fd );
	return as_tone_play_busy( dev->fd );
}

static int as_fxs_dev_play_callwait(as_device_t *dev)
{
	return as_tone_play_callwait( dev->fd );
}

static int as_fxs_dev_play_stop(as_device_t *dev)
{
	return as_tone_play_stop( dev->fd );
}


static int as_fxs_ring_start(as_device_t *dev, const char *callerId)
{
#if 0
	int pstnStandard = PbxConfiguration::instance()->getPstnStandard();
	
	if( pstnStandard==PSTN_STANDARD_CHINA && PbxConfiguration::instance()->isCallIdOn() )
		return as_ring_with_dtmf_caller_id(dev->fd, (unsigned char *)callerId);
	else
#endif
	dev->carCount = 0;	/*used as count of RINGEROFF signal */
	return as_ring_on_hook( dev->fd);
}

static int as_fxs_ring_stop(as_device_t *dev)
{
	return as_stop_ring(dev->fd);
}


static int as_pstn_query_status(as_device_t *dev )
{
	pthread_mutex_lock(&dev->span->spanMutex);
	dev->isQuery = 1;
	pthread_mutex_unlock(&dev->span->spanMutex);
	return 0;
}


static void as_pstn_get_status(as_device_t *dev )
{
	as_state_t state;

	pthread_mutex_lock(&dev->span->spanMutex);

	if(!dev->isQuery)
	{
		pthread_mutex_unlock(&dev->span->spanMutex);
		state = AS_DEVICE_STATE_INVALIDATE;
	}	
	else
	{
		dev->isQuery = 0;
		pthread_mutex_unlock(&dev->span->spanMutex);
		if(dev->phyType == as_fxo_device)/* ONHOOK is idle for FXO device */
		{
			if(as_device_check_fxo_online(dev->fd, dev->name) == 0)
			{/* FXO/DAA(%s) has no line plugin */
				cpLog(LOG_WARNING, "FXO has no line plugin!Please check it.");
				state = AS_DEVICE_STATE_OFFHOOK;
			}
			else
			{
				state = AS_DEVICE_STATE_ONHOOK;
			}
		}	
		else
			state = dev->state;
		
		cpDebug(LOG_DEBUG, "Check status for device %s  is %d\n",dev->name, state); 
		if(state ==  AS_DEVICE_STATE_ONHOOK)
		{	
			state = AS_DEVICE_STATE_IDLE;
			as_pstn_report_event(dev, DeviceEventIdle );
		}	
		else if(state ==  AS_DEVICE_STATE_OFFHOOK)
		{	
			state = AS_DEVICE_STATE_BUSY;
			as_pstn_report_event(dev, DeviceEventBusy );
		}
		else 
		{	
			state = AS_DEVICE_STATE_INVALIDATE;
		}
	}
	
	return ;
}

static int as_fxo_send_callid(as_device_t *dev, char *phone)
{
	cpDebug(LOG_DEBUG, "CallerID is not send in FXO");
	return 0;
}

static int as_fxs_send_callid(as_device_t *dev, char *phone)
{
//	usleep(2000*1000);  /* sleep at least 1/2 sec */
	cpDebug(LOG_DEBUG, "CallerID : %s", phone);
	pthread_mutex_lock(&dev->span->spanMutex);

	sprintf((char *)dev->calledPhoneNumber, "%s", phone);

	new TaskEvent(fxsPlayFSK, (void *)dev);

	pthread_mutex_unlock(&dev->span->spanMutex);	

	return 0;
}

/* ignore both RINGOFFHOOK and ONHOOK event, replaced with RINGEROFF */
static void  as_fxo_check_status(as_device_t *dev)
{
	int res;

	res = as_lib_event_get( dev->fd);
	
	if( res== AS_EVENT_RINGERON || res== 0 /*AS_EVENT_NONE */)
	{/* ignore */
	}
	
	if( res == AS_EVENT_RINGEROFF )
//	if( res == AS_EVENT_RINGOFFHOOK )
	{
		if( dev->state == AS_DEVICE_STATE_ONHOOK )
		{/* first RING, so DSP detect CallerId or DID */
			dev->state = AS_DEVICE_STATE_PENDING;
			
			if(PbxConfiguration::instance()->getPstnStandard() == PSTN_STANDARD_CHINA )
			{
				cpDebug(LOG_DEBUG, "\n\tCallerID detect in Chinese standard being on %s\n", dev->name );
#if 0				
				dev->taskType = PSTN_TASK_FXO_IN_DETECT_FSK;
				__as_pstn_add_task(dev);
#else
				new TaskEvent(fxoDetectFSK, (void *)dev);
#endif
			}
			
			cpDebug(LOG_DEBUG, "\n\n%s RX first ringing, enter into pending state\n\n", dev->name);
		}
		else if(dev->state == AS_DEVICE_STATE_PENDING )
		{/* Second RING, so report event to EP statemachine, then off-hook */
		
			/* really offhok only happened when OpFarEndAnswer */
			//as_lib_offhook( dev->fd );
			dev->state = AS_DEVICE_STATE_OFFHOOK;
			/* this event is executed in OpStartCall */
			as_pstn_report_event( dev, DeviceEventHookUp);
			cpDebug(LOG_DEBUG, "\n\n%s Hookup event send to EP after second ringing\n\n", dev->name);

			const char *number;
			if( strlen((const char *)dev->calledPhoneNumber) != 0 )
			{/* DID call */
				unsigned int i;
				number = (const char *)dev->calledPhoneNumber;
				for(i=0; i<strlen(number); i++)
				{
					__as_pstn_report_dtmf(dev, number[i] );
				}
			}
			else
			{/* Console for second dial */
				PstnEndPoint *ep = static_cast<PstnEndPoint* >(dev->private_data); 
				assert(ep !=0 );
				ep->callToConsole();
				if(res == 0)
				{/* add more logic when no console is available */
					cpDebug(LOG_DEBUG, "Call End!");
				}
			}
		}
		else
		{
			cpDebug(LOG_DEBUG, "%s has been in OFF-HOOK State(%d)" ,dev->name,  dev->state );
		}
	}
	else if ((res== AS_EVENT_CAR) ) 
	{/* polarity reversal, then enter into PENDING state */
		if (dev->state == AS_DEVICE_STATE_ONHOOK)
		{
			if(dev->carCount == PbxConfiguration::instance()->getCarCount( ))
			{/* first CAR signal */
				as_pstn_report_event( dev, DeviceEventCar );
				cpDebug(LOG_DEBUG, "First Car Signal(CAR begin) detected on %s", dev->name);
			}
			dev->carCount--;
			if(dev->carCount<=0)
			{
				as_pstn_report_event( dev, DeviceEventCar );
//				dev->carCount = PbxConfiguration::instance()->getCarCount();
				cpDebug(LOG_DEBUG, "Last Car Signal(CAR end) detected on %s", dev->name );
			}
		}
		else
		{
			cpDebug(LOG_ERR, "CAR detect when in state OFFHOOK on %s", dev->name);
		}
	}
/*	
	else if(res == AS_EVENT_ONHOOK)
	{	
		if(dev->state == AS_DEVICE_STATE_OFFHOOK)
		{
			dev->state = AS_DEVICE_STATE_ONHOOK;
		}
	}
*/	
#if 0	
	else if(res == AS_EVENT_RINGOFFHOOK )
	{/* off-hook OK */
		cpDebug(LOG_DEBUG, "\n\n\tFXO %s RX OFFHOOK\n", dev->name);

		if(dev->state == AS_DEVICE_STATE_OFFHOOK)
		{	
			cpDebug(LOG_DEBUG, "\n\n\tFXO %s RX Dialing with %s\n", dev->name,  fxo->callingBuf);
			as_dsp_play_dtmf( fxo->dsp, dev->fd, fxo->callingBuf);
		}	
	}
#endif

	if(dev->state == AS_DEVICE_STATE_PENDING)
	{
//		cpDebug(LOG_DEBUG, "FXO(DAA) : Check CallerID and DID CalleeID\n");
		/* added code here ,lizhijie, 2006.03.24 */
		/* read from FD, use DSP decode it and store to send out for CallerId and DID */
	}
}

static as_state_t __as_pstn_check_status(as_device_t *dev )
{
	int res;

	res = as_lib_event_get( dev->fd);
#if 0
	pthread_mutex_lock(&mutSlic);
	if(res == AS_DEVICE_STATE_INVALIDATE)
		res = pstn[dev->phyId].pstnState;
	
	pthread_mutex_unlock(&mutSlic);
#endif 
	if(res == AS_EVENT_RINGOFFHOOK)
	{
		cpDebug(LOG_DEBUG, "\r\n************OFFHOOK event checked on device %s\r\n" , dev->name );
		return AS_DEVICE_STATE_OFFHOOK;
	}
	else if( res ==AS_EVENT_WINKFLASH)
	{
		cpDebug(LOG_DEBUG, "\r\n************WINFLASH event checked on device %s\r\n" , dev->name );
		return AS_DEVICE_STATE_WINKFALSH;
	}
	else if( res ==AS_EVENT_ONHOOK) 
	{
		cpDebug(LOG_DEBUG, "\r\n************ONHOOK event checked on device %s\r\n" , dev->name );
		return AS_DEVICE_STATE_ONHOOK;
	}
	else if( res ==AS_EVENT_RINGEROFF) 
	{
		cpDebug(LOG_DEBUG, "\r\n************RINGEROFF event checked on device %s\r\n" , dev->name );
		dev->carCount++;
		if( (dev->carCount ==1) && (PbxConfiguration::instance()->getPstnStandard() == PSTN_STANDARD_CHINA ) )
			as_pstn_report_event(dev, DeviceEventRingerOff );
		return AS_DEVICE_STATE_ONHOOK;
	}
	else
	{
//		cpDebug(LOG_DEBUG, "%s RX %d event\n", dev->name , res);
	}

	return AS_DEVICE_STATE_INVALIDATE;
}

static void as_fxs_check_status(as_device_t *dev)
{
	as_state_t state = __as_pstn_check_status(dev);
	
	if(state == AS_DEVICE_STATE_OFFHOOK)
	{
		if(dev->state == AS_DEVICE_STATE_ONHOOK)
		{/* report event to EP and change into new state */
			as_pstn_report_event(dev, DeviceEventHookUp);
			dev->state = AS_DEVICE_STATE_OFFHOOK;
		}
	}
	else if(state == AS_DEVICE_STATE_ONHOOK )
	{/* FXS : from OFF-HOOK ro ON-HOOK, need a debounce */
		if(dev->state == AS_DEVICE_STATE_OFFHOOK)
		{	
		/* from OFFHOOK --> ONHOOK, pending must be first*/		

//			dev->state = AS_DEVICE_STATE_PENDING;

			/* return, then state change must be in next loop, so delayed */
			as_pstn_report_event(dev, DeviceEventHookDown);
			dev->state = AS_DEVICE_STATE_ONHOOK;
			return ;
		}
#if 0		
		else if(dev->state == AS_DEVICE_STATE_PENDING)
		{
			dev->state = AS_DEVICE_STATE_ONHOOK;
		}
#endif

	}
	else if( state == AS_DEVICE_STATE_WINKFALSH)
	{
		as_pstn_report_event(dev, DeviceEventFlash );
	}

	if(dev->state== AS_DEVICE_STATE_PENDING)
	{
		state = as_device_check_state_on_startup(dev->fd, dev->name);
		if(state == AS_DEVICE_STATE_ONHOOK )
		{
			dev->state = AS_DEVICE_STATE_ONHOOK;
			as_pstn_report_event( dev, DeviceEventHookDown);
		}
	}

	if(dev->state == AS_DEVICE_STATE_OFFHOOK)
	{
		unsigned char dtmf = as_get_dtmf_4_fxs(dev->fd);

		if(dtmf != 'X' && dtmf!=0xff )
		{
			cpDebug(LOG_DEBUG, "DTMF '%c'(0x%x)", dtmf, dtmf);
			__as_pstn_report_dtmf( dev, dtmf);
		}	
	}
}

static int as_fxo_ring_start(as_device_t *dev, const char *calleeNumber)
{
	pthread_mutex_lock(&dev->span->spanMutex);
	
	as_lib_offhook(dev->fd);
	dev->state = AS_DEVICE_STATE_OFFHOOK;


	if(PbxConfiguration::instance()->getFxoLineMode() == FXO_LINE_INDIRECT )
	{
		as_pstn_report_event( dev, DeviceEventHookUp);
	}
	else
	{
		strcpy(dev->callingBuf, calleeNumber);
		cpDebug(LOG_DEBUG, "\n\n\tFXO %s ringstart with '%s'\n", dev->name, dev->callingBuf);
		new TaskEvent(fxoDetectDT, (void *)dev);
	}
	
	pthread_mutex_unlock(&dev->span->spanMutex);	

	return 1;
}

/* send CAR signal to FXS port. used as query_channel */
static int as_fxs_send_car(as_device_t *dev)
{
	cpDebug(LOG_WARNING, "send CAR on FXS for CallerID");
	pthread_mutex_lock(&dev->span->spanMutex);
	as_car_on_fxs(dev->fd);

#if 0
	/* add a task for FSK modem data calcaulation, callerID is save as EP->peerNumber */
	dev->taskType = PSTN_TASK_FXS_ENCODE_FSK;
#endif

	pthread_mutex_unlock(&dev->span->spanMutex);	
	return 0;
}

static int as_fxo_ring_stop(struct AS_DEVICE* dev)
{/* for FXO, it is called when leave StateRinging */
	pthread_mutex_lock(&dev->span->spanMutex);
	
	dev->taskType = PSTN_TASK_UNINIT;//FXO_CALL_DIRECTION_UNKNOWN;
	if( dev->state == AS_DEVICE_STATE_OFFHOOK)
		as_lib_onhook(dev->fd);
	dev->state = AS_DEVICE_STATE_ONHOOK;

	pthread_mutex_unlock(&dev->span->spanMutex);	
	return 0;
}

static unsigned char  as_fxs_get_digit(as_device_t *dev)
{
	return as_get_dtmf_4_fxs(dev->fd) ;
}

static int as_fxo_primary_answer(as_device_t *dev)
{
	pthread_mutex_lock(&dev->span->spanMutex);	

	cpDebug(LOG_DEBUG, "FXO %s Primary Answer for CAR\n", dev->name);
	as_lib_offhook(dev->fd);

	dev->calledPhoneNumber[0] = 0;	/* terminate this char array */
	dev->carCount = 0;
	dev->state = AS_DEVICE_STATE_OFFHOOK;
	
#if 0
	dev->taskType = PSTN_TASK_FXO_IN_DETECT_FSK;
	__as_pstn_add_task(dev);
#else
	new TaskEvent(fxoDetectFSK, (void *)dev);
#endif
	
	pthread_mutex_unlock(&dev->span->spanMutex);	
	return 1;
}

static int as_fxo_incoming_sucess(as_device_t *dev)
{
	pthread_mutex_lock(&dev->span->spanMutex);	

	cpDebug(LOG_DEBUG, "FXO %s Primary Answer for CAR\n", dev->name);
	dev->taskType = PSTN_TASK_UNINIT;
	as_lib_onhook(dev->fd);
	dev->state = AS_DEVICE_STATE_ONHOOK;
	
	pthread_mutex_unlock(&dev->span->spanMutex);	
	return 1;
}

/* stop ringback tone in peer switch */
static int as_fxo_offhook(as_device_t *dev)
{
	pthread_mutex_lock(&dev->span->spanMutex);	

	cpDebug(LOG_DEBUG, "FXO %s offhook\n", dev->name);
	as_lib_offhook(dev->fd);
	pthread_mutex_unlock(&dev->span->spanMutex);	
	return 1;
}

#if 0
static int as_fxo_dialing_out(as_device_t *dev)
{
	FxoDialMode dialMode = PbxConfiguration::instance()->getFxoDialMode();
	pthread_mutex_lock(&dev->span->spanMutex);	

	cpDebug(LOG_DEBUG, "%s dialing with %s\n\n", dev->name, dev->callingBuf );
	switch(dialMode)
	{
		case FXO_DIAL_DP10:
		{
			cpDebug(LOG_DEBUG, "FXO DP 10PPS\n\n" );
			assist_dsp_fxo_send_pps(dev->fd, dev->callingBuf ,AS_DP_10PPS);
			break;
		}
		case FXO_DIAL_DP20:
		{
			cpDebug(LOG_DEBUG, "FXO DP 20PPS\n\n" );
			assist_dsp_fxo_send_pps(dev->fd, dev->callingBuf ,AS_DP_10PPS);
			break;
		}	
		default:
		{
			cpDebug(LOG_DEBUG, "DTMF dial '%s'\n\n" ,dev->callingBuf);
			as_dsp_play_dtmf( dev->dsp, dev->fd, dev->callingBuf );
			break;
		}	
	}
	dev->callingBuf[0] = 0;
	
	pthread_mutex_unlock(&dev->span->spanMutex);	
	return 1;
}
#endif

/* as_operation_t is executed in the context of DeviceCommandThread */
/* order of fields musy be the same as defined in structure */
static as_operations_t	fxo_ops =
{
	ring_start				: as_fxo_ring_start,
	ring_stop				: as_fxo_ring_stop,
	
	send_callid			: as_fxo_send_callid,
	
	tone_dial_start		: as_pstn_dummy,
	tone_dial_stop			: as_pstn_dummy,

	tone_ringback_start	: as_pstn_dummy, //as_fxo_dialing_out,
	tone_ringback_stop	: as_fxo_offhook,

	
	tone_busy_start		: as_pstn_dummy,
	tone_busy_stop		: as_pstn_dummy,
	tone_fastbusy_start	: as_fxo_dev_play_fastbusy,		/* it is call end, for FXO, so on-hook*/
	tone_fastbusy_stop		: as_pstn_dummy,
	tone_callwait_start		: as_pstn_dummy,
	tone_callwait_stop		: as_pstn_dummy,

	report_dtmf_digit		: as_fxo_get_dtmf_dummy,

	check_status			: as_fxo_check_status,
	
	query_status 			: as_pstn_query_status,
	get_status 			: as_pstn_get_status,	

	getPcmChannelId		: as_pstn_dummy,
	query_channel              : as_pstn_dummy,
//	setup_gain			: as_channel_gain_setup

	send_primary_answer	: as_fxo_primary_answer,
	send_incoming_success	: as_fxo_incoming_sucess 
};

static as_operations_t	fxs_ops =
{
	ring_start				: as_fxs_ring_start,
	ring_stop				: as_fxs_ring_stop,
	
	send_callid			: as_fxs_send_callid,					/* send CALLER ID called by DeviceSignalCarCallerID */
	
	tone_dial_start		: as_fxs_dev_play_dial,
	tone_dial_stop			: as_fxs_dev_play_stop,
	tone_ringback_start	: as_fxs_dev_play_ringback,
	tone_ringback_stop	: as_fxs_dev_play_stop,
	tone_busy_start		: as_fxs_dev_play_busy,
	tone_busy_stop		: as_fxs_dev_play_stop,
	tone_fastbusy_start	: as_fxs_dev_play_fastbusy,			/* it is call end, for FXS, play busy tone */
	tone_fastbusy_stop		: as_fxs_dev_play_stop,
	tone_callwait_start		: as_fxs_dev_play_callwait,
	tone_callwait_stop		: as_fxs_dev_play_stop,

	report_dtmf_digit		: as_fxs_get_digit,

	check_status			: as_fxs_check_status,
	query_status 			: as_pstn_query_status,
	get_status 			: as_pstn_get_status,	

	getPcmChannelId		: as_pstn_dummy,
	query_channel             : as_fxs_send_car,					/* send CAR and begin to calculate FSK modem data */
//	setup_gain			: as_channel_gain_setup
	send_primary_answer	: as_pstn_dummy,
	send_incoming_success	: as_pstn_dummy
};


as_span_t  *as_check_hardware(const char *name, assist_app_type appType )
{
	int  i;
	int count = 0;
	int iFxo = 0;
	int fd;
	char devicename[AS_DEVICE_NAME_LENGTH];
	as_span_t	*span;
	as_device_t	*dev;
	
	span = &master_span;
//	span->spanMutex = PTHREAD_MUTEX_INITIALIZER;
	pthread_mutex_init(&span->spanMutex, NULL);
	if(appType == assist_app_slic)
	{
		count = as_get_device_num();
		if(count <= 0)
			return 0;
		
		span->count = count;
		cpDebug(LOG_DEBUG, "PSTN %d devices are found", span->count);
		
		for(i=0; i<count; i++)
		{
			dev = (as_device_t *)malloc(sizeof(as_device_t) );
			assert(dev!=0);
			memset(dev, 0, sizeof(as_device_t));

			dev->id = i;
			span->devs[i] = dev;
			dev->span = span;
			dev->type = as_device_slic;

			dev->btBufCount = 600;
			
			dev->carCount = PbxConfiguration::instance()->getCarCount();
			dev->dsp = as_dsp_init(PbxConfiguration::instance()->getDspConfig());
			dev->taskType = PSTN_TASK_UNINIT;

			struct as_channel_state *chanstate = as_get_dev_state( i);
			if(chanstate->type==AS_CHANNEL_TYPE_FXS)
			{
				sprintf(dev->name, "%s_%d_FXS", name, i);
				cpDebug(LOG_DEBUG, "Device NO. %d is FXS(SLIC)\n", i );
				dev->phyType =  as_fxs_device;
				dev->ops = &(fxs_ops);
				dev->phyId = i;

				if( chanstate->available==AS_TXSTATE_ONHOOK )
				{
					cpDebug(LOG_DEBUG, "'%s' : Device is ON-HOOK\n", dev->name);
					dev->state = AS_DEVICE_STATE_ONHOOK;
				}	
				else if( chanstate->available == AS_TXSTATE_OFFHOOK)
				{
					cpDebug(LOG_DEBUG, "'%s' : Device is OFF-HOOK\n", dev->name);
					dev->state = AS_DEVICE_STATE_OFFHOOK;
				}	
				else
				{
					cpDebug(LOG_DEBUG, "'%s' : Device state is invalidate, check your hardware and driver carefully\n", dev->name);
					assert(1);
				}
			}
			else if(chanstate->type==AS_CHANNEL_TYPE_FXO)
			{
				sprintf(dev->name, "%s_%d_FXO", name, i);
				cpDebug(LOG_DEBUG, "Device No. %d is FXO(DAA)\n", i);
				dev->phyType = as_fxo_device;
				dev->ops = &(fxo_ops);
				
				if( chanstate->available==AS_TXSTATE_ONHOOK )
				{
					cpDebug(LOG_DEBUG, "'%s' : Device is ON-HOOK\n", dev->name);
				}	
				else if( chanstate->available == AS_TXSTATE_OFFHOOK)
				{
					cpDebug(LOG_DEBUG, "'%s' : Device is OFF-HOOK\n", dev->name);
				}	
				else
				{
					cpDebug(LOG_DEBUG, "'%s' : Device state is invalidate, check your hardware and driver carefully\n", dev->name);
					assert(1);
				}
				dev->phyId = iFxo;
				dev->state = AS_DEVICE_STATE_ONHOOK;//as_device_check_state_on_startup( fd, devicename);
				span->fxoDevs[iFxo] = dev;
				iFxo++;
			}

			free(chanstate);
		}
	}
	else
	{
		cpDebug(LOG_DEBUG, "PSTN plugin configuration error. This is SLIC/DAA!PCM is not support now!\n");
		assert(1);
	}
	span->fxoCount = iFxo;

	for(i=0; i< span->count; i++)
	{
		sprintf(devicename, "%s/%d", AS_DEVICE_NAME, i);

		fd = open( devicename, O_RDWR );
		span->devs[i]->fd = fd;
		if( fd <= 0 )
		{
			cpDebug(LOG_ERR,  "Device '%s' Open failed. Error: '%s'.\r\n",  devicename,  strerror(errno) );
			continue;
		}
		as_lib_onhook(span->devs[i]->fd);
		if( span->devs[i]->phyType == as_fxs_device) 
			as_device_standard_config(span->devs[i]->fd, PbxConfiguration::instance()->getPstnStandard());
	}

	cpDebug(LOG_DEBUG, "Device is Assist %s card\r\n" ,name );

	assert(count!=0);
	sprintf(span->name, "ASSIST");

	return span;
}

