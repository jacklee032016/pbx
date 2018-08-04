/*
 * $Log: assist_pstn_state.c,v $
 * Revision 1.1.1.1  2006/11/30 16:22:45  lizhijie
 * AS800 PstnAPI
 *
 * Revision 1.7  2006/05/30 03:04:27  lizhijie
 * optimzed DTMF decoder and add DSP configuration interface
 *
 * Revision 1.6  2006/05/25 03:19:41  lizhijie
 * add pstn_standard
 *
 * Revision 1.5  2006/03/25 11:27:28  lizhijie
 * add FXO line status detect
 *
 * Revision 1.4  2006/03/25 11:26:08  lizhijie
 * no message
 *
 * Revision 1.3  2006/03/20 06:46:37  lizhijie
 * add and debug some. refer to ChangeLog
 *
 * Revision 1.2  2006/03/20 06:34:30  lizhijie
 * no message
 *
 * Revision 1.1.1.1  2006/03/10 09:31:43  lizhijie
 * PSTN APIs library for Gatewat project
 *
 * $Id: assist_pstn_state.c,v 1.1.1.1 2006/11/30 16:22:45 lizhijie Exp $
*/

#define _GNU_SOURCE
#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>

#include "as_fxs.h"
#include "as_dev_ctl.h"
#include "as_ring_param.h"

#include "assist_lib.h"

 /* this function is used to check whether a line connect to FXO port, so it is only used in FXO device */
int  as_device_check_fxo_online(int fd, const char *name)
{
	int res;
	struct wcfxs_stats stats;
	int i;
	int voltage = 0;

	for(i = 0; i <10; i++)
	{
		res = ioctl( fd, WCFXS_GET_STATS, &stats);
		if (res) 
		{
			as_error_msg("Unable to get status on channel '%s'\n", name );
			exit(1);
		}
#if 1
		as_error_msg("voltage is %d\n", abs(stats.batvolt ));
#endif
		voltage += abs(stats.batvolt);
	}
	
	if( (voltage/10) < 1500)  /* mv */
		return 0;
	else
		return 1;
}


 /* this function is only used when no ringing, for example, when our program is startup */
as_state_t  as_device_check_state_on_startup(int fd, const char *name)
{
	int res;
	struct wcfxs_stats stats;
	
	res = ioctl( fd, WCFXS_GET_STATS, &stats);
	if (res) 
	{
		as_error_msg("Unable to get status on channel '%s'\n", name );
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

unsigned char  as_dtmf_get_digit(int fd )
{
	int signal;
	if (ioctl(fd, AS_CTL_GET_DTMF_DETECT, &signal) == -1) 
	{
		as_error_msg("IOCTL error in device : %s \r\n",  strerror(errno) );
		return 'X';
	}
#if 0//AS_EVENT_DEBUG			
	printf("DTMF signal '%c' is detect on device '%s'\r\n", (unsigned char)signal, devicename);
#endif
	return (unsigned char)signal;
}

int as_get_channel_state(struct as_channel_state *channel_states)
{
	int i=0;
	int fd;
	int res;
	int chan_num;
	
	struct as_channel_state chan_state;

	fd=open("/dev/astel/0",O_RDWR);
	if (fd == -1)
	{
		as_error_msg("Can't open device file!\n");
	}

	res=ioctl(fd,AS_GET_CHANNEL_NUMBER,&chan_num);
	if (res == -1)
	{
		as_error_msg("ioctl failure!\n");
		perror("Error Status: ");
	}

#if 0
	as_error_msg("chan_num = -----%d\n", chan_num);
#endif

	assert(chan_num <= AS_DEVICE_MAX);

	i = 0;
	while(i<chan_num)		
	{	
		chan_state.channel_no=i;
		res=ioctl(fd,AS_GET_CHANNELS_STATES,&chan_state);
		if(res<0)
		{
			channel_states[i].type=chan_state.type;
			channel_states[i].channel_no = chan_state.channel_no;
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


int fxo_try_dial(int fd,char *digitstring)
{
	int res =-1;

	printf("PSTN NUMBER: %s\r\n\r\n", digitstring);
	printf("\nafter ring...\n");

	//res = as_lib_onhook(fd);
	//printf("ONHOOK = %d\n",res); 

	int hook = AS_ONHOOK;

	while (res == -1)
	{
		res = ioctl(fd, AS_CTL_HOOK, &hook);
		//printf("OnHook operator\n");
		printf("error in AS_ONHOOK : %s with fd : %d\r\n", strerror(errno), fd);
	}
	printf("AS_ONHOOK OK!\n");

	hook = AS_RING;
	res = -1;
	while (res == -1)
	{
		res = ioctl(fd, AS_CTL_HOOK, &hook);
		//printf("error in AS_RING : %s\r\n", strerror(errno) );
	}

	printf("AS_RING OK!\n");
#if 0
	if(res < 0)
	{		
		printf("error in AS_RING : %s\r\n", strerror(errno) );
		//exit(1);
		return -1;
	}
#endif 

	/*frist ring then delay (350 - 500)ms to wait Abelian group OFF-HOOK*/
	usleep(1000000);     
	
	res = as_lib_offhook(fd);
	//printf("OFFHOOK = %d\n",res);
	
//	res = as_lib_dial(fd,  digitstring, 150);
	//printf("DIAL = %d\n",res);	  
	if(res==-1)	  
	{
	      	printf("\ndial fail\n");
		return -1;
	}
	      
	else
		printf("dial over!!!\n");
	return 0;
}


int as_get_device_num()
{
	int fd,res;
	int dev_num;
	
	fd=open("/dev/astel/0",O_RDWR);
	if (fd<0)
	{
		printf("unable to open the device, Maybe your driver is not load or hardware is not attached:%s\n ",strerror(errno));
		return -1;
	}
	
	res=ioctl(fd,AS_GET_CHANNEL_NUMBER,&dev_num);
	if(res<0)
	{
		printf("error in get the channels' num: %s\r\n", strerror(errno));
		dev_num = -1;
	}

	close(fd);
	
	return dev_num;
}


struct as_channel_state *as_get_dev_state(int dev_number)
{
	int fd;
	int res;
	
	int totalDev;
	
	struct as_channel_state *chan_state;

	fd=open("/dev/astel/0",O_RDWR);
	if (fd == -1)
	{
		as_error_msg("Can't open device file!\n");
		exit(1);
	}

	/* error check*/
	res=ioctl(fd,AS_GET_CHANNEL_NUMBER,&totalDev);
	if (res == -1)
	{
		as_error_msg("ioctl failure!\n");
		perror("Error Status: ");
	}

//	as_error_msg("Total SLIC/DAA Device :-----%d\n", totalDev);
	assert( totalDev <= AS_DEVICE_MAX);
	assert(dev_number <= totalDev); 

	chan_state = (struct as_channel_state *)malloc(sizeof(struct as_channel_state));
	assert(chan_state);
	
	chan_state->channel_no= dev_number;
	res=ioctl(fd, AS_GET_CHANNELS_STATES, chan_state);
	close(fd);

	if(res<0)
	{
		free(chan_state);
		return 0;			
	}

	return chan_state;
}

static int as_device_setup_china(int fd)
{
	int res;
	struct as_ring_cadence chineseCadence ={{2000,4000, 0,0,  0,0,0,0,  0,0,0,0,  0,0,0,0 } };

	res = ioctl(fd,  AS_CTL_SETCADENCE, &chineseCadence);

	return res;
}

static int as_device_setup_japan(int fd)
{
	int res;
	struct as_ring_cadence japaneseCadence ={{1000,2000, 0,0,  0,0,0,0,  0,0,0,0,  0,0,0,0 } };

	res = ioctl(fd,  AS_CTL_SETCADENCE, &japaneseCadence);

	return res;
}

int as_device_standard_config(int fd, pstn_standard_t standard)
{
	if(standard == PSTN_STANDARD_CHINA)
		return as_device_setup_china( fd);
	return as_device_setup_japan(fd);
}

