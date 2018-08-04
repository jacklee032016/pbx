/*
 * $Id: PstnTaskEngine.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */

#include "PstnEndPoint.hxx"

#include "assist_lib.h"
#include "pstn.hxx"
#include "PbxConfiguration.hxx"

using namespace Assist;

typedef  void *(*_task_start_routing)(void *);

void as_pstn_report_event(as_device_t *dev, DeviceEventType signal);

/* before send out DTMF, detect DT(Dial Tone) when call out */
void *fxoDetectDT(void *param)
{
	as_device_t 		*taskDev = (as_device_t *)param;
	char 			buf[AS_DRIVER_BUFFER_LENGTH];
	int				len, res;
	DeviceEventType 	result = DeviceEventUndefined;

	assert(taskDev!=0);
	taskDev->timeout = 0;
	
	cpDebug(LOG_DEBUG, "detecting DT(Dial Tone) on %s\n\n", taskDev->name);
	while(1)
	{
		len = read( taskDev->fd, buf, AS_DRIVER_BUFFER_LENGTH);
		res = as_dsp_progress_detect(taskDev->dsp, (unsigned char *)buf, len);
		taskDev->timeout++;
		
		if(res == AS_PROGRESS_DIALTONE )
//		if(res != AS_PROGRESS_BUSYTONE && res !=AS_PROGRESS_UNKNOWN )
		{
			result = DeviceEventHookUp;
			cpDebug(LOG_DEBUG, "DT(Dial Tone[%d]) is detected on %s\n\n", res, taskDev->name);
			break;
		}
		else if(res != AS_PROGRESS_UNKNOWN )
		{
			/* send DeviceEventBusy to PstnEndPoint which is state of RING.
			* then OpDeviceBusy is called and EP return to IDLE state
			*/
			cpDebug(LOG_ERR, "PROGRESS %d detect when FXO dialing\n", res);
			/* no DT is detected when call out from FXO */
			result = DeviceEventBusy;
//			assert(0);
			break;
		}

		if(taskDev->timeout > 250)// NTT_FSK_DURATION_3_SECONDS)
		{
			result = DeviceEventBusy;
			break;
		}
	}
#if 0
	cpDebug(LOG_DEBUG, "detecting DT(Dial Tone) on %s\n\n", taskDev->name);
	if(result == DeviceEventBusy)
	{
		taskDev->callingBuf[0] = 0;
//		(taskDev->ops->tone_dial_stop)(taskDev);
	}
#else
	if( result == DeviceEventHookUp)
	{
		FxoDialMode dialMode = PbxConfiguration::instance()->getFxoDialMode();

		cpDebug(LOG_DEBUG, "%s dialing with %s\n\n", taskDev->name, taskDev->callingBuf );
		switch(dialMode)
		{
			case FXO_DIAL_DP10:
			{
				cpDebug(LOG_DEBUG, "FXO DP 10PPS\n\n" );
				assist_dsp_fxo_send_pps(taskDev->fd, taskDev->callingBuf ,AS_DP_10PPS);
				break;
			}
			case FXO_DIAL_DP20:
			{
				cpDebug(LOG_DEBUG, "FXO DP 20PPS\n\n" );
				assist_dsp_fxo_send_pps(taskDev->fd, taskDev->callingBuf ,AS_DP_10PPS);
				break;
			}	
			default:
			{
				cpDebug(LOG_DEBUG, "DTMF dial '%s'\n\n" ,taskDev->callingBuf);
				as_dsp_play_dtmf( taskDev->dsp, taskDev->fd, taskDev->callingBuf );
				break;
			}	
		}
	}
	taskDev->callingBuf[0] = 0;

#endif
	
	taskDev->taskType = PSTN_TASK_UNINIT;
	as_pstn_report_event( taskDev, result );

	cpDebug(LOG_DEBUG, "DT(Dial Tone) Detect thread ended!" );

	return 0;
}

/* detect FSK callerID and DID when call come from FXO */
void *fxoDetectFSK(void *param)
{
	as_device_t 		*taskDev = (as_device_t *)param;
	char 			buf[AS_DRIVER_BUFFER_LENGTH];
	int				res, length = 0;
	unsigned char 	fskdata[10000];
	DeviceEventType 	result = DeviceEventUndefined;
	pstn_param_t 		*fskParam = NULL;
//	int i=0;

	assert(taskDev!=0);
	taskDev->timeout = 0;

	while(1)
	{
		res = read( taskDev->fd, buf, AS_DRIVER_BUFFER_LENGTH);
		taskDev->timeout ++;

		if( res != AS_DRIVER_BUFFER_LENGTH)
			break;
		
		if( res >0 &&  as_dsp_fsk_isVAD(taskDev->dsp, (unsigned char *)buf, res)  )
		{
			memcpy(	fskdata+length, buf, res);
			length += res;
			as_error_msg("FSK ");
		}
		
		if(taskDev->timeout >= NTT_FSK_DURATION_3_SECONDS)
		{/* timeout */
			cpDebug(LOG_DEBUG, "FSK demodulation failed because of timeout" );
			result = DeviceEventIncomingEnd;
			break;
		}
		if( ( length+AS_DRIVER_BUFFER_LENGTH) > (int) sizeof(fskdata) )
		{
			cpLog(LOG_ERR, "FSK buffer is to small(%d) for demodulation" ,sizeof(fskdata) );
			assert(0);
		}
		
	}

	fskParam = as_dsp_fsk_decode(taskDev->dsp,fskdata, length );
	if(fskParam != NULL)
	{
		cpDebug(LOG_DEBUG, "FSK modem demodulation has ended!");
		PstnEndPoint *ep = static_cast<PstnEndPoint* >(taskDev->private_data);
		assert(ep != NULL);
		if( (fskParam->result & AS_CALL_PARAM_CALLERID ))
		{/* caller ID for phone number display */
			ep->setPeerNumber( (const char *)fskParam->caller);
		}	
		if( (fskParam->result & AS_CALL_PARAM_CALLED ) )
		{/* called phone number for DID */
			sprintf( (char *)taskDev->calledPhoneNumber, "%s", (char *)fskParam->called);
		}

		result = DeviceEventIncomingEnd ;
	}
	
	taskDev->taskType = PSTN_TASK_UNINIT;
	as_pstn_report_event( taskDev, result );

	cpDebug(LOG_DEBUG, "FSK Detect thread ended!" );
	
	return 0;
}

void *fxoDetectBT(void *param)
{
	as_device_t 		*taskDev = (as_device_t *)param;
	int				res;

	assert(taskDev!=0);

	while(1)
	{
		if(taskDev->btStage == BT_DETECT_STAGE_CHECK)
		{
			res = as_dsp_progress_detect(taskDev->dsp, (unsigned char *)taskDev->busyData, taskDev->btBufCount*AS_DRIVER_BUFFER_LENGTH);
			
			if(res == AS_PROGRESS_BUSYTONE)
			{
				cpDebug(LOG_DEBUG, "BT(Busy Tone[%d]) is detected on %s\n\n", res, taskDev->name);
				as_pstn_report_event( taskDev, DeviceEventHookDown );
			}
			else if(res != AS_PROGRESS_UNKNOWN )
			{
				/* send DeviceEventBusy to PstnEndPoint which is state of RING.
				* then OpDeviceBusy is called and EP return to IDLE state
				*/
				cpDebug(LOG_ERR, "PROGRESS %d detect when FXO dialing\n", res);
				/* no DT is detected when call out from FXO */
			}

			taskDev->btStage = BT_DETECT_STAGE_INPUT;
			break;
		}	
	}

	return 0;
}

/* encode FSK callerID and DID when a call to FXS */
void *fxsEncodeFSK(void *param)
{
	as_device_t 		*taskDev = (as_device_t *)param;
//	char 			buf[160];
//	int				len;
//	DeviceEventType 	result = DeviceEventUndefined;
//	pstn_param_t 		*fskParam = NULL;

	assert(taskDev!=0);
	taskDev->timeout = 0;

	taskDev->taskType = PSTN_TASK_UNINIT;
//	as_pstn_report_event( taskDev, result );

	cpDebug(LOG_DEBUG, "FSK Detect thread ended!" );
	
	return 0;
}

/* encode and play FSK callerID and DID when a call to FXS */
void *fxsPlayFSK(void *param)
{
	as_device_t 		*taskDev = (as_device_t *)param;
	unsigned char 	*buf;
	int				length;
	int				i, count, res;
//	DeviceEventType 	result = DeviceEventUndefined;

	assert(taskDev!=0);
	taskDev->timeout = 0;
	buf = as_dsp_fsk_encode(taskDev->dsp, (char *)taskDev->calledPhoneNumber, NULL, &length );

	count = length /AS_DRIVER_BUFFER_LENGTH;
	for( i=0; i< count; i++ )
	{
		res = write(taskDev->fd, buf +i*AS_DRIVER_BUFFER_LENGTH, AS_DRIVER_BUFFER_LENGTH);
		if(res != AS_DRIVER_BUFFER_LENGTH)
		{
			printf("file write failure, length is %d\n", res);
		}
	}
	res = length - count*AS_DRIVER_BUFFER_LENGTH;
	if(res >0)
	{
		res = write(taskDev->fd, buf+i*AS_DRIVER_BUFFER_LENGTH, res);
	}	

	taskDev->taskType = PSTN_TASK_UNINIT;
//	as_pstn_report_event( taskDev, result );

	cpDebug(LOG_DEBUG, "FSK Play thread ended!" );
	
	return 0;
}

