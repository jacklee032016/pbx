/*$Id: assist_init.c,v 1.1.1.1 2006/11/29 09:50:50 lizhijie Exp $*/
#include "pbx_globals.h"
#include "assist_globals.h"

static as_span_t  master_span =
{
	name		:	"Assist devices span",
	timeout		:	 0
};

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

//	setup_gain			: as_channel_gain_setup

};

as_span_t  *as_check_hardware(const char *name, int law)
{
	int  i;
	int count = 0;
	int fd;
	char devicename[AS_DEVICE_NAME_LENGTH];
	as_span_t	*span;
	as_device_t	*dev;
#if 1
	span = &master_span;
	span->ops = &(slic_ops);
#else
	span = (as_span_t *)malloc(sizeof(as_span_t));
	assert(span!=0);
	memset(span, 0, sizeof(as_span_t));
	span->op = ;
#endif

	for(i=0; i<AS_DEVICE_MAX; i++)
	{
		sprintf(devicename, "%s/%d", name, i+1);
		fd = open( devicename, O_RDWR );
		if( fd <= 0 )
		{
			as_error_msg( "Device '%s' Open failed. Error: '%s'.\r\n",  devicename,  strerror(errno) );
			continue;
		}

		dev = (as_device_t *)malloc(sizeof(as_device_t) );
		assert(dev!=0);
		sprintf(dev->name, "%s", devicename);
		dev->id = count;
		dev->fd = fd;
		count ++;
		span->devs[dev->id] = dev;
		dev->span = span;
#if __WITH_SLIC__
		dev->state = as_device_check_state_on_startup( fd, devicename);
		dev->type = as_device_slic;
		if(dev->state == AS_DEVICE_STATE_ONHOOK )
			as_error_msg("Device '%s' is in the state of ONHOOK\r\n", devicename);
		else
			as_error_msg("Device '%s' is in the state of ONHOOK\r\n", devicename);
#else
		dev->type = as_device_pcm;
#endif
		if(dev->type == as_device_slic)
		{
			if(law !=U_LAW_CODE)
			{
				as_error_msg("SLIC device without G711 U-law audio codec.\r\nPlease check your configuration carefully!");
				exit(1);
			}
		}
		if(dev->type == as_device_pcm)
		{
			if(law !=A_LAW_CODE)
			{
				as_error_msg("PCM device without G711 A-law audio codec.\r\nPlease check your configuration carefully!");
				exit(1);
			}
		}
//		span->ops->setup_gain(dev, rxGain, txGain);
#if __WITH_SLIC__
		as_error_msg( "Device is Assist Codec/SLIC card\r\n" );
#else
		as_error_msg( "Device is Assist PCM card\r\n" );
#endif

	}

	assert(count!=0);
	span->count = count;
	sprintf(span->name, "ASSIST");

	return span;
}

