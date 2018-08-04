/*$Id: assist_init.c,v 1.5 2007/03/27 20:23:07 lizhijie Exp $*/
#include "pbx_globals.h"
#include "assist_globals.h"
#include "math.h"
#include "as_dev_ctl.h"

typedef enum
{
	AS_CHANNEL_TYPE_FXS,			/* FXS device channel */
	AS_CHANNEL_TYPE_FXO			/* FXO device channel */
} as_channel_type_t;

static as_span_t  master_span =
{
	name		:	"Assist devices span",
	timeout		:	 0
};

#if 0
/* support both SLIC and PCM device */
void as_channel_gain_setup(as_device_t *dev, int rx, int tx)
{
	float tx_db, rx_db;
	int law;
	tx_db = 20.0*log10( tx );
	rx_db = 20.0*log10( rx );
	if( dev->type==as_device_slic )
		law = U_LAW_CODE;
	else if(dev->type == as_device_pcm )
		law = A_LAW_CODE;
	else
	{
		AS_MSG_LOG(AS_LOG_ERR,"Not support device Type of '%d'\r\n", dev->type);
		exit(1);
	}
	as_dsp_set_gain(dev->fd, law, rx_db, tx_db);
}
#endif


static as_operations_t	slic_ops =
{
	ring_start				: as_dev_ring_start,
	ring_stop				: as_dev_ring_stop,
	tone_dial_start		: as_tone_dev_play_dial,
	tone_dial_stop			: as_tone_dev_play_stop,
	tone_ringback_start	: as_tone_dev_play_ringback,
	tone_ringback_stop	: as_tone_dev_play_stop,
	tone_busy_start		: as_tone_dev_play_busy,
	tone_busy_stop		: as_tone_dev_play_stop,
	tone_fastbusy_start	: as_tone_dev_play_fastbusy,
	tone_fastbusy_stop		: as_tone_dev_play_stop,
	tone_callwait_start		: as_tone_dev_play_callwait,
	tone_callwait_stop		: as_tone_dev_play_stop,
	check_status			: as_device_check_status,
	report_dtmf_digit		: as_dtmf_get_digit,

	//added by lijie
	query_status 			: as_device_query_status,
	get_status 			: as_device_get_status,	
	getPcmChannelId		: NULL,
	trans_telnumber		: as_device_trans_telnumber,

//	setup_gain			: as_channel_gain_setup

};

static as_operations_t	pcm_ops =
{
	ring_start				: pcm_dev_ring_start,
	ring_stop				: pcm_dev_ring_stop,
	tone_dial_start		: pcm_dev_tone_dial_play,
	tone_dial_stop			: pcm_dev_tone_dial_stop,
	tone_ringback_start	: pcm_dev_tone_ringback_play,
	tone_ringback_stop	: pcm_dev_tone_ringback_stop,
	tone_busy_start		: pcm_dev_tone_busy_play,
	tone_busy_stop		: pcm_dev_tone_play_stop,
	tone_fastbusy_start	: pcm_dev_tone_fastbusy_play,
	tone_fastbusy_stop		: pcm_dev_tone_play_stop,
	tone_callwait_start		: pcm_dev_tone_callwait_play,
	tone_callwait_stop		: pcm_dev_tone_play_stop,
	
	check_status			: pcm_dev_check_status,
	report_dtmf_digit		: pcm_dev_dtmf_get_digit,

	getPcmChannelId		: pcm_dev_get_channel_id,
	//added by lijie to query device status
	query_status			: pcm_dev_query_status,
	//added by lijie to get device status
	get_status			: pcm_dev_get_status,
	trans_telnumber		: pcm_dev_trans_telnumber

};

log_stru_t deviceLog = 
{
	lstyle		:	USE_FILE,
	llevel		: 	AS_LOG_NOTICE,
	lfacility		:	AS_LOG_7,
	logFileName	:	AS_LOG_FILE_PCM_DEVICE,
	isDaemonized	:	0
};


/* following are support both in PCM and SLIC */
int as_set_linear(int fd, int linear)
{
	int res;
	res = ioctl( fd, AS_CTL_SETLINEAR, &linear);
	if (res)
		return res;
	return 0;
}

/* law is defined in rtp/rtpTypes.h */
int as_set_law(int fd, int law, const char *devicename)
{
	int res = 1;
	int local_law;

	if(law == 0)
		local_law = AS_LAW_MULAW;
	else if(law == 8)
		local_law = AS_LAW_ALAW;
	else if(law == 10 || law == 11 )
	{
		return as_set_linear(fd, res);
	}
	else
	{
		AS_MSG_LOG(AS_LOG_WARNING, "Assist device '%s' can not support your code of '%s'", law);
	}

	res = ioctl( fd, AS_CTL_SETLAW, &local_law);
	if (res)
		return res;
	return 0;
}


as_span_t  *as_check_hardware(const char *name, assist_app_type appType, int law, int ipstnCount, int level)
{
	int  i;
	int count = 0;
	int iPstn = 0;
	int fd;
	char devicename[AS_DEVICE_NAME_LENGTH];
	as_span_t	*span;
	as_device_t	*dev;
	struct as_channel_state chanel[AS_DEVICE_MAX];
	
	span = &master_span;
	if(appType == assist_app_slic)
	{
		span->ops = &(slic_ops);
		sprintf(deviceLog.name, "%s", "SLIC");
	}	
	else
	{
		span->ops = &(pcm_ops);
		sprintf(deviceLog.name, "%s", "PCM");
	}
	
	if(level<=AS_LOG_DEBUG && level>= AS_LOG_EMERG)
		deviceLog.llevel = level;

	if (as_log_init( &deviceLog) < 0)
	{
		printf("%s Device Log File %s Init Failed.\n",deviceLog.name, deviceLog.logFileName);
	}

	if(ipstnCount>AS_DEVICE_MAX/2)
		ipstnCount = AS_DEVICE_MAX/2;
	if(appType == assist_app_slic)
	{
		as_get_channel_state(chanel);

		for(i=0; i<AS_DEVICE_MAX; i++)
		{
	
			sprintf(devicename, "%s/%d", AS_DEVICE_NAME, i);

			fd = open( devicename, O_RDWR );

			if( fd <= 0 )
			{
				AS_MSG_LOG(AS_LOG_ERR, "Device '%s' Open failed. Error: '%s'.",  devicename,  strerror(errno) );
				continue;
			}

			dev = (as_device_t *)malloc(sizeof(as_device_t) );
			assert(dev!=0);

			if(1)//chanel[i].type == AS_CHANNEL_TYPE_FXS)
			{
				sprintf(dev->name, "%s_HARDWARE_%d", name, i);
		
				dev->id = count;
				dev->phyId = i;
				span->devs[count] = dev;
				dev->span = span;
				dev->fd = fd;
				dev->type = as_device_slic;
				dev->phyType =  as_hardware_device;


				as_lib_onhook(dev->fd);
	
				dev->state = AS_DEVICE_STATE_ONHOOK; //as_device_check_state_on_startup( fd, devicename);
		
				if(dev->state == AS_DEVICE_STATE_ONHOOK )
				{
					AS_MSG_LOG(AS_LOG_INFO,"Device '%s_HARDWARE_%d' is in the state of ONHOOK", name, i);
				}
				else
				{
					AS_MSG_LOG(AS_LOG_INFO,"Device '%s_HARDWARE_%d' is in the state of OFFHOOK", name, i);
				}
			}
			else if(chanel[i].type == AS_CHANNEL_TYPE_FXO)
			{
				sprintf(dev->name, "%s_PSTN_%d", name, iPstn);
		
				dev->id = iPstn+PSTN_DEVICES_EPOCH;
				dev->phyId = i;
				span->devs[count] = dev;
				dev->span = span;
				dev->fd = fd;
				dev->type = as_device_slic;
				dev->phyType = as_pstn_device;

				as_lib_onhook(dev->fd);
	
				dev->state = AS_DEVICE_STATE_ONHOOK;//as_device_check_state_on_startup( fd, devicename);
		
				if(dev->state == AS_DEVICE_STATE_ONHOOK )
				{
					AS_MSG_LOG(AS_LOG_INFO,"Device '%s_PSTN_%d' is in the state of ONHOOK", name, iPstn);
				}	
				else
				{
					AS_MSG_LOG(AS_LOG_INFO,"Device '%s_PSTN_%d' is in the state of OFFHOOK", name, iPstn);
				}
				iPstn++;
			}

			count++;

		}
	}
	else
	{
		for(i=0; i<AS_DEVICE_MAX/2; i++)
		{
	
			dev = (as_device_t *)malloc(sizeof(as_device_t) );
			assert(dev!=0);

			sprintf(dev->name, "%s_HARDWARE_%d", name, i);

			dev->id = i;
			dev->phyId = i;
			
			span->devs[i] = dev;
			dev->span = span;

			dev->fd = AS_DEVICE_NOT_DEFINED;
			dev->type = as_device_pcm;
			dev->phyType = as_hardware_device;
			dev->state = AS_DEVICE_STATE_ONHOOK;
					
			AS_MSG_LOG(AS_LOG_INFO, "Device '%s_HARDWARE_%d' is in the state of ONHOOK", name, i);
		
			count ++;

		}

		for(i=0; i<ipstnCount; i++)
		{
			dev = (as_device_t *)malloc(sizeof(as_device_t) );
			assert(dev!=0);

			sprintf(dev->name, "%s_PSTN_%d", name, i);

			dev->id = i+PSTN_DEVICES_EPOCH;
			dev->phyId = i+AS_DEVICE_MAX/2;
			
			span->devs[i+AS_DEVICE_MAX/2] = dev;
			dev->span = span;

			dev->fd = AS_DEVICE_NOT_DEFINED;
			dev->type = as_device_pcm;
			dev->phyType = as_pstn_device;
			dev->state = AS_DEVICE_STATE_ONHOOK;
			
			AS_MSG_LOG(AS_LOG_INFO, "Device '%s_PSTN_%d' is in the state of ONHOOK", name, i);
			
			count ++;

		}

	}

	AS_MSG_DEBUG(AS_LOG_INFO, "Device is Assist %s card" ,name );

	assert(count!=0);
	span->count = count;
	sprintf(span->name, "ASSIST");

	slic_init(span);
	return span;
}

