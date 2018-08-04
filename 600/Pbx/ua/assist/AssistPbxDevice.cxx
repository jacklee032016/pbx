/*
 * $Id: AssistPbxDevice.cxx,v 1.1.1.1 2006/11/29 09:50:50 lizhijie Exp $
*/
#include "UaConfiguration.hxx"
#include "UaTimerEvent.hxx"
#include "UaDevice.hxx"

#include "AssistPbxDevice.hxx"

#include "AssistNullDevice.hxx"

using namespace Vocal;

#if 0 //modified by lijie 2005-06-03
AssistPbxDevice::AssistPbxDevice(AssistNullDevice *agent, char *devicename, int deviceid, Sptr < Fifo < Sptr < SipProxyEvent > > > sessionQ)
	:AssistDevice(agent, devicename, as_virtual_device, deviceid, sessionQ ),
	cwBeep( 0 ),
	cwCadenceOn( true ),
	cwCnt( 0 ),
	cwBeepOn( false ),
	sendRingback(false),
	sendGreeting(false),
	sendbusy(false),
	status(pbx_unused),
	callerNum(0),
	calleeNum(0),
	lastTimeForRtp(0)
{
	/* create RTP thread of it*/
#define AS_TONE_FILE_NAME_LENGTH		64

	as_law_type_t law = UaConfiguration::instance()->getG711Law();

	char tonefile[AS_TONE_FILE_NAME_LENGTH];

	pbxDtmfMode = (as_pbx_dtmf_mode_t )UaConfiguration::instance()->getPbxDtmfMode();

	sprintf(tonefile, "%s%s.%s", PBX_TONES_DIR, "Tone/ringback.tone", (law ==  as_g711_u_law) ?"u":"a"  );
	ringbackFd = open(tonefile , O_RDONLY);
	if ( ringbackFd == -1 )
	{
		cpLog(LOG_ERR, "can not open %s" , tonefile );
	}

	memset(tonefile,0, AS_TONE_FILE_NAME_LENGTH);
	sprintf(tonefile, "%s%s.%s", PBX_TONES_DIR, "Tone/greeting" , (law ==  as_g711_u_law) ?"u":"a"  );
//	sprintf(tonefile, "%s%s", PBX_TONES_DIR, "Tone/monkeys.a" );
	greetingFd = open(tonefile , O_RDONLY);
	if ( greetingFd == -1 )
	{
		cpLog(LOG_ERR, "can not open %s" , tonefile );
	}

	memset(tonefile,0, AS_TONE_FILE_NAME_LENGTH);
	sprintf(tonefile, "%s%s.%s", PBX_TONES_DIR, "Tone/busy.tone" , (law ==  as_g711_u_law) ?"u":"a"  );
	busyFd = open(tonefile , O_RDONLY);
	if ( busyFd == -1 )
	{
		cpLog(LOG_ERR, "can not open %s" , tonefile );
	}

	int err = gettimeofday(&lastTime, NULL);
	assert( !err );

}

AssistPbxDevice::~AssistPbxDevice()
{
	/* close rtp thread of it */
}

void AssistPbxDevice::provideDialToneStart()
{
	cpDebug(LOG_DEBUG, "***  Start dial tone on device  %s ***" , myDeviceName);
//	deviceMutex.lock();
//	deviceMutex.unlock();
}

//added by lijie to query device status
void AssistPbxDevice::provideQueryStatus()
{
	cpDebug(LOG_DEBUG, "***  Start query status on device  %s ***" , myDeviceName);
//	deviceMutex.lock();
//	deviceMutex.unlock();
	agent->reportEvent( DeviceEventIdle, myDeviceId );
}
void AssistPbxDevice::provideDialToneStop()
{
	cpDebug(LOG_DEBUG, "***  Stop dial tone on %s ***" , myDeviceName);
//	deviceMutex.lock();
//	deviceMutex.unlock();
	agent->reportEvent( DeviceEventHookDown, myDeviceId );
	cpDebug(LOG_DEBUG, "***  Stop dial tone  ***" );
}

void AssistPbxDevice::provideRingStart()
{
	cpDebug(LOG_DEBUG, "***  Start Ringing on %s ***" , myDeviceName );
	pbxStartTimer(DeviceSignalRingStart);
	status = pbx_ring;

//	deviceMutex.lock();
//	deviceMutex.unlock();
}

void AssistPbxDevice::provideRingStop( )
{
	cpDebug(LOG_DEBUG, "***  Stop Ringing on %s ***", myDeviceName);
//	deviceMutex.lock();

	status = pbx_offhook;
	sendGreeting = true;
	getDigitCollector()->reset();
	resetPbxDtmfBuffer();

//	deviceMutex.unlock();
}

void AssistPbxDevice::provideLocalRingbackStart()
{
	cpDebug(LOG_DEBUG, "***  Start local ringback on %s ***", myDeviceName);
	sendRingback = true;
//	deviceMutex.lock();
//	deviceMutex.unlock();
}

void AssistPbxDevice::provideLocalRingbackStop( )
{
	cpDebug(LOG_DEBUG, "***  Stop local ringback on %s ***" ,myDeviceName);
//	deviceMutex.lock();
//	deviceMutex.unlock();
}

void AssistPbxDevice::provideBusyToneStart( )
{
	cpDebug(LOG_DEBUG, "***  Start busy tone on %s ***" , myDeviceName);
	pbxStartTimer(DeviceSignalBusyStart);
	status = pbx_hanging;
	sendbusy = true;
//	deviceMutex.lock();
#if 0
	if( status == pbx_offhook)
	{/* receive the callee number for second time */
		status = pbx_hanging;
	//	if(calleeNum==0)
		{
			agent->reportEvent( DeviceEventHookDown, AS_DEVICE_NOT_DEFINED);
		}
	}
#endif	
//	deviceMutex.unlock();
}

void AssistPbxDevice::provideBusyToneStop()
{
	cpDebug(LOG_DEBUG, "***  Stop busy tone on %s ***" , myDeviceName);
//	deviceMutex.lock();
	status = pbx_unused;
	sendbusy = false;
//	deviceMutex.unlock();
}

void AssistPbxDevice::provideFastBusyToneStart( )
{
	cpDebug(LOG_DEBUG, "***  Start fast busy tone on %s ***" , myDeviceName);
	pbxStartTimer(DeviceSignalBusyStart);
	status = pbx_hanging;
	sendbusy = true;
//	deviceMutex.lock();
//	status = pbx_hanging;
//	deviceMutex.unlock();
}

void AssistPbxDevice::provideFastBusyToneStop( )
{
	cpDebug(LOG_DEBUG, "***  Stop fast busy tone on %s ***" , myDeviceName);
//	deviceMutex.lock();
	status = pbx_unused;
	sendbusy = false;
//	deviceMutex.unlock();
}

void AssistPbxDevice::provideCallWaitingBeepStart()
{
	cpDebug(LOG_DEBUG, "***  Start call waiting beep on %s***", myDeviceName);
//	deviceMutex.lock();
//	deviceMutex.unlock();
}


void AssistPbxDevice::provideCallWaitingBeepStop()
{
	cpDebug(LOG_DEBUG, "***  Stop call waiting beep on %s***" , myDeviceName);
//	deviceMutex.lock();
//	deviceMutex.unlock();
}


/***/
void AssistPbxDevice::ProcessRTP()
{
	static int count = 0;
	if (audioStack == 0)
	{
//		vusleep(10000);
//		cpDebug(LOG_DEBUG, "PbxRtpThread audiostack is null");
		return ;
	}

	RtpSessionState sessionState = audioStack->getSessionState();
//cpDebug(LOG_DEBUG, "PbxRtpThread audiostack on device %s, Id is %d", myDeviceName, myDeviceId);
 
	
	//only process receive if receive is enabled
	if ( sessionState == rtp_session_recvonly ||
		sessionState == rtp_session_sendrecv )
	{
#if  0	
/* following is test code for dsp parse DTMF signal from audio */
		int res;
		char phonenum[64];
		int length = 160*14;
		int fd = audioStack->getRtpRecv()->getSocketFD();
		res = as_dsp_dtmf_detect(fd, U_LAW_CODE, length, phonenum, 64);
		if(res>0)
		{
			printf("CallerID '%s' detected in data buffer with length of %d\n", phonenum, length);
		}	
#endif			
		inRtpPkt = audioStack->receive ();
		if (inRtpPkt)
		{
			int cc;
				
			cc = inRtpPkt->getPayloadUsage();
			if ( cc != RESID_RTP_RATE)
				cpDebug(LOG_DEBUG, "RtpStack API size doesn't match RESID_RTP_RATE");

			if(pbxDtmfMode==as_pbx_dtmf_audio)
			{
				char *vBuf;
				vBuf = inRtpPkt->getPayloadLoc();
			
				myReadBuffer.put(vBuf, cc);
#if 0
				char buf[DTMF_PARSE_LENGTH];
				char phonenumber[32];
				if(myReadBuffer.bytesUsed() > DTMF_PARSE_LENGTH )
				{
					cc = myReadBuffer.get(buf, DTMF_PARSE_LENGTH);

					int res = (dsp->ops->dtmf_decode)(dsp, buf, cc, phonenumber, 32);
					if(res>0)
					{
						int j;
						cpDebug(LOG_DEBUG, "CallerID ='%s' in buffer with length of '%d'\n", phonenumber, cc);
						for(j=0; j< res; j++)
							emitDtmfSignal(phonenumber[j]);
					}	
				}
#endif			

#if 1
				char buf[DTMF_DETECT_STEP_SIZE];
				if(myReadBuffer.bytesUsed() > DTMF_DETECT_STEP_SIZE )
				{
					cc = myReadBuffer.get(buf, DTMF_DETECT_STEP_SIZE);

//			assist_dsp_dtmf_detect_step( dsp,  buf, DTMF_DETECT_STEP_SIZE );
					int res = (dsp->ops->dtmf_decode_step)(dsp, buf, cc );
					if(res>0)
					{
						count++;
						if(count > 10)
						{
							char phonenumber[32];
							res = (dsp->ops->dtmf_decode_get)(dsp, phonenumber, 32 );
							if(res > 0 )
							{
								int j;
								cpDebug(LOG_DEBUG, "CallerID ='%s' in buffer with length of '%d'\n", phonenumber, cc);
								for(j=0; j< res; j++)
									emitDtmfSignal(phonenumber[j]);
							}	
						}
					}	
				}
#endif				
			}	
			
			delete inRtpPkt; 
			inRtpPkt = NULL;

		}
	}

	if ( sessionState == rtp_session_sendonly ||
		sessionState == rtp_session_sendrecv )
	{
		// cc = number of samples

#if 0
		if(cc != RESID_RTP_RATE)
			cpDebug(LOG_DEBUG, "Read buffer length is %d, and required is %d\r\n", cc, RESID_RTP_RATE);
		short line2[RESID_RTP_RATE];
		int i;
		for(i=0;i<cc; i++)
		{
			line2[i] = as_lib_law_full_ulaw2linear(outBufferPkt[i] );
			outBufferPkt[i] = as_lib_law_linear2alaw(line2[i] );
		}

#endif
		int cc;

			cc = getSendBackTone( outBufferPkt, RESID_RTP_RATE);
		if (audioStack /*&& checkInterval() */)
		{
			audioStack->transmitRaw (outBufferPkt, cc);
		}
		
	}


	if (audioStack &&
		sessionState != rtp_session_inactive &&
		sessionState != rtp_session_undefined )
	{
		audioStack->processRTCP();
	}
//	deviceMutex.unlock();
}

char* AssistPbxDevice::getCwBeep()
{
	char *beep = 0;

	if( cwCadenceOn )
	{
		beep = cwBeep + cwCnt;
		cwCnt += RESID_RTP_RATE;
		if( cwCnt >= CWBEEP_ON_TIME )
		{
			cwCnt = 0;
			cwCadenceOn = false;
		}
	}
	else
	{
		cwCnt += RESID_RTP_RATE;
		if( cwCnt >= (CWBEEP_OFF_TIME - RESID_RTP_RATE) )
		{
			cwCnt = 0;
			cwCadenceOn = true;
		}
	}

	return beep;
}

int AssistPbxDevice::getSendBackTone(char *ringData, int size)
{
	int fd = -1;
	struct timeval start;      // ctam
	struct timeval finish;     // ctam
	gettimeofday (&start, 0);    // ctam

	int cc = 0;
	char tone[RESID_RTP_RATE];

	if(sendGreeting == true)
		fd = greetingFd;
	else  if(sendRingback == true)
		fd = ringbackFd;
	else if(sendbusy == true)
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

#if __ARM_IXP__
#else
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

void AssistPbxDevice::setCalleeInfo(string _callernum, string _calleenum)
{
	if( _calleenum==0 || strlen(_calleenum.c_str())==0)
	{
		if(callerNum!=0)
		{
			delete callerNum;
			callerNum = 0;
		}
		if(calleeNum!=0)
		{
			delete calleeNum;
			calleeNum = 0;
		}
	}
	else
	{
		callerNum = new string(_callernum);
		calleeNum = new string(_calleenum);
		cpDebug(LOG_DEBUG, "Caller is '%s', Callee is '%s'", callerNum->c_str(), calleeNum->c_str());
		status = pbx_ring;
	}	

}

bool AssistPbxDevice::emitDtmfSignal(char signal)
{
	stopSendGreeting();
	if(dtmf_buf.in_pos < AS_PBX_DTMF_BUFFER_LENGTH )
	{
		dtmf_buf.digits[dtmf_buf.in_pos] = signal;
		dtmf_buf.in_pos ++;
		return true;
	}
	else
	{
		cpLog(LOG_ERR, "Too much DTMF key has received from RTP stack, '%c' is ignored", signal);
		return false;
	}	
}

/* report DTMF which from RTP by both 2833 or audio in DeviceThread */
unsigned char  AssistPbxDevice::reportDtmfDigit( )
{
	if( dtmf_buf.out_pos < dtmf_buf.in_pos )
	{
		dtmf_buf.out_pos++;
		return dtmf_buf.digits[ dtmf_buf.out_pos-1 ];
	}
	return 'X';
}

void AssistPbxDevice::resetPbxDtmfBuffer()
{
	dtmf_buf.in_pos = 0;
	dtmf_buf.out_pos = 0;
	memset(dtmf_buf.digits, 0, AS_PBX_DTMF_BUFFER_LENGTH);
}

#if 0
Sptr<SipCallId> AssistPbxDevice::getPbxCallId( ) const
{
	return pbxCallId;
}

void AssistPbxDevice::setPbxCallId( const Sptr<SipCallId> newCallId )
{
	if( newCallId == 0 )
		cpDebug(LOG_DEBUG, "Clear PBX internal Call Id");

	pbxCallId = newCallId;

	if( pbxCallId == 0 )
		cpDebug(LOG_DEBUG, "PBX internal Call Id is cleared");
}
#endif

void AssistPbxDevice::startSendRingback() 
{
	sendRingback = true;
}

void AssistPbxDevice::stopSendRingback() 
{
	sendRingback = false;
}

void AssistPbxDevice::startSendGreeting() 
{
	sendGreeting = true;
}

void AssistPbxDevice::stopSendGreeting() 
{
	sendGreeting = false;
}

void AssistPbxDevice::startSendBusy() 
{
	sendbusy = true;
}

void AssistPbxDevice::stopSendBusy() 
{
	sendbusy = false;
}

void AssistPbxDevice::pbxStartTimer(DeviceSignalType signal)
{
	int timeout = UaConfiguration::instance()->getAnswerRestartTimer();

	Sptr < UaHardwareEvent > timerEvent = new UaHardwareEvent( callProcessingQueue );
//	timerEvent->setCallId(callId);

	cpDebug(LOG_DEBUG, "Start TIMER for %d ms.", timeout );
	timerEvent->setDeviceId( myDeviceId );
	timerEvent->type = HardwareTimeoutType;
	timerEvent->signalOrRequest.signal = signal;

	timerEvent->startTimer(timerEvent, timeout );
}

void AssistPbxDevice::pbxTimeoutHandler(Sptr < UaHardwareEvent > timerEvent)
{
	DeviceSignalType signal = timerEvent->signalOrRequest.signal;

	switch(signal)
	{
		case DeviceSignalRingStart:
			agent->reportEvent( DeviceEventHookUp, myDeviceId );
			break;
		case DeviceSignalBusyStart:
		case DeviceSignalFastBusyStart:
			if(getHardwareStatus()==HARDWARE_UNAVAILABLE)
				agent->reportEvent( DeviceEventHookDown, myDeviceId );
			sendbusy = false;
			status = pbx_unused;
			break;
		case DeviceSignalBusyStop:
		case DeviceSignalFastBusyStop:
			status = pbx_unused;
			break;
		case DeviceSignalRemoteRingbackStart:
//			status = pbx_unused;
			sendRingback = true;
			break;
		case DeviceSignalRemoteRingbackStop:
			status = pbx_unused;
			sendRingback = false;
			break;
		default:
			cpDebug(LOG_DEBUG, "No handler for timeout event in %s", myDeviceName);
			break;
	}
}

bool AssistPbxDevice::checkInterval()
{
	struct timeval now;
	long diff;

	int err = gettimeofday(&now, NULL);
	assert( !err );

	diff = (now.tv_sec -lastTime.tv_sec)*1000*1000+ now.tv_usec -lastTime.tv_usec;
	cpDebug(LOG_DEBUG, "Diff=%ld(us)" , diff );
#if __ARM_IXP__
	if( diff >  95 *100 )
#else
	if( diff >  195 * 100 )
#endif		
	{
		lastTime.tv_sec  = now.tv_sec;
		lastTime.tv_usec = now.tv_usec;
		return true;
	}

	return false;
}
#else

AssistPbxDevice::AssistPbxDevice(AssistNullDevice *agent, char *devicename, int deviceid, Sptr < Fifo < Sptr < SipProxyEvent > > > sessionQ)
	:AssistDevice(agent, devicename, as_virtual_device, deviceid, sessionQ ),
	cwBeep( 0 ),
	cwCadenceOn( true ),
	cwCnt( 0 ),
	cwBeepOn( false ),
	callerNum(0),
	calleeNum(0),
	pbxUrl("")
{

}

AssistPbxDevice::~AssistPbxDevice()
{

}

void AssistPbxDevice::provideDialToneStart()
{
	cpDebug(LOG_DEBUG, "***  Start dial tone on device  %s ***" , myDeviceName);
	actionMutex.lock();
	lseek(greetingFd, 0, SEEK_SET);
	actionType = StartGreeting;
	gettimeofday (&prevTime, 0);
	actionMutex.unlock();
	return;
}

void AssistPbxDevice::provideQueryStatus()
{
	cpDebug(LOG_DEBUG, "***  Start query status on device  %s ***" , myDeviceName);
	count = 0;
	agent->reportEvent( DeviceEventIdle, myDeviceId );
}
void AssistPbxDevice::provideDialToneStop()
{
	cpDebug(LOG_DEBUG, "***  Stop dial tone on %s ***" , myDeviceName);
	actionMutex.lock();
	actionType = StopGreeting;
	actionMutex.unlock();
	return;
}

void AssistPbxDevice::provideRingStart()
{
	cpDebug(LOG_DEBUG, "***  Start Ringing on %s ***" , myDeviceName );

	pbxStartTimer(DeviceSignalRingStart);
}

void AssistPbxDevice::provideRingStop( )
{
	cpDebug(LOG_DEBUG, "***  Stop Ringing on %s ***", myDeviceName);
}

void AssistPbxDevice::provideLocalRingbackStart()
{
	cpDebug(LOG_DEBUG, "***  Start local ringback on %s ***", myDeviceName);
	actionMutex.lock();
	lseek(ringbackFd, 0, SEEK_SET);
	actionType = StartRingBack;
	gettimeofday (&prevTime, 0);
	actionMutex.unlock();
}

void AssistPbxDevice::provideLocalRingbackStop( )
{
	cpDebug(LOG_DEBUG, "***  Stop local ringback on %s ***" ,myDeviceName);
	actionMutex.lock();
	actionType = StopRingBack;
	actionMutex.unlock();
}

void AssistPbxDevice::provideRemoteRingbackStart()
{
	cpDebug(LOG_DEBUG, "***  Start local ringback on %s ***", myDeviceName);
	actionMutex.lock();
	lseek(ringbackFd, 0, SEEK_SET);
	actionType = StartRingBack;
	gettimeofday (&prevTime, 0);
	actionMutex.unlock();
}

void AssistPbxDevice::provideRemoteRingbackStop( )
{
	cpDebug(LOG_DEBUG, "***  Stop local ringback on %s ***" ,myDeviceName);
	actionMutex.lock();
	actionType = StopRingBack;
	actionMutex.unlock();
}
void AssistPbxDevice::provideBusyToneStart( )
{
	cpDebug(LOG_DEBUG, "***  Start busy tone on %s ***" , myDeviceName);

	actionMutex.lock();
	lseek(busyFd, 0, SEEK_SET);
	actionType = StartBusyTone;
	gettimeofday (&prevTime, 0);
	actionMutex.unlock();
 	pbxStartTimer(DeviceSignalBusyStart);
}

void AssistPbxDevice::provideBusyToneStop()
{
	cpDebug(LOG_DEBUG, "***  Stop busy tone on %s ***" , myDeviceName);
	actionMutex.lock();
	actionType = StopBusyTone;
	actionMutex.unlock();

}

void AssistPbxDevice::provideFastBusyToneStart( )
{
	cpDebug(LOG_DEBUG, "***  Start fast busy tone on %s ***" , myDeviceName);
	pbxStartTimer(DeviceSignalFastBusyStart);
}

void AssistPbxDevice::provideFastBusyToneStop( )
{
	cpDebug(LOG_DEBUG, "***  Stop fast busy tone on %s ***" , myDeviceName);
}

void AssistPbxDevice::provideCallWaitingBeepStart()
{
	cpDebug(LOG_DEBUG, "***  Start call waiting beep on %s***", myDeviceName);
	actionMutex.lock();
	lseek(callwaitingFd, 0, SEEK_SET);
	actionType = StartCallWaiting;
	gettimeofday (&prevTime, 0);
	actionMutex.unlock();
}


void AssistPbxDevice::provideCallWaitingBeepStop()
{
	cpDebug(LOG_DEBUG, "***  Stop call waiting beep on %s***" , myDeviceName);
	actionMutex.lock();
	actionType = StopCallWaiting;
	actionMutex.unlock();
}


void AssistPbxDevice::ProcessRTP()
{

	audioMutex.lock();
	
	if (audioStack == 0)
	{
		audioMutex.unlock();
		return ;
	}

	RtpSessionState sessionState = audioStack->getSessionState();
	if ( sessionState == rtp_session_recvonly ||
		sessionState == rtp_session_sendrecv )
	{
		inRtpPkt = audioStack->receive ();
		if (inRtpPkt)
		{
			int cc;
				
			cc = inRtpPkt->getPayloadUsage();
			if ( cc != RESID_RTP_RATE)
				cpDebug(LOG_DEBUG, "RtpStack API size doesn't match RESID_RTP_RATE");
#if 0
			if(pbxDtmfMode != as_pbx_dtmf_rfc2883)
			{
				char *vBuf;
				vBuf = inRtpPkt->getPayloadLoc();
			
				myReadBuffer.put(vBuf, cc);

				char buf[DTMF_DETECT_STEP_SIZE];
				if(myReadBuffer.bytesUsed() > DTMF_DETECT_STEP_SIZE )
				{
					cc = myReadBuffer.get(buf, DTMF_DETECT_STEP_SIZE);
					int res = (dsp->ops->dtmf_decode_step)(dsp, buf, cc );
					if(res>0)
					{
						count++;
						if(count > 10)
						{
							char phonenumber[32];
							res = (dsp->ops->dtmf_decode_get)(dsp, phonenumber, 32 );
							if(res > 0 )
							{
								int j;
								cpDebug(LOG_DEBUG, "CallerID ='%s' in buffer with length of '%d'\n", phonenumber, cc);
								for(j=0; j< res; j++)
								{
									pbxDtmfMode = as_pbx_dtmf_audio;
									emitDtmfSignal(phonenumber[j]);
								}	
							}	
						}
					}	
				}
			}	
#endif		
			
			delete inRtpPkt; 
			inRtpPkt = NULL;

		}
	}

	if ( sessionState == rtp_session_sendonly ||
		sessionState == rtp_session_sendrecv )
	{

		getSendBackTone( outBufferPkt, RESID_RTP_RATE);
#if 0
		if (cc > 0)
		{
			audioStack->transmitRaw (outBufferPkt, cc);
		}
#endif
		
	}


	if (audioStack &&
		sessionState != rtp_session_inactive &&
		sessionState != rtp_session_undefined )
	{
		audioStack->processRTCP();
	}
	audioMutex.unlock();

}

char* AssistPbxDevice::getCwBeep()
{
	char *beep = 0;

	if( cwCadenceOn )
	{
		beep = cwBeep + cwCnt;
		cwCnt += RESID_RTP_RATE;
		if( cwCnt >= CWBEEP_ON_TIME )
		{
			cwCnt = 0;
			cwCadenceOn = false;
		}
	}
	else
	{
		cwCnt += RESID_RTP_RATE;
		if( cwCnt >= (CWBEEP_OFF_TIME - RESID_RTP_RATE) )
		{
			cwCnt = 0;
			cwCadenceOn = true;
		}
	}

	return beep;
}

void AssistPbxDevice::setCalleeInfo(string _callernum, string _calleenum)
{
	if( _calleenum==0 || strlen(_calleenum.c_str())==0)
	{
		if(callerNum!=0)
		{
			delete callerNum;
			callerNum = 0;
		}
		if(calleeNum!=0)
		{
			delete calleeNum;
			calleeNum = 0;
		}
	}
	else
	{
		callerNum = new string(_callernum);
		calleeNum = new string(_calleenum);
		cpDebug(LOG_DEBUG, "Caller is '%s', Callee is '%s'", callerNum->c_str(), calleeNum->c_str());
	}	

}

bool AssistPbxDevice::emitDtmfSignal(char signal)
{
	switch(signal)
	{
		case '1' /*0x01*/:
			agent->reportEvent( DeviceEventDtmf1, myDeviceId);
			break;
		case '2' /*0x02*/:
			agent->reportEvent(DeviceEventDtmf2, myDeviceId);
			break;
		case '3'/*0x03*/:
			agent->reportEvent( DeviceEventDtmf3, myDeviceId);
			break;
		case '4'/*0x04*/:
			agent->reportEvent( DeviceEventDtmf4, myDeviceId);
			break;
		case '5'/*0x05*/:
			agent->reportEvent( DeviceEventDtmf5, myDeviceId);
			break;
		case '6'/*0x06*/:
			agent->reportEvent( DeviceEventDtmf6, myDeviceId);
			break;
		case '7'/*0x07*/:
			agent->reportEvent( DeviceEventDtmf7, myDeviceId);
			break;
		case '8'/*0x08*/:
			agent->reportEvent( DeviceEventDtmf8, myDeviceId);
			break;
		case '9'/*0x09*/:
			agent->reportEvent( DeviceEventDtmf9, myDeviceId);
			break;
		case '*'/*0x0A*/:
			agent->reportEvent( DeviceEventDtmfStar, myDeviceId);
			break;
		case '0'/*0x0B*/:
			agent->reportEvent( DeviceEventDtmf0, myDeviceId);
			break;
		case '#'/*0x0C*/:
			agent->reportEvent( DeviceEventDtmfHash, myDeviceId);
			break;
		default:
			break;
	}		
	return false;
}

unsigned char  AssistPbxDevice::reportDtmfDigit( )
{
	return ' ';	
}

void AssistPbxDevice::resetPbxDtmfBuffer()
{
	dtmf_buf.in_pos = 0;
	dtmf_buf.out_pos = 0;
	memset(dtmf_buf.digits, 0, AS_PBX_DTMF_BUFFER_LENGTH);
}

void AssistPbxDevice::pbxStartTimer(DeviceSignalType signal)
{
	int timeout = UaConfiguration::instance()->getAnswerRestartTimer();

	Sptr < UaHardwareEvent > timerEvent = new UaHardwareEvent( UaDevice::getDeviceQueue());
	cpDebug(LOG_DEBUG, "Start TIMER for %d ms.", timeout );
	timerEvent->setDeviceId( myDeviceId );
	timerEvent->type = HardwareTimeoutType;
	timerEvent->signalOrRequest.signal = signal;

	timerEvent->startTimer(timerEvent, timeout );
}

void AssistPbxDevice::pbxTimeoutHandler(Sptr < UaHardwareEvent > timerEvent)
{
	DeviceSignalType signal = timerEvent->signalOrRequest.signal;

	switch(signal)
	{
		case DeviceSignalRingStart:
			agent->reportEvent( DeviceEventHookUp, myDeviceId );
			break;
		case DeviceSignalFastBusyStart:
		case DeviceSignalBusyStart:
			agent->reportEvent( DeviceEventHookDown, myDeviceId );
			break;
		case DeviceSignalBusyStop:
		case DeviceSignalFastBusyStop:
			break;
		case DeviceSignalRemoteRingbackStart:
			break;
		case DeviceSignalRemoteRingbackStop:
			break;
		default:
			cpDebug(LOG_DEBUG, "No handler for timeout event in %s", myDeviceName);
			break;
	}
}

#endif
		
