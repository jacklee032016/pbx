/*
 * $Id: AssistDeviceInfo.cxx,v 1.5 2007/09/08 18:21:41 lizhijie Exp $
*/

#include "UaConfiguration.hxx"
#include "AssistDeviceInfo.hxx"
#include "AssistNullDevice.hxx"
#include <sys/time.h>
#include "g711.h"
#include "cpLog.h"

using namespace Vocal;

#if 0 //modified by lijie 2005-06-03
AssistDeviceInfo::	AssistDeviceInfo( AssistNullDevice *agent, as_span_t *_span, int id, Sptr < Fifo < Sptr < SipProxyEvent > > > sessionQ)
	:AssistDevice( agent, _span->devs[id]->name, as_hardware_device, id, sessionQ ),
	myDev(_span->devs[id])
{
	const char *phone = UaConfiguration::instance()->getPhoneNumberByDeviceName(_span->devs[id]->name);
	
	if( phone ==0)
	{
		cpLog(LOG_ALERT, "No Local branch phone number is defined for device %s" , myDeviceName );
		exit(1);
	}

	pbxDtmfMode = as_pbx_dtmf_invalide;

	memset(phonenumber, 0 , AS_PBX_DTMF_BUFFER_LENGTH);
	sprintf( phonenumber, "%s", phone );

	busyFd = open( PBX_TONES_DIR"Tone/busy.tone.a"  , O_RDONLY);
	if ( busyFd == -1 )
	{
		cpLog(LOG_ERR, "can not open test tone file in hardware devicee" );
	}

}


AssistDeviceInfo::~AssistDeviceInfo()
{
}

void AssistDeviceInfo::provideDialToneStart()
{
	cpDebug(LOG_DEBUG, "***  Start dial tone on device  %s ***" , myDeviceName);
//	deviceMutex.lock();
	myDev->span->ops->tone_dial_start(myDev);
//	deviceMutex.unlock();
}

//added by lijie to query device status
void AssistDeviceInfo::provideQueryStatus()
{
	cpDebug(LOG_DEBUG, "***  Start query status on device  %s ***" , myDeviceName);
//	deviceMutex.lock();
	myDev->span->ops->query_status(myDev);
//	deviceMutex.unlock();
}
void AssistDeviceInfo::provideDialToneStop()
{
	cpDebug(LOG_DEBUG, "***  Stop dial tone on %s***" , myDeviceName);
//	deviceMutex.lock();
	myDev->span->ops->tone_dial_stop(myDev);
//	deviceMutex.unlock();
	cpDebug(LOG_DEBUG, "***  Stop dial tone  ***" );
}

void AssistDeviceInfo::provideRingStart()
{
	cpDebug(LOG_DEBUG, "***  Start Ringing on %s ***" , myDeviceName );
//	deviceMutex.lock();
	myDev->span->ops->ring_start( myDev);
//	deviceMutex.unlock();
}



void AssistDeviceInfo::provideRingStop( )
{
	cpDebug(LOG_DEBUG, "***  Stop Ringing on %s ***", myDeviceName);
//	deviceMutex.lock();
	myDev->span->ops->ring_stop(myDev);
//	deviceMutex.unlock();
}

void AssistDeviceInfo::provideLocalRingbackStart()
{
	cpDebug(LOG_DEBUG, "***  Start local ringback on %s ***", myDeviceName);
//	deviceMutex.lock();
	myDev->span->ops->tone_ringback_start(myDev);
//	deviceMutex.unlock();
}

void AssistDeviceInfo::provideLocalRingbackStop( )
{
	cpDebug(LOG_DEBUG, "***  Stop local ringback on %s ***" ,myDeviceName);
//	deviceMutex.lock();
	myDev->span->ops->tone_ringback_stop(myDev);
//	deviceMutex.unlock();
}

void AssistDeviceInfo::provideBusyToneStart( )
{
	cpDebug(LOG_DEBUG, "***  Start busy tone on %s ***" , myDeviceName);
//	deviceMutex.lock();
	myDev->span->ops->tone_busy_start(myDev );
//	status = pbx_hanging;
//	deviceMutex.unlock();
}

void AssistDeviceInfo::provideBusyToneStop()
{
	cpDebug(LOG_DEBUG, "***  Stop busy tone on %s ***" , myDeviceName);
//	deviceMutex.lock();
	myDev->span->ops->tone_busy_stop(myDev );
//	status = pbx_unused;
//	deviceMutex.unlock();
}

void AssistDeviceInfo::provideFastBusyToneStart( )
{
	cpDebug(LOG_DEBUG, "***  Start fast busy tone on %s ***" , myDeviceName);
//	deviceMutex.lock();
	myDev->span->ops->tone_fastbusy_start(myDev );
//	status = pbx_hanging;
//	deviceMutex.unlock();
}

void AssistDeviceInfo::provideFastBusyToneStop( )
{
	cpDebug(LOG_DEBUG, "***  Stop fast busy tone on %s ***" , myDeviceName);
//	deviceMutex.lock();
	myDev->span->ops->tone_fastbusy_stop(myDev);
//	status = pbx_unused;
//	deviceMutex.unlock();
}

void AssistDeviceInfo::provideCallWaitingBeepStart()
{
	cpDebug(LOG_DEBUG, "***  Start call waiting beep on %s***", myDeviceName);
//	deviceMutex.lock();
	myDev->span->ops->tone_callwait_start(myDev );
//	deviceMutex.unlock();
}

void AssistDeviceInfo::provideCallWaitingBeepStop()
{
	cpDebug(LOG_DEBUG, "***  Stop call waiting beep on %s***" , myDeviceName);

//	deviceMutex.lock();
	myDev->span->ops->tone_callwait_stop(myDev);
//	deviceMutex.unlock();
}

void AssistDeviceInfo::ProcessRTP()
{
	//deviceMutex.lock();
	if (audioStack == 0)
	{
//		vusleep(10000);
//		deviceMutex.unlock();
		return ;
	}

	RtpSessionState sessionState = audioStack->getSessionState();

	
#if 1
	//only process receive if receive is enabled
	if ( sessionState == rtp_session_recvonly ||
		sessionState == rtp_session_sendrecv )
	{
		inRtpPkt = audioStack->receive ();
		if (inRtpPkt)
		{
			int length = inRtpPkt->getPayloadUsage();
			if ( length != RESID_RTP_RATE)
				cpDebug(LOG_DEBUG, "RtpStack API size doesn't match RESID_RTP_RATE");

			char *vBuf;
			vBuf = inRtpPkt->getPayloadLoc();
#if 0
			short linebuf[RESID_RTP_RATE];
			//);
//			int linebuf[RESID_RTP_RATE];
			int i;
			for(i=0;i<length; i++)
			{
				linebuf[i] = as_lib_law_full_alaw2linear(vBuf[i] );
				vBuf[i] = as_lib_law_linear2ulaw(linebuf[i] );
//				linebuf[i] = alaw2linear(vBuf[i] );
//				vBuf[i] = linear2ulaw(linebuf[i] );
			}
#endif
			write (myDev->fd, vBuf, RESID_RTP_RATE);

			delete inRtpPkt; 
			inRtpPkt = NULL;
		}
	}
#endif



#if 1
	if ( sessionState == rtp_session_sendonly ||
		sessionState == rtp_session_sendrecv )
	{
		int cc;
#if 0		
		if ( isSendRingback )
		{
			cc = getRingbackTone( outBufferPkt, RESID_RTP_RATE);
		}
		else
#endif			
		char buf[RESID_RTP_RATE];
		memset(buf, 0XFF, RESID_RTP_RATE);


//		cc = read(myDev->fd, buf , RESID_RTP_RATE);
		cc = read(myDev->fd, outBufferPkt, RESID_RTP_RATE);
//		cc = getSendBackTone( outBufferPkt, RESID_RTP_RATE);

		if(cc != RESID_RTP_RATE)
			cpDebug(LOG_DEBUG, "read length is %d, required is %d", cc , RESID_RTP_RATE);

#if 0
		short line2[RESID_RTP_RATE];
		memset(line2, 0XFF, RESID_RTP_RATE*sizeof(short) );
//		int line2[RESID_RTP_RATE];
		int i;
		for(i=0;i<cc; i++)
		{
			line2[i] = as_lib_law_full_ulaw2linear(outBufferPkt[i] );
			outBufferPkt[i] = as_lib_law_linear2alaw(line2[i] );
//			line2[i] = as_lib_law_full_ulaw2linear(buf[i] );
//			buf[i] = as_lib_law_linear2alaw(line2[i] );
//			line2[i] = ulaw2linear(outBufferPkt[i] );
//			outBufferPkt[i] = linear2alaw(line2[i] );
		}
#endif


//		if (audioStack)
		{
			cc = as_tran->transmitRaw (outBufferPkt, cc);
//			cc = audioStack->transmitRaw (outBufferPkt, cc);
//			cc = audioStack->transmitRaw (buf, cc);
		}
	}
#endif	

#if 0
	int cc;
		cc=write (myDev->fd, outBufferPkt, cc);
		if(cc != RESID_RTP_RATE)
			cpDebug(LOG_DEBUG, "read length is %d, required is %d", cc , RESID_RTP_RATE);
		cc = read(myDev->fd, outBufferPkt, RESID_RTP_RATE);
		if(cc != RESID_RTP_RATE)
			cpDebug(LOG_DEBUG, "write length is %d, required is %d", cc , RESID_RTP_RATE);
#endif

	if (audioStack &&
		sessionState != rtp_session_inactive &&
		sessionState != rtp_session_undefined )
	{
		audioStack->processRTCP();
	}
//	deviceMutex.unlock();
}

unsigned char  AssistDeviceInfo::reportDtmfDigit( )
{
	return ( myDev->span->ops->report_dtmf_digit)(myDev);
}

void AssistDeviceInfo::setPhoneNumber(char *phone)
{	
	memset(phonenumber, 0 , AS_PBX_DTMF_BUFFER_LENGTH);
	sprintf( phonenumber, "%s", phone );
}

bool AssistDeviceInfo::emitDtmfSignal(char signal)
{
	cpDebug(LOG_DEBUG, "Just discard the DTMF signal from RTP on hardware device");
	return false;
}

int AssistDeviceInfo::openDeviceFile()
{
	char devName[AS_DEVICE_NAME_LENGTH];

	if(UaConfiguration::instance()->getAppType() == assist_app_pcm )
	{
	//	sprintf(devName, "%s/%d", AS_DEVICE_NAME, myDev->phyId +1);
		sprintf(devName, "%s/%d", AS_DEVICE_NAME,  (myDev->span->ops->getPcmChannelId)(myDev)+1 );
		cpDebug(LOG_DEBUG, "Open device %s ",devName );
		myFd = open( devName, O_RDWR );
		if(myFd<=0)
		{
			cpDebug(LOG_DEBUG, "Open device %s failed, because of %s",devName, strerror(errno) );
		}
		assert( myFd > 0 );
		myDev->fd = myFd;
	}	

//	(dsp->ops->gain_setup)(dsp, myDev->fd );
	return 0;
};
		
void AssistDeviceInfo::closeDeviceFile()
{

	if(UaConfiguration::instance()->getAppType() == assist_app_pcm )
	{
		close(myFd);
		myFd = AS_DEVICE_NOT_DEFINED;
		myDev->fd = AS_DEVICE_NOT_DEFINED ;
	}
}



int AssistDeviceInfo::getSendBackTone(char *ringData, int size)
{
	int fd = -1;
	struct timeval start;      // ctam
	struct timeval finish;     // ctam
	gettimeofday (&start, 0);    // ctam

	int cc = 0;
	char tone[RESID_RTP_RATE];

	fd = busyFd;

	if(fd != -1)
	{
		cc = read(fd, tone, RESID_RTP_RATE);
		while ( cc < size )
		{
			lseek(fd, 0, SEEK_SET);
			cc += read( fd, tone + cc, size - cc);
		}
	}
	else
		memset(tone, 0 , RESID_RTP_RATE);
	
	memcpy(ringData, tone, size);

#if 1
	long diff = 0;
	while ( diff < 19500 )
	{
		if ( diff < 20000 ) 
			vusleep(1000);
		gettimeofday (&finish, 0);   // ctam
		diff = finish.tv_usec - start.tv_usec + 1000000 * (finish.tv_sec - start.tv_sec);
	}
#endif

	return cc;
}
#else
AssistDeviceInfo::AssistDeviceInfo( AssistNullDevice *agent, as_device_t *device, Sptr < Fifo < Sptr < SipProxyEvent > > > sessionQ)
	:AssistDevice( agent, device->name, device->phyType, device->id, sessionQ ),
	myDev(device),
	pstnUrl("")
{

	const char *phone = UaConfiguration::instance()->getPhoneNumberByDeviceName(device->name);
	
	if( phone ==0)
	{
		cpLog(LOG_ALERT, "No Local branch phone number is defined for device %s" , myDeviceName );
		exit(1);
	}

	memset(phonenumber, 0 , AS_PBX_DTMF_BUFFER_LENGTH);
	sprintf( phonenumber, "%s", phone );

}


AssistDeviceInfo::~AssistDeviceInfo()
{
}

void AssistDeviceInfo::provideDialToneStart()
{
	cpDebug(LOG_DEBUG, "***  Start dial tone on device  %s ***" , myDeviceName);
	myDev->span->ops->tone_dial_start(myDev);
}

void AssistDeviceInfo::provideQueryStatus()
{
	cpDebug(LOG_DEBUG, "***  Start query status on device  %s ***" , myDeviceName);
	myDev->span->ops->query_status(myDev);
	/* added by lizhijie, 2007.09.02 */
	snprintf(myDev->callerId, MAX_CALLERID_LENGTH, "%s", callerNumber.c_str() );
	cpDebug(LOG_DEBUG, "CallerId  %s : %s ", myDev->callerId, callerNumber.c_str());
}
void AssistDeviceInfo::provideDialToneStop()
{
	cpDebug(LOG_DEBUG, "***  Stop dial tone on %s***" , myDeviceName);
	myDev->span->ops->tone_dial_stop(myDev);
	cpDebug(LOG_DEBUG, "***  Stop dial tone  ***" );
}

void AssistDeviceInfo::provideRingStart()
{
	

	//static string local =  "000"+UaConfiguration::instance()->getLocalCountry()+UaConfiguration::instance()->getLocalDistrict()+UaConfiguration::instance()->getLocalDevice();
	//static int size = local.size();
	
 	cpDebug(LOG_DEBUG, "***  Start Ringing on %s ***" , myDeviceName );

	if(myDev->phyType == as_hardware_device)
		myDev->span->ops->ring_start( myDev);
	else if(myDev->phyType == as_pstn_device)
	{ 
		//bufTel = pstnUrl.substr(0, pstnUrl.size()-1);
		string	bufTel = pstnUrl;
		string::size_type pos = bufTel.find("#");
		if (pos != string::npos)
			bufTel.erase(pos, 1);
		bufTel = UaConfiguration::instance()->getLocalBranch()+bufTel;
		myDev->span->ops->trans_telnumber(myDev, bufTel.c_str());	
	}
	else
		cpLog(LOG_ERR, "invalid device type");
}

void AssistDeviceInfo::provideRingStop( )
{
	cpDebug(LOG_DEBUG, "***  Stop Ringing on %s ***", myDeviceName);
	myDev->span->ops->ring_stop(myDev);
}

void AssistDeviceInfo::provideLocalRingbackStart()
{
	cpDebug(LOG_DEBUG, "***  Start local ringback on %s ***", myDeviceName);
	myDev->span->ops->tone_ringback_start(myDev);
}

void AssistDeviceInfo::provideLocalRingbackStop( )
{
	cpDebug(LOG_DEBUG, "***  Stop local ringback on %s ***" ,myDeviceName);
	myDev->span->ops->tone_ringback_stop(myDev);
}

void AssistDeviceInfo::provideRemoteRingbackStart()
{
	cpDebug(LOG_DEBUG, "***  Start local ringback on %s ***", myDeviceName);
	actionMutex.lock();
	lseek(ringbackFd, 0, SEEK_SET);
	actionType = StartRingBack;
	gettimeofday (&prevTime, 0);
	actionMutex.unlock();
}

void AssistDeviceInfo::provideRemoteRingbackStop( )
{
	cpDebug(LOG_DEBUG, "***  Stop local ringback on %s ***" ,myDeviceName);
	myDev->span->ops->tone_ringback_stop(myDev);
	actionMutex.lock();
	lseek(ringbackFd, 0, SEEK_SET);
	actionType = StopRingBack;
	gettimeofday (&prevTime, 0);
	actionMutex.unlock();
}
void AssistDeviceInfo::provideBusyToneStart( )
{
	cpDebug(LOG_DEBUG, "***  Start busy tone on %s ***" , myDeviceName);
	myDev->span->ops->tone_busy_start(myDev );
}

void AssistDeviceInfo::provideBusyToneStop()
{
	cpDebug(LOG_DEBUG, "***  Stop busy tone on %s ***" , myDeviceName);
	myDev->span->ops->tone_busy_stop(myDev );
}

void AssistDeviceInfo::provideFastBusyToneStart( )
{
	cpDebug(LOG_DEBUG, "***  Start fast busy tone on %s ***" , myDeviceName);
	myDev->span->ops->tone_fastbusy_start(myDev );
}

void AssistDeviceInfo::provideFastBusyToneStop( )
{
	cpDebug(LOG_DEBUG, "***  Stop fast busy tone on %s ***" , myDeviceName);
	myDev->span->ops->tone_fastbusy_stop(myDev);
}

void AssistDeviceInfo::provideCallWaitingBeepStart()
{
	cpDebug(LOG_DEBUG, "***  Start call waiting beep on %s***", myDeviceName);
	myDev->span->ops->tone_callwait_start(myDev);
}

void AssistDeviceInfo::provideCallWaitingBeepStop()
{
	cpDebug(LOG_DEBUG, "***  Stop call waiting beep on %s***" , myDeviceName);
	myDev->span->ops->tone_callwait_stop(myDev);
}

void AssistDeviceInfo::ProcessRTP()
{

	bool bWrong = false;
	struct timeval nowTime;
	
	audioMutex.lock();
	if (audioStack == 0)
	{
		audioMutex.unlock();
		return ;
	}


	gettimeofday (&nowTime, 0);
	
	RtpSessionState sessionState = audioStack->getSessionState();

	
	//only process receive if receive is enabled
	if ( sessionState == rtp_session_recvonly ||
		sessionState == rtp_session_sendrecv )
	{
		inRtpPkt = audioStack->receive ();

		if (inRtpPkt)
		{
			
			int length = inRtpPkt->getPayloadUsage();

			if ( length != RESID_RTP_RATE)
				cpDebug(LOG_DEBUG, "RtpStack API size doesn't match RESID_RTP_RATE");

			if(length<=0)
				bWrong = true;	
			
			char *vBuf;

			vBuf = inRtpPkt->getPayloadLoc();
			write (myDev->fd, vBuf, RESID_RTP_RATE);

			delete inRtpPkt; 
			inRtpPkt = NULL;
		}
		else
		{
			bWrong = true;
		}
		
		if(bWrong)
		{
			if((nowTime.tv_sec-prevTime.tv_sec)>(UaConfiguration::instance()->getMaxDropTime()+1))
			{
				cpDebug(LOG_DEBUG,"Offline detected");
				agent->reportEvent(DeviceEventDropTalk, myDeviceId);
				gettimeofday (&prevTime, 0);
			}	
		}
		else
			gettimeofday (&prevTime, 0);
	}




	if ( sessionState == rtp_session_sendonly ||
		sessionState == rtp_session_sendrecv )
	{
		int cc;
		
		char buf[RESID_RTP_RATE];
		memset(buf, 0XFF, RESID_RTP_RATE);

		cc = getSendBackTone(outBufferPkt, RESID_RTP_RATE);
		
		if(cc == -1) 
		{
			cc = read(myDev->fd, outBufferPkt, RESID_RTP_RATE);
		}

		if(cc != RESID_RTP_RATE)
			cpDebug(LOG_DEBUG, "read length is %d, required is %d", cc , RESID_RTP_RATE);

		if(cc>0)
			cc = as_tran->transmitRaw (outBufferPkt, cc);

	}

	if (audioStack &&
		sessionState != rtp_session_inactive &&
		sessionState != rtp_session_undefined )
	{
		int ret;
		ret=audioStack->processRTCP();
		if(!ret)
		{
			gettimeofday (&prevTime, 0);
		}
	}
	
	audioMutex.unlock();
	
}

unsigned char  AssistDeviceInfo::reportDtmfDigit( )
{
	return ( myDev->span->ops->report_dtmf_digit)(myDev);
}

bool AssistDeviceInfo::emitDtmfSignal(char signal)
{
	cpDebug(LOG_DEBUG, "Just discard the DTMF signal from RTP on hardware device");
	return false;
}

int AssistDeviceInfo::openDeviceFile()
{
//	char devName[AS_DEVICE_NAME_LENGTH];


	//get the time of begin audio
	gettimeofday (&prevTime, 0);

	if(UaConfiguration::instance()->getAppType() == assist_app_pcm )
	{
#if 0	
		sprintf(devName, "%s/%d", AS_DEVICE_NAME,  (myDev->span->ops->getPcmChannelId)(myDev));
		cpDebug(LOG_DEBUG, "Open device %s ",devName );
		myFd = open( devName, O_RDWR );
		if(myFd<=0)
		{
			cpDebug(LOG_DEBUG, "Open device %s failed, because of %s",devName, strerror(errno) );
		}
		assert( myFd > 0 );
#endif
		myFd = myDev->span->ops->getPcmChannelId(myDev);
		myDev->fd = myFd;
	}	

#if 1
	if(dsp && dsp->rxGain!= 1 && dsp->txGain!= 1 )
	{
		(dsp->ops->gain_setup)(dsp, myDev->fd );
	}
#endif

	return 0;
};
		
void AssistDeviceInfo::closeDeviceFile()
{

#if 0
	if(UaConfiguration::instance()->getAppType() == assist_app_pcm )
	{
		close(myFd);
		myFd = AS_DEVICE_NOT_DEFINED;
		myDev->fd = AS_DEVICE_NOT_DEFINED ;
	}
#endif	
}
#endif

