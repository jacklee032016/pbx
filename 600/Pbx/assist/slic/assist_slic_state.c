/*$Id: assist_slic_state.c,v 1.3 2007/03/21 20:39:56 lizhijie Exp $*/
#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include "assist_globals.h"
#include "pbx_globals.h"
#include "asstel.h"

static PSTN_INFO pstn[AS_DEVICE_MAX];
static int isQuery[AS_DEVICE_MAX];


static pthread_mutex_t mutSlic = PTHREAD_MUTEX_INITIALIZER;

/* device must be in ON_HOOK for ring */
int as_ring_start( int  fd, const char *devicename )
{
	int x;
	int res =0;
		
	if( fd <0)
	{
		AS_MSG_LOG(AS_LOG_ERR, "Uninit the dev device in %s" , devicename );
		return -1;
	}	
#if 0
	x = AS_ONHOOK;
	res = ioctl( fd,  AS_CTL_HOOK, &x);
#endif

	x = AS_START;
	res = ioctl(fd,  AS_CTL_HOOK, &x);
#if AS_DEBUG
	if (res) 
	{
		switch(errno) 
		{
			case EBUSY:
			case EINTR:
				AS_MSG_DEBUG(AS_LOG_INFO,  "ring the phone BUSY in device '%s'" , devicename);
				usleep(10000);
				break;
			case EINPROGRESS:
				AS_MSG_DEBUG(AS_LOG_INFO, "ring the phone IN-PROGRESS in device '%s'", devicename );
				res = 0;
				break;
			default:
				AS_MSG_DEBUG(AS_LOG_INFO, "Couldn't ring the phone in device '%s': %s", devicename, strerror(errno));
				break;
		}
	}
#endif	
	return res;
}

/* device must be in ON_HOOK for ring */
int as_ring_stop( int  fd, const char *devicename )
{
	int x;
	int res =0;
		
	if( fd <0)
	{
		AS_MSG_LOG(AS_LOG_ERR, "Uninit the dev device in %s\n" , devicename );
		return -1;
	}	
#if 0
	x = AS_ONHOOK;
	res = ioctl( fd,  AS_CTL_HOOK, &x);
#endif

	x = AS_RINGOFF;
	res = ioctl(fd,  AS_CTL_HOOK, &x);
#if AS_DEBUG
	if (res) 
	{
		switch(errno) 
		{
			case EBUSY:
			case EINTR:
				/* Wait just in case */
				AS_MSG_DEBUG(AS_LOG_INFO, "ringoff the phone BUSY in device '%s'" , devicename);
				usleep(10000);
				break;
			case EINPROGRESS:
				AS_MSG_DEBUG(AS_LOG_INFO, "ringoff the phone IN-PROGRESS in device '%s'", devicename );
				res = 0;
				break;
			default:
				AS_MSG_DEBUG(AS_LOG_INFO, "Couldn't ringoff the phone in device '%s': %s", devicename, strerror(errno));
				break;
		}
	}
#endif	
	return res;
}

int as_dev_ring_start(as_device_t *dev)
{
	return as_ring_start(dev->fd, dev->name);
}

int as_dev_ring_stop(as_device_t *dev)
{
	return as_ring_stop(dev->fd, dev->name);
}


/* this function is only used when no ringing, for example, when our program is startup */
as_state_t  as_device_check_state_on_startup(int fd, const char *name)
{
	int res;
	struct wcfxs_stats stats;
	
	res = ioctl( fd, WCFXS_GET_STATS, &stats);
	if (res) 
	{
		AS_MSG_LOG(AS_LOG_ERR, "Unable to get status on channel '%s'\n", name );
		exit(1);
	}
#if 0	
		printf("TIP: %7.4f Volts\n", (float)stats.tipvolt / 1000.0);
		printf("RING: %7.4f Volts\n", (float)stats.ringvolt / 1000.0);
		printf("VBAT: %7.4f Volts\n", (float)stats.batvolt / 1000.0);
		printf("Value of Register is %d\r\n", stats.ringvolt);
#endif
	if( stats.ringvolt < (-40000) )
		return AS_DEVICE_STATE_ONHOOK;
	else
		return AS_DEVICE_STATE_OFFHOOK;
}

void slic_init(as_span_t  *span)
{
	int i;
	as_device_t *dev;
	
	memset(isQuery, 0x00, sizeof(isQuery));
	memset(pstn, 0x00, sizeof(pstn));

	for(i = 0; i<AS_DEVICE_MAX; i++)
	{
		pstn[i].pstnState = AS_DEVICE_STATE_INVALIDATE;
	}

	for(i = 0; i<span->count; i++)
	{
		dev = span->devs[i];
		pstn[dev->phyId].dev = dev;
	}
	
	as_dial_init(100);
}

int as_device_query_status(as_device_t *dev )
{
	pthread_mutex_lock(&mutSlic);
	isQuery[dev->phyId] = 1;
	pthread_mutex_unlock(&mutSlic);
	return 0;
}


as_state_t as_device_get_status(as_device_t *dev )
{
	as_state_t state;

	pthread_mutex_lock(&mutSlic);

	if(!isQuery[dev->phyId])
	{
		pthread_mutex_unlock(&mutSlic);
		state = AS_DEVICE_STATE_INVALIDATE;
	}	
	else
	{
		isQuery[dev->phyId] = 0;
		pthread_mutex_unlock(&mutSlic);
		if(dev->phyType == as_pstn_device)
			state = AS_DEVICE_STATE_ONHOOK;
		else
			state = dev->state;
		
		AS_MSG_DEBUG(AS_LOG_INFO, "Check status for device %d  is %d",dev->phyId, state); 
		if(state ==  AS_DEVICE_STATE_ONHOOK)
			state = AS_DEVICE_STATE_IDLE;
		else if(state ==  AS_DEVICE_STATE_OFFHOOK)
			state = AS_DEVICE_STATE_BUSY;
		else 
			state = AS_DEVICE_STATE_INVALIDATE;
	}
	return state;
	
}

as_state_t as_device_check_status(as_device_t *dev )
{
	int res;

	res = as_lib_event_get( dev->fd);
	//added by lijie to report pstn offhook event
	pthread_mutex_lock(&mutSlic);
	if(res == AS_DEVICE_STATE_INVALIDATE)
		res = pstn[dev->phyId].pstnState;
	
	pthread_mutex_unlock(&mutSlic);
	if(res == AS_EVENT_RINGOFFHOOK)
	{
		AS_MSG_DEBUG(AS_LOG_INFO, "\r\n************OFFHOOK event checked on device %s" , dev->name );
		return AS_DEVICE_STATE_OFFHOOK;
	}
	else if( res ==AS_EVENT_WINKFLASH)
	{
		AS_MSG_DEBUG(AS_LOG_INFO, "\r\n************WINFLASH event checked on device %s" , dev->name );
		return AS_DEVICE_STATE_WINKFALSH;
	}
	else if( res ==AS_EVENT_ONHOOK) 
	{
		AS_MSG_DEBUG(AS_LOG_INFO, "\r\n************ONHOOK event checked on device %s" , dev->name );
		return AS_DEVICE_STATE_ONHOOK;
	}	

	AS_MSG_LOG(AS_LOG_WARNING, "check device %s status failed : %s!\n", dev->name, strerror(errno) );
	return AS_DEVICE_STATE_INVALIDATE;
}

unsigned char  as_dtmf_get_digit(as_device_t *dev)
{
	int signal;
	if (ioctl(dev->fd, AS_CTL_GET_DTMF_DETECT, &signal) == -1) 
	{
		AS_MSG_LOG(AS_LOG_ERR, "IOCTL error in device '%s': %s \r\n", dev->name, strerror(errno) );
		return 'X';
	}
#if 0//AS_EVENT_DEBUG			
	printf("DTMF signal '%c' is detect on device '%s'\r\n", (unsigned char)signal, devicename);
#endif
	return (unsigned char)signal;
}

int as_device_trans_telnumber(as_device_t *dev, const char *bufTelNumber)
{
	pthread_mutex_lock(&mutSlic);	
	pstn[dev->phyId].iEnablePstn = 1;
	pstn[dev->phyId].dev = dev;
	strcpy(pstn[dev->phyId].pstnBuf, bufTelNumber);
	pthread_mutex_unlock(&mutSlic);	

	return 1;
}
int as_get_channel_state(struct as_channel_state *channel_states)
{
	int i=0;
	int fd;
	int res;
	int chan_num;
	
	struct as_channel_state chan_state;

	fd=open("/dev/astel/0",O_RDWR);

	res=ioctl(fd,AS_GET_CHANNEL_NUMBER,&chan_num);

	assert(chan_num<=AS_DEVICE_MAX);

	i = 0;
	while(i<chan_num)		
	{	
		chan_state.channel_no=i;
		res=ioctl(fd,AS_GET_CHANNELS_STATES,&chan_state);
		if(res<0)
		{
			channel_states[i].type=chan_state.type;
			channel_states[i].channel_no=i;
			channel_states[i].available=-1;
		}
		else
		{
			channel_states[i].type=chan_state.type;
			channel_states[i].channel_no=chan_state.channel_no;
			channel_states[i].available=chan_state.available;
		}
		i++;
	}

	if(fd>0)
		close(fd);

	return res;
}


void *slic_Thread(void *p)
{
	int i;
	

	while(1)
	{
		i = 0;
		for(; i<AS_DEVICE_MAX; i++)
		{
			pthread_mutex_lock(&mutSlic);	
			if(pstn[i].iEnablePstn && pstn[i].dev)
			{
				as_lib_offhook(pstn[i].dev->fd);
				as_lib_dial(pstn[i].dev->fd,  pstn[i].pstnBuf, 10000);
				pstn[i].pstnState = AS_DEVICE_STATE_OFFHOOK;
				pstn[i].iEnablePstn = 0;
			}
			pthread_mutex_unlock(&mutSlic);			
		}

	}
}

