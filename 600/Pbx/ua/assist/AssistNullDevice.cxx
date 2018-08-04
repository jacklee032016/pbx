/* $Id: AssistNullDevice.cxx,v 1.3 2007/05/13 20:49:37 lizhijie Exp $
 */

#include "global.h"

/* stdlib */
#include <iostream>
#include <cstdio>
#include <unistd.h>
#include <sys/ioctl.h>
#include <csignal>

/* sockets */
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <cerrno>
#include <linux/telephony.h>
#include <linux/ixjuser.h>

#include "VTime.hxx"
#include "Rtp.hxx"
#include "VCondition.h"

/* error handling */
#include "UaDevice.hxx"

#include "AssistNullDevice.hxx"
#include "UaDeviceEvent.hxx"
#include "UaConfiguration.hxx"

#include "AssistDeviceInfo.hxx"
#include "SystemInfo.hxx"
#include "AssistInvalidDevice.hxx"

using namespace Vocal;

#if 0 //modified by lijie 2005-06-03
AssistNullDevice::AssistNullDevice( const char* deviceName, as_span_t *_span,
                                Sptr < Fifo < Sptr < SipProxyEvent > > > inputQ,
                                Sptr < Fifo < Sptr < SipProxyEvent > > > outputQ , assist_app_type apptype)
        : ResGwDevice( deviceName, inputQ, outputQ  ),
        appType(apptype)        
{
	int i;
	as_device_t *dev;
	char name[AS_DEVICE_NAME_LENGTH];
	// initialize queues
	sessionQ = outputQ;
	myQ = inputQ;

	span = _span;

	hookStateOffhook = false;

	DigitTimeout::instance( outputQ, UaConfiguration::instance()->getInitialDigitTimeout(), 
		UaConfiguration::instance()->getInterDigitTimeout() );

	span->pbx_count = UaConfiguration::instance()->getPbxCount();
	if( span->pbx_count > span->count)
		span->pbx_count = span->count;
	
	for(i=0; i<span->count; i++)
	{
		dev = span->devs[i];
		dev->private_data = new AssistDeviceInfo(this, span, i, sessionQ);
	}

	for(i=0; i<span->pbx_count; i++)
	{
		sprintf(name, "virtual_device_%d", i+1 );
		span->pbx_devs[i] = new AssistPbxDevice( this, name , VIRTUAL_DEVICES_EPOCH+ i, sessionQ);
	}

	myDeviceName = deviceName;
}  // end Constructor


AssistNullDevice::~AssistNullDevice()
{
	as_device_t *dev;
	int i;

	for(i=0; i<span->count; i++)
	{
		dev = span->devs[i];
		delete (AssistDeviceInfo *)dev->private_data;
		dev->private_data = 0;
	}

	for(i=0; i<span->pbx_count; i++)
	{
		delete (AssistPbxDevice *)span->pbx_devs[i];
		span->pbx_devs[i] = 0;
	}

} // end 


void* AssistNullDevice::slicHardwareMain(void *param)
{
	fd_set readfds;

	struct timeval tv;
	int retval;
	int maxFd = 128;  // should be 0 then increment as needed when addToFdSet

	// reset file descriptor
	FD_ZERO(&readfds);
	addToFdSet(&readfds);
	// process forever on behalf of AssistNullDevice hardware

	tv.tv_sec = 0;
	tv.tv_usec = 500;

	/* monitor the exception fdset with only assist device when unlocked */
	if((retval = select(maxFd, 0, 0, &readfds, &tv)) < 0)
	{
		cpLog( LOG_ERR, "select() returned error %d", retval );
	}
	else
	{
		if(process(&readfds) < 0)
		{
			cpLog( LOG_ERR, "hardware encountered an error" );
			assert(0);
		}
	}

	//added by lijie
	int i;
	as_device_t *dev;
	AS_DEVICE_STATE state;

	deviceMutex.lock();
	for(i=0; i< span->count; i++)
	{
		dev = span->devs[i];
		state = span->ops->get_status( dev );
		if(state == AS_DEVICE_STATE_IDLE)
		{
			reportEvent( DeviceEventIdle, dev->id);
			cpDebug(LOG_DEBUG, "device '%s' idle", dev->name);
		}		
		else if(state == AS_DEVICE_STATE_BUSY)
		{
			reportEvent( DeviceEventBusy, dev->id);
			cpDebug(LOG_DEBUG, "device '%s' busy", dev->name);			
		}
		//added ends here
	}	
       deviceMutex.unlock();
	//added ends here
	return 0;
}

void* AssistNullDevice::pcmHardwareMain(void *param)
{
	int i;
	as_device_t *dev;
	AS_DEVICE_STATE state;
	vusleep(1000*200);  /* delay for 1*n ms */

	deviceMutex.lock();
	for(i=0; i< span->count; i++)
	{
		dev = span->devs[i];
		state = span->ops->check_status( dev );
		if(state == AS_DEVICE_STATE_OFFHOOK )
		{
			if(dev->state == AS_DEVICE_STATE_ONHOOK )
			{
				dev->state = AS_DEVICE_STATE_OFFHOOK;
				reportEvent( DeviceEventHookUp, dev->id );
				cpDebug(LOG_DEBUG, "device '%s' hookup", dev->name);
			}
		}/* end of device in the state of offhook */
		else if(state == AS_DEVICE_STATE_ONHOOK)
		{/* device is in the state of on-hook */
			if(dev->state == AS_DEVICE_STATE_OFFHOOK )
			{
				dev->state = AS_DEVICE_STATE_ONHOOK;
				reportEvent( DeviceEventHookDown, dev->id);
				cpDebug(LOG_DEBUG, "device '%s' hookdown", dev->name);
			}
		}	
		else if(state == AS_DEVICE_STATE_WINKFALSH)
		{
		}

		if(dev->state == AS_DEVICE_STATE_OFFHOOK)
		{
			reportHardwareDtmfEvent( (AssistDevice *) dev->private_data);
		}

		//added by lijie to query if device is busy or idle
		state = span->ops->get_status( dev );
		if(state == AS_DEVICE_STATE_IDLE)
		{
			reportEvent( DeviceEventIdle, dev->id);
			printf("device idle\n");
			cpDebug(LOG_DEBUG, "device '%s' idle", dev->name);
		}		
		else if(state == AS_DEVICE_STATE_BUSY)
		{
			reportEvent( DeviceEventBusy, dev->id);
			printf("device busy\n");
			cpDebug(LOG_DEBUG, "device '%s' busy", dev->name);			
		}
	}

	deviceMutex.unlock();
	return 0;
}


void* AssistNullDevice::hardwareMain (void* parms)
{
	int i;
	if(appType == assist_app_slic)
		slicHardwareMain( parms);
	else
		pcmHardwareMain( parms );

	pbxDeviceMutex.lock();
	for(i=0; i<span->pbx_count; i++)
	{
		AssistPbxDevice* dev = (AssistPbxDevice *)span->pbx_devs[i];
//		cpDebug(LOG_DEBUG, "free is checked on device %d" ,dev->getDeviceId() );
		if(dev->getStatus() == pbx_offhook)
		{
			reportHardwareDtmfEvent( dev);
		}	
	}
	pbxDeviceMutex.unlock();


	if( myQ->size() > 0 )
	{
		cpDebug(LOG_DEBUG, "Signal from call control" );
		processSessionMsg( myQ->getNext() );
	}

	return 0;
} // end hardwareMain()

int AssistNullDevice::process (fd_set* fd)
{
	int i;
	as_device_t *dev;
	AS_DEVICE_STATE state;

	vusleep(0);  // needed to stabilize hookstate readings

//	cpDebug(LOG_DEBUG, "process" );

	deviceMutex.lock();

	for(i=0; i< span->count; i++)
	{
		dev = span->devs[i];
		state = span->ops->check_status( dev );
		if(state == AS_DEVICE_STATE_OFFHOOK )
		{
			if(dev->state == AS_DEVICE_STATE_ONHOOK )
			{
				dev->state = AS_DEVICE_STATE_OFFHOOK;
//				if( hookStateOffhook == false )
				{
//					hookStateOffhook = true;
					reportEvent( DeviceEventHookUp, dev->id );
					cpDebug(LOG_DEBUG, "device '%s' hookup", dev->name);
				}
			}
		}/* end of device in the state of offhook */
		else if(state == AS_DEVICE_STATE_ONHOOK)
		{/* device is in the state of on-hook */
			if(dev->state == AS_DEVICE_STATE_OFFHOOK )
			{
				vusleep(20000);  // needed to stabilize hookstate readings
//				state = span->ops->check_status( dev );
				state = as_device_check_state_on_startup(dev->fd, dev->name);

				if(state == AS_DEVICE_STATE_ONHOOK )

				{
//						deviceMutex.unlock();
					dev->state = AS_DEVICE_STATE_ONHOOK;
					reportEvent( DeviceEventHookDown, dev->id);
					cpDebug(LOG_DEBUG, "device '%s' hookdown", dev->name);
//						deviceMutex.lock();
				}
#if 0				
				if( hookStateOffhook == true )
				{

				vusleep(20000);  // needed to stabilize hookstate readings
					state = span->ops->check_status( dev );
					if(state == AS_DEVICE_STATE_ONHOOK )
					{
						deviceMutex.unlock();
						onhookOrFlash(dev);
						deviceMutex.lock();
					}
				}
#endif				

			}
		}	
		else if(state == AS_DEVICE_STATE_WINKFALSH)
		{
		}

		if(dev->state == AS_DEVICE_STATE_OFFHOOK)
		{
			reportHardwareDtmfEvent( (AssistDevice *) dev->private_data);
		}

	}

	deviceMutex.unlock();
	return 0;
} // end process()


void AssistNullDevice::processRTP ()
{
	int i;
	as_device_t  *dev;
	AssistDeviceInfo *devInfo;

	deviceMutex.lock();

	for(i=0; i<span->count; i++)
	{
		dev = span->devs[i];
		devInfo = (AssistDeviceInfo *) dev->private_data ;
		assert(devInfo!=0 );
		devInfo->ProcessRTP();
	}
	
	deviceMutex.unlock();
}

void AssistNullDevice::processPbxRTP ()
{
	int i;
	AssistPbxDevice *dev;

	pbxDeviceMutex.lock();

	for(i=0; i<span->pbx_count; i++)
	{
		dev = (AssistPbxDevice *)span->pbx_devs[i];
		assert(dev!=0 );
		dev->ProcessRTP();
	}
	
	pbxDeviceMutex.unlock();
}


int AssistNullDevice::addToFdSet (fd_set* fd)
{
#if 0
	if ( ( audioActive || audioHalfActive ) && audioStack )
	{
		RtpSessionState sessionState = audioStack->getSessionState();
		if ( sessionState == rtp_session_recvonly ||
			sessionState == rtp_session_sendrecv )
		{
			FD_SET((audioStack->getRtpRecv())->getSocketFD(), fd);
			FD_SET((audioStack->getRtcpRecv())->getSocketFD(), fd);
		}
	}

	FD_SET(myFD, fd);
#endif
#if __WITH_SLIC__
	int i;
	for(i=0; i< span->count; i++)
	{
		FD_SET((span->devs[i])->fd,  fd);
	}
#endif

	return 0;
} // addToFdSet()

int AssistNullDevice::audioStart (const HardwareAudioRequest& request,int deviceId)
{
	deviceMutex.lock();
	AssistDevice *dev = getAssistDevice( deviceId);
	if(dev==0)
	{
		cpLog(LOG_EMERG, "No hardware device with Id as '%d'", deviceId);
		exit(1);
	}
	else
		dev->audioStart( request);

	deviceMutex.unlock();
	return 0;
} // end audioStart()

int AssistNullDevice::audioStop(int deviceId)
{
	deviceMutex.lock();
	AssistDevice *dev = getAssistDevice( deviceId);
	if(dev==0)
	{
		cpLog(LOG_EMERG, "No hardware device with Id as '%d'", deviceId);
		exit(1);
	}
	else
		dev->audioStop();

	deviceMutex.unlock();
	return 0;
} // end audioStop()

/* description: suspend the RTP transmit and receive, and also stop the */
int AssistNullDevice::audioSuspend (int deviceId)
{
	deviceMutex.lock();
	AssistDevice *dev = getAssistDevice( deviceId);
	if(dev==0)
	{
		cpLog(LOG_EMERG, "No hardware device with Id as '%d'", deviceId);
		exit(1);
	}
	else
		dev->audioSuspend();

	deviceMutex.unlock();
	return 0;
} // end audioSuspend()

int AssistNullDevice::audioResume (const HardwareAudioRequest& request,int deviceId)
{
	deviceMutex.lock();
	AssistDevice *dev = getAssistDevice(deviceId);
	if(dev ==0)
	{
		cpLog(LOG_EMERG, "No hardware device with Id as '%d'", deviceId);
		exit(1);
	}
	else
		dev->audioResume(request);

	deviceMutex.unlock();
	return 0;
} // end audioResume()

void AssistNullDevice::onhookOrFlash(as_device_t *dev)
{
	AS_DEVICE_STATE state;
	deviceMutex.lock();
	vusleep(20000);  // needed to stabilize hookstate readings

	state = span->ops->check_status( dev );
	if( state==AS_DEVICE_STATE_ONHOOK )
	{	// user is onhook or doing a flash
		hookStateOffhook = false;
		// need to wait FLASH_TIMEOUT microsec before reporting onhook
		// if user picks up before the timeout, report flash
		int flashTime = 0;

		//cpDebug(LOG_DEBUG,  "Starting Flash Timer");
		while (flashTime < FLASH_TIMEOUT) // begin testing for flash event
		{
			vusleep(50000);  // sleep for 50,000 microsec (50 ms)
			flashTime += 50000;
			state = span->ops->check_status( dev );
			if(state == AS_DEVICE_STATE_OFFHOOK)
			{
				flashTime = FLASH_TIMEOUT;  // will get us out of while-loop
				//cpDebug(LOG_DEBUG,  "Hardware Event: deviceEventFlash");
				hookStateOffhook = true;
				reportEvent( DeviceEventFlash, dev->id);
			}
		} // end period for testing for a flash event
		//cpDebug(LOG_DEBUG,  "End Flash Timer");

		if (hookStateOffhook == false)
			// this must be an actual onhook event
		{
			//cpDebug(LOG_DEBUG,  "Hardware Event: deviceEventHookDown");
			reportEvent( DeviceEventHookDown, dev->id);
		}
	}
	deviceMutex.unlock();
} // end onhookOrFlash

void AssistNullDevice::provideDialToneStart(int deviceId)
{
	cpDebug(LOG_DEBUG, "***  Start dial tone on device with id %d ***" , deviceId);
#if 0
	if(deviceId==AS_DEVICE_NOT_DEFINED)
		return myPbxDev->provideDialToneStart();
	deviceMutex.lock();
	span->ops->tone_dial_start(span->devs[deviceId]);
	deviceMutex.unlock();
#endif
	AssistDevice *dev = getAssistDevice( deviceId);
	assert(dev != 0 );
	dev->provideDialToneStart();

}

//added by lijie to query device status
void AssistNullDevice::provideQueryStatus(int deviceId)
{
	cpDebug(LOG_DEBUG, "***  Start query status on device with id %d ***" , deviceId);
#if 0
	if(deviceId==AS_DEVICE_NOT_DEFINED)
		return myPbxDev->provideDialToneStart();
	deviceMutex.lock();
	span->ops->tone_dial_start(span->devs[deviceId]);
	deviceMutex.unlock();
#endif
	AssistDevice *dev = getAssistDevice( deviceId);
	assert(dev != 0 );
	dev->provideQueryStatus();

}
void AssistNullDevice::provideDialToneStop(int deviceId)
{
	cpDebug(LOG_DEBUG, "***  Stop dial tone  ***" );
	AssistDevice *dev = getAssistDevice( deviceId);
	assert(dev != 0 );
	dev->provideDialToneStop();
}


void AssistNullDevice::provideRingStart(int deviceId)
{
	cpDebug(LOG_DEBUG, "***  Start Ringing  ***" );
	AssistDevice *dev = getAssistDevice( deviceId);
	assert(dev != 0 );
	dev->provideRingStart();
}


void AssistNullDevice::provideRingStop(int deviceId)
{
	cpDebug(LOG_DEBUG, "***  Stop Ringing  ***" );
	AssistDevice *dev = getAssistDevice( deviceId);
	assert(dev != 0 );
	dev->provideRingStop();
}

void AssistNullDevice::provideLocalRingbackStart(int deviceId)
{
	cpDebug(LOG_DEBUG, "***  Start local ringback  ***" );
	AssistDevice *dev = getAssistDevice( deviceId);
	assert(dev != 0 );
	dev->provideLocalRingbackStart();
}


void AssistNullDevice::provideLocalRingbackStop(int deviceId)
{
	cpDebug(LOG_DEBUG, "***  Stop local ringback  ***" );
	AssistDevice *dev = getAssistDevice( deviceId);
	assert(dev != 0 );
	dev->provideLocalRingbackStop();
}


void AssistNullDevice::provideBusyToneStart(int deviceId)
{
	cpDebug(LOG_DEBUG, "***  Start busy tone  ***" );
	AssistDevice *dev = getAssistDevice( deviceId);
	assert(dev != 0 );
	dev->provideBusyToneStart();
}


void AssistNullDevice::provideBusyToneStop(int deviceId)
{
	cpDebug(LOG_DEBUG, "***  Stop busy tone  ***" );
	AssistDevice *dev = getAssistDevice( deviceId);
	assert(dev != 0 );
	dev->provideBusyToneStop();
}


void AssistNullDevice::provideFastBusyToneStart(int deviceId)
{
	cpDebug(LOG_DEBUG, "***  Start fast busy tone  ***" );
	AssistDevice *dev = getAssistDevice( deviceId);
	assert(dev != 0 );
	dev->provideFastBusyToneStart();
}


void AssistNullDevice::provideFastBusyToneStop(int deviceId)
{
	cpDebug(LOG_DEBUG, "***  Stop fast busy tone  ***" );
	AssistDevice *dev = getAssistDevice( deviceId);
	assert(dev != 0 );
	dev->provideFastBusyToneStop();
}


void AssistNullDevice::provideCallWaitingBeepStart(int deviceId)
{
	cpDebug(LOG_DEBUG, "***  Start call waiting beep ***" );
	AssistDevice *dev = getAssistDevice( deviceId);
	assert(dev != 0 );
	dev->provideCallWaitingBeepStart();
}


void AssistNullDevice::provideCallWaitingBeepStop(int deviceId)
{
	cpDebug(LOG_DEBUG, "***  Stop call waiting beep ***" );
	AssistDevice *dev = getAssistDevice( deviceId);
	assert(dev != 0 );
	dev->provideCallWaitingBeepStop();
}


void AssistNullDevice::provideDtmf(DeviceSignalType signal,int deviceId)
{
	char digit = ' ';
	int dtmfEvent = 0;

	switch( signal )
	{
		case DeviceSignalDigitStar: 
			digit = '*';
			dtmfEvent = 10;
			break;
		case DeviceSignalDigitHash:
			digit = '#';
			dtmfEvent = 11;
			break;
		case DeviceSignalDigit0:
		case DeviceSignalDigit1:
		case DeviceSignalDigit2:
		case DeviceSignalDigit3:
		case DeviceSignalDigit4:
		case DeviceSignalDigit5:
		case DeviceSignalDigit6:
		case DeviceSignalDigit7:
		case DeviceSignalDigit8:
		case DeviceSignalDigit9:
			digit = '0' + signal - DeviceSignalDigit0;
			dtmfEvent = signal - DeviceSignalDigit0;
			break;
		default:
			cpDebug(LOG_DEBUG, "Unknown DTMF Signal %d", signal );
			return;
	}




	AssistDevice *dev = getAssistDevice( deviceId );
	if(dev==0)
	{
		cpDebug(LOG_DEBUG, "No device existing for device ID %d", deviceId );
	}
	else
	{
		RtpSession *_audioStack = dev->getAudioStack();
		if( _audioStack && dev->getDeviceType()==as_hardware_device 
			&& (UaConfiguration::instance()->getPbxDtmfMode()==as_pbx_dtmf_rfc2883) )
		{
			cpDebug(LOG_DEBUG, "received DTMF Signal '%c' from WorkerThread for DeviceId %d, and transfer to remote end in RFC2833", digit ,deviceId );
			cpDebug(LOG_DEBUG, "DTMF %c sent out in RTP ", digit );
			_audioStack->transmitEvent( dtmfEvent );
		}
		else
		{
			cpDebug(LOG_DEBUG, "No audio stack existing to send DTMF event" );
		}
	}	
}


void AssistNullDevice::provideCallInfo(string _callerNum, string _calleeNum, string reason)
{
	cpDebug(LOG_DEBUG, "Caller is '%s',\t\tCallee is '%s',\t\treason is '%s'\r\n" , 
		_callerNum.c_str(), _calleeNum.c_str(), reason.c_str() );
//	myPbxDev->setCalleeInfo( _callerNum,  _calleeNum);
}

void AssistNullDevice::reportHardwareDtmfEvent(AssistDevice *dev)
{
//	if(as_dtmf_is_ready(dev->fd, dev->name) )
	{
		switch(dev->reportDtmfDigit() )
		{
			case '1' /*0x01*/:
				reportEvent( DeviceEventDtmf1, dev->getDeviceId());
				break;
			case '2' /*0x02*/:
				reportEvent(DeviceEventDtmf2, dev->getDeviceId());
				break;
			case '3'/*0x03*/:
				reportEvent( DeviceEventDtmf3, dev->getDeviceId());
				break;
			case '4'/*0x04*/:
				reportEvent( DeviceEventDtmf4, dev->getDeviceId());
				break;
			case '5'/*0x05*/:
				reportEvent( DeviceEventDtmf5, dev->getDeviceId());
				break;
			case '6'/*0x06*/:
				reportEvent( DeviceEventDtmf6, dev->getDeviceId());
				break;
			case '7'/*0x07*/:
				reportEvent( DeviceEventDtmf7, dev->getDeviceId());
				break;
			case '8'/*0x08*/:
				reportEvent( DeviceEventDtmf8, dev->getDeviceId());
				break;
			case '9'/*0x09*/:
				reportEvent( DeviceEventDtmf9, dev->getDeviceId());
				break;
			case '*'/*0x0A*/:
				reportEvent( DeviceEventDtmfStar, dev->getDeviceId());
				break;
			case '0'/*0x0B*/:
				reportEvent( DeviceEventDtmf0, dev->getDeviceId());
				break;
			case '#'/*0x0C*/:
				reportEvent( DeviceEventDtmfHash, dev->getDeviceId());
				break;
			default:
//				cpLog(LOG_ERR, "Unrecognized DTMF digit from hardware");
				break;
		}
	}
}

#if 0
void AssistNullDevice::setPbxCallId( const Sptr<SipCallId> newCallId , int deviceId)
{
	cpDebug(LOG_DEBUG, "setPbxCallId" );
	AssistDevice *dev = getAssistDevice( deviceId);
	AssistPbxDevice *pbxDev = (AssistPbxDevice *)dev ;
//	pbxDev.staticCast( dev );
	
	if(pbxDev == 0)
	{
		cpDebug(LOG_DEBUG, "No PBX device with deviceId '%d' ", deviceId);
		exit(1);
	}
	pbxDev->setPbxCallId( newCallId);

	cpDebug(LOG_DEBUG, "Pbx CallID is added into device with deviceId '%d' ", deviceId);
	return;
}


Sptr<SipCallId> AssistNullDevice::getPbxCallId( int deviceId) 
{
	cpDebug(LOG_DEBUG, "getPbxCallId" );
	AssistDevice *dev = getAssistDevice( deviceId);
	
	AssistPbxDevice *pbxDev = (AssistPbxDevice *)dev ;
//	pbxDev.dynamicCast( dev );
	if(pbxDev == 0)
	{
		cpDebug(LOG_DEBUG, "No PBX device with deviceId '%d' ", deviceId);
		exit(1);
	}
	return pbxDev->getPbxCallId();
}
#endif

void AssistNullDevice::setCallId( const Sptr<SipCallId> newCallId , int deviceId)
{
	AssistDevice *dev = getAssistDevice( deviceId);
	if(dev != 0)
	{
		dev->setCallId( newCallId);
		return;
	}
	cpDebug(LOG_DEBUG, "No device with deviceId '%d' ", deviceId);
}


Sptr<SipCallId> AssistNullDevice::getCallId( int deviceId) 
{
	cpDebug(LOG_DEBUG, "getCallId" );
	
	AssistDevice *dev = getAssistDevice( deviceId );
	if(dev != 0)
		return dev->getCallId();
	cpDebug(LOG_DEBUG, "No callId in device with deviceId '%d' ", deviceId);
	return 0;
}

//added by lijie to allow call waiting feath
void AssistNullDevice::addCallWaitingId( const Sptr<SipCallId> newCallId , int deviceId)
{
	AssistDevice *dev = getAssistDevice( deviceId);
	if(dev != 0)
	{
		dev->addCallWaitingId( newCallId);
		return;
	}
	cpDebug(LOG_DEBUG, "No device with deviceId '%d' ", deviceId);
}

Sptr<SipCallId> AssistNullDevice::getCallWaitingId( int deviceId)//; // const
{
	//cpDebug(LOG_DEBUG, "getCallWaitingId" );
	
	//AssistDevice *dev;// = getAssistDevice( deviceId);
	AssistDevice *dev = getAssistDevice( deviceId);
	
	if(dev != 0)
		return dev->getCallWaitingId();
	
	cpDebug(LOG_DEBUG, "No callId in device with deviceId '%d' ", deviceId);
	return 0;
}

void AssistNullDevice::removeCallWaitingId( const SipCallId& callId , int deviceId)
{
	AssistDevice *dev = getAssistDevice( deviceId);
	
	if(dev != 0)
	{
		dev->removeCallWaitingId( callId);
		return;
	}
	
	cpDebug(LOG_DEBUG, "No device with deviceId '%d' ", deviceId);
}


bool  AssistNullDevice::isOnCallWaiting( const SipCallId& callId, int deviceId) 
{
	cpDebug(LOG_DEBUG, "isOnCallWaiting" );
	
	AssistDevice *dev = getAssistDevice( deviceId );
	
	if(dev != 0)
		return dev->isOnCallWaiting(callId);
	
	cpDebug(LOG_DEBUG, "No device with deviceId '%d' ", deviceId);
	return false;
}
//added ends here
Sptr<DigitCollector> AssistNullDevice::getDigitCollector(int deviceId) 
{
	cpDebug(LOG_DEBUG, "get DigitCollector for device with ID '%d'", deviceId);
	AssistDevice *dev = getAssistDevice( deviceId );
	assert(dev!=0);
//	if( dev!= 0)
		return dev->getDigitCollector();
//	cpLog(LOG_ERR, "No DigitCollector for device with ID '%d'", deviceId);
//	return 0;
}

int AssistNullDevice::getDeviceId( const SipCallId& callId ) 
{
	int i;
	Sptr <SipCallId> myCallId;

	for(i=0; i<span->count; i++)
	{
		as_device_t  *dev = span->devs[i];
		AssistDeviceInfo *devInfo = (AssistDeviceInfo *) dev->private_data ;
		myCallId  = devInfo->getCallId();
		if(myCallId==0)
			continue;
		if( myCallId->getHost()== callId.getHost()
			&& myCallId->getLocalId()== callId.getLocalId() )
		{
			return dev->id;
		}
	}

	for(i=0; i<span->pbx_count; i++)
	{
		AssistPbxDevice* dev = (AssistPbxDevice *)span->pbx_devs[i];
		myCallId  = dev->getCallId();
		if(myCallId==0)
			continue;
		if( myCallId->getHost()== callId.getHost()
			&& myCallId->getLocalId()== callId.getLocalId() )
		{
			return dev->getDeviceId();
		}
	}
	
	return AS_DEVICE_NOT_DEFINED;
}

int AssistNullDevice::getPbxDeviceId( const SipCallId& callId ) 
{
	int i;
	Sptr <SipCallId> myCallId;

	for(i=0; i<span->pbx_count; i++)
	{
		AssistPbxDevice* dev = (AssistPbxDevice *)span->pbx_devs[i];
		myCallId  = dev->getCallId();
		if(myCallId != 0 )
		{
			if( myCallId->getHost()== callId.getHost()
				&& myCallId->getLocalId()== callId.getLocalId() )
			{
				return dev->getDeviceId();
			}
		}	
#if 0		
		myCallId  = dev->getPbxCallId();
		if(myCallId != 0 )
		{
			if( myCallId->getHost()== callId.getHost()
				&& myCallId->getLocalId()== callId.getLocalId() )
			{
				return dev->getDeviceId();
			}
		}	
#endif		
	}
	
	return AS_DEVICE_NOT_DEFINED;
}

int AssistNullDevice::getFreePbxDevice( ) 
{
	int i;

	for(i=0; i<span->pbx_count; i++)
	{
		AssistPbxDevice* dev = (AssistPbxDevice *)span->pbx_devs[i];
		cpDebug(LOG_DEBUG, "free is checked on device %d" ,dev->getDeviceId() );
		if(dev->getStatus() == pbx_unused )
		{
			cpDebug(LOG_DEBUG, "device %d is freed" ,dev->getDeviceId() );
			return dev->getDeviceId();
		}	
	}
	
	return AS_DEVICE_NOT_DEFINED;
}

AssistDevice* AssistNullDevice::getAssistDevice(int deviceId)
{
	int i;
	as_device_t  *dev;
	AssistDevice *devInfo;

	for(i=0; i<span->count; i++)
	{
		dev = span->devs[i];
		if(dev->id == deviceId)
		{
			devInfo = (AssistDevice *)dev->private_data;
			return devInfo;
		}
	}

	for(i=0; i<span->pbx_count; i++)
	{
		devInfo = (AssistDevice *)span->pbx_devs[i];
		if(devInfo->getDeviceId() == deviceId )
		{
			return devInfo;
		}
	}
	return 0;
}

/* only used to lookup hardware device for local dial, so pbx device is not return */
AssistDevice* AssistNullDevice::getAssistDeviceInfo(const char *deviceName)
{
	int i;
	as_device_t  *dev;
	AssistDeviceInfo *devInfo;

	for(i=0; i<span->count; i++)
	{
		dev = span->devs[i];
		if( strcmp(dev->name, deviceName)==0)
		{
			devInfo = (AssistDeviceInfo *)dev->private_data;
			return devInfo;
		}
	}
	return 0;
}

/* only used to lookup hardware device for local dial, so pbx device is not return */
AssistDevice* AssistNullDevice::getAssistDeviceInfoByPhoneNumber(const char *phone)
{
	int i;
	as_device_t  *dev;
	AssistDeviceInfo *devInfo;
	
	/*add by Tangbiao 2005-4-8*/	
	const char *temp[32];
	char phone_temp[128];
	//UaConfiguration::instance()->lock_config();
	for(i=0; i<span->count; i++)
	{
          temp[i]=UaConfiguration::instance()->getPhoneNumberByDeviceName(span->devs[i]->name);
	  strcpy(phone_temp,temp[i]);
	  dev = span->devs[i];
          devInfo = (AssistDeviceInfo *)dev->private_data;
	  devInfo->setPhoneNumber(phone_temp);
	  //cout<<"change DeviceName"<<endl;
	}
	//UaConfiguration::instance()->unlock_config();
	/*add by Tangbiao 2005-4-8*/
	
	for(i=0; i<span->count; i++)
	{
		dev = span->devs[i];
		devInfo = (AssistDeviceInfo *)dev->private_data;
		//cout<<"PhoneNumber:%s"<<devInfo->getPhoneNumber()<<endl;
		if( strcmp(devInfo->getPhoneNumber(), phone)==0)
		{
			return devInfo;
		}
	}
	return 0;
}

int AssistNullDevice::getRtpPort(int deviceId)
{
	int port = 0;

	deviceMutex.lock();
	AssistDevice *devInfo = getAssistDevice( deviceId);
	if(devInfo==0)
	{
		cpLog(LOG_EMERG, "No device with Id as '%d' when getRtpPort", deviceId);
		exit(1);
	}
	else
		port = devInfo->getRtpPort() ;

	deviceMutex.unlock();
	return port;
}

void AssistNullDevice::releaseRtpPort(int deviceId)
{
	deviceMutex.lock();
	AssistDevice *devInfo = getAssistDevice( deviceId);
	if(devInfo==0)
	{
		cpLog(LOG_EMERG, "No device with Id as '%d' when releaseRtpPort", deviceId);
		exit(1);
	}
	else
		devInfo->releaseRtpPort() ;
	deviceMutex.unlock();
}

HardwareStatusType AssistNullDevice::getHardwareStatus(int deviceId)
{
	if(deviceId==AS_DEVICE_NOT_DEFINED)
		return HARDWARE_UNAVAILABLE;
	AssistDevice *devInfo;
	devInfo = getAssistDevice(deviceId);
	if(devInfo==0)
	{
		cpDebug(LOG_DEBUG, "No Assist device with id '%d' exist", deviceId);
		//return HARDWARE_UNAVAILABLE;
		exit(1);
	}
	return devInfo->getHardwareStatus();
}

bool AssistNullDevice::isMyHardware(const SipCallId& callid, int deviceId)
{
	AssistDevice *devInfo;
	devInfo = getAssistDevice(deviceId);
	if(devInfo==0)
	{
		cpDebug(LOG_DEBUG, "No Assist device with id '%d' exist", deviceId);
//		return false;
		exit(1);
	}
	return devInfo->isMyHardware(callid);
}

void AssistNullDevice::providePbxTimerEvent(Sptr<UaHardwareEvent> msg)
{
	AssistPbxDevice  *pbx = (AssistPbxDevice *)getAssistDevice( msg->getDeviceId());
	assert(pbx);
	pbx->pbxTimeoutHandler(msg);
}
#else
static const char* devType[] = 
{
	"PHONE",
	"PBX",
	"PSTN",
	"INVALID",
	"OTHER"
};


char AssistNullDevice::bufstatus[21] = 
{
	'-','-','-','-','-','-','-','-',
	0x20,0x20,
	'-','-','-','-',
	0x20,0x20,
	'-','-',    
	0X20, //0x20, 
	'N'
};

int AssistNullDevice::curline = 0;   
AssistNullDevice::AssistNullDevice( const char* deviceName, as_span_t *_span,
                                Sptr < Fifo < Sptr < SipProxyEvent > > > inputQ,
                                Sptr < Fifo < Sptr < SipProxyEvent > > > outputQ , assist_app_type apptype)
        : ResGwDevice( deviceName, inputQ, outputQ  ),
        appType(apptype)        
{
	int i;
	as_device_t *dev;
	char name[AS_DEVICE_NAME_LENGTH];
	// initialize queues
	sessionQ = outputQ;
	myQ = inputQ;

	span = _span;

	//hookStateOffhook = false;

	DigitTimeout::instance( outputQ, UaConfiguration::instance()->getInitialDigitTimeout(), 
		UaConfiguration::instance()->getInterDigitTimeout() );

	span->pbx_count = UaConfiguration::instance()->getPbxCount();
	if( span->pbx_count > span->count)
		span->pbx_count = span->count;
	
	/* added by wuaiwu, 2005/09/06/07 */
	int ipstn = 10;
	int ihd = 0;
	//int index = 0;
	cpDebug(LOG_EMERG, "---%d", span->count);
	
	for(i=0; i<span->count; i++)
	{
		dev = span->devs[i];
		
		if (dev->phyType == as_pstn_device)
		{
			//index = ipstn;
			bufstatus[ipstn] = 'I';
			ipstn++;
		}
		else
		{
			bufstatus[ihd] = 'I';
			ihd++;
		}
		
		//bufstatus[index] = 'E';
		
		dev->private_data = new AssistDeviceInfo(this, dev, sessionQ);
	}

	for(i=0; i<span->pbx_count; i++)
	{
		sprintf(name, "virtual_device_%d", i);
		bufstatus[i+16] = 'E';
		span->pbx_devs[i] = new AssistPbxDevice( this, name , VIRTUAL_DEVICES_EPOCH+ i, sessionQ);
	}

	span->invalid_dev = new AssistInvalidDevice(this, "invalid_device",  as_invalid_device, AS_DEVICE_NOT_DEFINED, sessionQ);
	myDeviceName = deviceName;

	//Modify by chensir, clear LCD's screen before print.
	//as_lcd_printf(6,1,bufstatus);
	//as_lcd_clear(0);
	as_lcd_printf(5, 1, bufstatus);
	msleep(50);
	
}  // end Constructor


AssistNullDevice::~AssistNullDevice()
{
	as_device_t *dev;
	int i;

	for(i=0; i<span->count; i++)
	{
		dev = span->devs[i];
		delete (AssistDeviceInfo *)dev->private_data;
		dev->private_data = 0;
	}

	for(i=0; i<span->pbx_count; i++)
	{
		delete (AssistPbxDevice *)span->pbx_devs[i];
		span->pbx_devs[i] = 0;
	}

	delete (AssistInvalidDevice *)span->invalid_dev;
} // end 


void* AssistNullDevice::slicHardwareMain(void *param)
{
	fd_set readfds;

	int i;
	as_device_t *dev;
	AS_DEVICE_STATE state;
	
	struct timeval tv;
	int retval;
	int maxFd = 128;  
	
	// reset file descriptor
	FD_ZERO(&readfds);
	addToFdSet(&readfds);
	// process forever on behalf of AssistNullDevice hardware

	tv.tv_sec = 0;
	tv.tv_usec = 500;

	/* monitor the exception fdset with only assist device when unlocked */
	if((retval = select(maxFd, 0, 0, &readfds, &tv)) < 0)
	{
		cpLog( LOG_ERR, "select() returned error %d", retval );
	}
	else
	{
		if(process(&readfds) < 0)
		{
			cpLog( LOG_ERR, "hardware encountered an error" );
			assert(0);
		}
	}

	for(i=0; i< span->count; i++)
	{
		dev = span->devs[i];
		state = span->ops->get_status( dev );
		if(state == AS_DEVICE_STATE_IDLE)
		{
			reportEvent( DeviceEventIdle, dev->id);
			cpDebug(LOG_DEBUG, "device '%s' idle", dev->name);
		}		
		else if(state == AS_DEVICE_STATE_BUSY)
		{
			reportEvent( DeviceEventBusy, dev->id);
			cpDebug(LOG_DEBUG, "device '%s' busy", dev->name);			
		}
	}	
	
	return 0;
}

void* AssistNullDevice::pcmHardwareMain(void *param)
{
	int i;
	as_device_t *dev;
	AS_DEVICE_STATE state;

	vusleep(5000);
	
	for(i=0; i< span->count; i++)
	{
		dev = span->devs[i];
		state = span->ops->check_status( dev );
		if(state == AS_DEVICE_STATE_OFFHOOK )
		{
			if(dev->state == AS_DEVICE_STATE_ONHOOK )
			{
				dev->state = AS_DEVICE_STATE_OFFHOOK;
				reportEvent( DeviceEventHookUp, dev->id );
				cpDebug(LOG_DEBUG, "device '%s' hookup", dev->name);
			}
		}/* end of device in the state of offhook */
		else if(state == AS_DEVICE_STATE_ONHOOK)
		{/* device is in the state of on-hook */
			if(dev->state == AS_DEVICE_STATE_OFFHOOK )
			{
				dev->state = AS_DEVICE_STATE_ONHOOK;
				reportEvent( DeviceEventHookDown, dev->id);
				cpDebug(LOG_DEBUG, "device '%s' hookdown", dev->name);
			}
		}	
		else if(state == AS_DEVICE_STATE_WINKFALSH)
		{
		}

		if(dev->state == AS_DEVICE_STATE_OFFHOOK)
		{
			reportHardwareDtmfEvent( (AssistDevice *) dev->private_data);
		}

		//added by lijie to query if device is busy or idle
		state = span->ops->get_status( dev );
		if(state == AS_DEVICE_STATE_IDLE)
		{
			reportEvent( DeviceEventIdle, dev->id);
			cpDebug(LOG_DEBUG, "device '%s' idle", dev->name);
		}		
		else if(state == AS_DEVICE_STATE_BUSY)
		{
			reportEvent( DeviceEventBusy, dev->id);
			cpDebug(LOG_DEBUG, "device '%s' busy", dev->name);			
		}
	}

	return 0;
}


void* AssistNullDevice::hardwareMain (void* parms)
{
	if(appType == assist_app_slic)
		slicHardwareMain( parms);
	else
		pcmHardwareMain( parms );

	if( myQ->size() > 0 )
	{
		cpDebug(LOG_DEBUG, "Signal from call control" );
		processSessionMsg( myQ->getNext() );
	}

	return 0;
} // end hardwareMain()

int AssistNullDevice::process (fd_set* fd)
{
	int i;
	as_device_t *dev;
	AS_DEVICE_STATE state;

	vusleep(0);  // needed to stabilize hookstate readings


	for(i=0; i< span->count; i++)
	{
		dev = span->devs[i];
		state = span->ops->check_status( dev );
		if(state == AS_DEVICE_STATE_OFFHOOK )
		{
			if(dev->state == AS_DEVICE_STATE_ONHOOK )
			{
				dev->state = AS_DEVICE_STATE_OFFHOOK;
//				if( hookStateOffhook == false )
				{
//					hookStateOffhook = true;
					reportEvent( DeviceEventHookUp, dev->id );
					cpDebug(LOG_DEBUG, "device '%s' hookup", dev->name);
				}
			}
		}/* end of device in the state of offhook */
		else if(state == AS_DEVICE_STATE_ONHOOK)
		{/* device is in the state of on-hook */
			if(dev->state == AS_DEVICE_STATE_OFFHOOK )
			{
				vusleep(20000);  // needed to stabilize hookstate readings
				state = as_device_check_state_on_startup(dev->fd, dev->name);
				if(state == AS_DEVICE_STATE_ONHOOK )
				{
					dev->state = AS_DEVICE_STATE_ONHOOK;
					reportEvent( DeviceEventHookDown, dev->id);
					cpDebug(LOG_DEBUG, "device '%s' hookdown", dev->name);
				}	

			}
		}	
		else if(state == AS_DEVICE_STATE_WINKFALSH)
		{
			reportEvent( DeviceEventFlash, dev->id);
			cpDebug(LOG_DEBUG, "device '%s' winkflash", dev->name);
		}

		if(dev->state == AS_DEVICE_STATE_OFFHOOK)
		{
			reportHardwareDtmfEvent( (AssistDevice *) dev->private_data);
		}

	}

	return 0;
} // end process()


void AssistNullDevice::processRTP ()
{
	int i;
	as_device_t  *dev;
	AssistDeviceInfo *devInfo;

	for(i=0; i<span->count; i++)
	{
		dev = span->devs[i];
		
		devInfo = (AssistDeviceInfo *) dev->private_data ;
		assert(devInfo!=0 );
		devInfo->ProcessRTP();

	}
}

void AssistNullDevice::processPbxRTP ()
{
	int i;
	AssistPbxDevice *dev;

	for(i=0; i<span->pbx_count; i++)
	{
		dev = (AssistPbxDevice *)span->pbx_devs[i];
		assert(dev!=0 );
		dev->ProcessRTP();
	}
}


int AssistNullDevice::addToFdSet (fd_set* fd)
{

	if(appType == assist_app_slic)
	{
		int i;
		for(i=0; i< span->count; i++)
		{
			FD_SET((span->devs[i])->fd,  fd);
		}
	}	
	return 0;
} // addToFdSet()

int AssistNullDevice::audioStart (const HardwareAudioRequest& request,int deviceId)
{
	AssistDevice *dev = getAssistDevice( deviceId);
	if(dev==0)
	{
		cpLog(LOG_EMERG, "No hardware device with Id as '%d'", deviceId);
		exit(1);
	}
	else
		dev->audioStart( request);

	return 0;
} // end audioStart()

int AssistNullDevice::audioStop(int deviceId)
{
	AssistDevice *dev = getAssistDevice( deviceId);
	if(dev==0)
	{
		cpLog(LOG_EMERG, "No hardware device with Id as '%d'", deviceId);
		exit(1);
	}
	else
		dev->audioStop();
	return 0;
} // end audioStop()

/* description: suspend the RTP transmit and receive, and also stop the */
int AssistNullDevice::audioSuspend (int deviceId)
{
	AssistDevice *dev = getAssistDevice( deviceId);

	if(dev==0)
	{
		cpLog(LOG_EMERG, "No hardware device with Id as '%d'", deviceId);
		exit(1);
	}
	else
		dev->audioSuspend();
	return 0;
} // end audioSuspend()

int AssistNullDevice::audioResume (const HardwareAudioRequest& request,int deviceId)
{
	AssistDevice *dev = getAssistDevice(deviceId);

	if(dev ==0)
	{
		cpLog(LOG_EMERG, "No hardware device with Id as '%d'", deviceId);
		exit(1);
	}
	else
		dev->audioResume(request);

	return 0;
} // end audioResume()

void AssistNullDevice::onhookOrFlash(as_device_t *dev)
{
	AS_DEVICE_STATE state;
	vusleep(20000);  // needed to stabilize hookstate readings

	state = span->ops->check_status( dev );
	
	if( state==AS_DEVICE_STATE_ONHOOK )
	{	// user is onhook or doing a flash
		//hookStateOffhook = false;
		// need to wait FLASH_TIMEOUT microsec before reporting onhook
		// if user picks up before the timeout, report flash
		int flashTime = 0;

		//cpDebug(LOG_DEBUG,  "Starting Flash Timer");
		while (flashTime < FLASH_TIMEOUT) // begin testing for flash event
		{
			vusleep(50000);  // sleep for 50,000 microsec (50 ms)
			flashTime += 50000;
			state = span->ops->check_status( dev );
			if(state == AS_DEVICE_STATE_OFFHOOK)
			{
				flashTime = FLASH_TIMEOUT;  // will get us out of while-loop
				//cpDebug(LOG_DEBUG,  "Hardware Event: deviceEventFlash");
				//hookStateOffhook = true;
				reportEvent( DeviceEventFlash, dev->id);
			}
		} // end period for testing for a flash event
		//cpDebug(LOG_DEBUG,  "End Flash Timer");

		//if (hookStateOffhook == false)
			// this must be an actual onhook event
		//{
			//cpDebug(LOG_DEBUG,  "Hardware Event: deviceEventHookDown");
		//	reportEvent( DeviceEventHookDown, dev->id);
		//}
	}
} // end onhookOrFlash

void AssistNullDevice::provideDialToneStart(int deviceId)
{
	cpDebug(LOG_DEBUG, "***  Start dial tone on device with id %d ***" , deviceId);
	AssistDevice *dev = getAssistDevice( deviceId);
	assert(dev != 0 );
	dev->provideDialToneStart();

}

void AssistNullDevice::provideQueryStatus(int deviceId)
{
	cpDebug(LOG_DEBUG, "***  Start query status on device with id %d ***" , deviceId);
	AssistDevice *dev = getAssistDevice( deviceId);
	assert(dev != 0 );
	dev->provideQueryStatus();

}
void AssistNullDevice::provideDialToneStop(int deviceId)
{
	cpDebug(LOG_DEBUG, "***  Stop dial tone  ***" );
	AssistDevice *dev = getAssistDevice( deviceId);
	assert(dev != 0 );
	dev->provideDialToneStop();
}


void AssistNullDevice::provideRingStart(int deviceId)
{
	cpDebug(LOG_DEBUG, "***  Start Ringing  ***" );
	AssistDevice *dev = getAssistDevice( deviceId);
	assert(dev != 0 );
	dev->provideRingStart();
}


void AssistNullDevice::provideRingStop(int deviceId)
{
	cpDebug(LOG_DEBUG, "***  Stop Ringing  ***" );
	AssistDevice *dev = getAssistDevice( deviceId);
	assert(dev != 0 );
	dev->provideRingStop();
}

void AssistNullDevice::provideLocalRingbackStart(int deviceId)
{
	cpDebug(LOG_DEBUG, "***  Start local ringback  ***" );
	AssistDevice *dev = getAssistDevice( deviceId);
	assert(dev != 0 );
	dev->provideLocalRingbackStart();
}


void AssistNullDevice::provideLocalRingbackStop(int deviceId)
{
	cpDebug(LOG_DEBUG, "***  Stop local ringback  ***" );
	AssistDevice *dev = getAssistDevice( deviceId);
	assert(dev != 0 );
	dev->provideLocalRingbackStop();
}

void AssistNullDevice::provideRemoteRingbackStart(int deviceId)
{
	cpDebug(LOG_DEBUG, "***  Start local ringback  ***" );
	AssistDevice *dev = getAssistDevice( deviceId);
	assert(dev != 0 );
	dev->provideRemoteRingbackStart();
}


void AssistNullDevice::provideRemoteRingbackStop(int deviceId)
{
	cpDebug(LOG_DEBUG, "***  Stop local ringback  ***" );
	AssistDevice *dev = getAssistDevice( deviceId);
	assert(dev != 0 );
	dev->provideRemoteRingbackStop();
}

void AssistNullDevice::provideBusyToneStart(int deviceId)
{
	cpDebug(LOG_DEBUG, "***  Start busy tone  ***" );
	AssistDevice *dev = getAssistDevice( deviceId);
	assert(dev != 0 );
	dev->provideBusyToneStart();
}


void AssistNullDevice::provideBusyToneStop(int deviceId)
{
	cpDebug(LOG_DEBUG, "***  Stop busy tone  ***" );
	AssistDevice *dev = getAssistDevice( deviceId);
	assert(dev != 0 );
	dev->provideBusyToneStop();
}


void AssistNullDevice::provideFastBusyToneStart(int deviceId)
{
	cpDebug(LOG_DEBUG, "***  Start fast busy tone  ***" );
	AssistDevice *dev = getAssistDevice( deviceId);
	assert(dev != 0 );
	dev->provideFastBusyToneStart();
}


void AssistNullDevice::provideFastBusyToneStop(int deviceId)
{
	cpDebug(LOG_DEBUG, "***  Stop fast busy tone  ***" );
	AssistDevice *dev = getAssistDevice( deviceId);
	assert(dev != 0 );
	dev->provideFastBusyToneStop();
}


void AssistNullDevice::provideCallWaitingBeepStart(int deviceId)
{
	cpDebug(LOG_DEBUG, "***  Start call waiting beep ***" );
	AssistDevice *dev = getAssistDevice( deviceId);
	assert(dev != 0 );
	dev->provideCallWaitingBeepStart();
}


void AssistNullDevice::provideCallWaitingBeepStop(int deviceId)
{
	cpDebug(LOG_DEBUG, "***  Stop call waiting beep ***" );
	AssistDevice *dev = getAssistDevice( deviceId);
	assert(dev != 0 );
	dev->provideCallWaitingBeepStop();
}


void AssistNullDevice::provideDtmf(DeviceSignalType signal,int deviceId)
{
	cpDebug(LOG_DEBUG, "***   Provide Dtmf  ***" );
	AssistDevice *dev = getAssistDevice( deviceId);
	assert(dev != 0 );
	dev->provideDtmf(signal);
}


void AssistNullDevice::provideCallInfo(string _callerNum, string _calleeNum, string reason)
{
	cpDebug(LOG_DEBUG, "Caller is '%s',\t\tCallee is '%s',\t\treason is '%s'\r\n" , 
		_callerNum.c_str(), _calleeNum.c_str(), reason.c_str() );
}

void AssistNullDevice::provideLocalErrorTone(int deviceId, DeviceSignalType tsignal)
{
	cpDebug(LOG_DEBUG, "***  Start Local Error tone  ***" );
	#if 0
		AssistDevice *dev = getAssistDevice(deviceId);
		assert(dev != 0 );
		dev->provideLocalErrorTone(tsignal);
	#endif 

	char strerr[32] = {0};
	switch (tsignal)
	{
	case DeviceSignalDialErrorTone:
		sprintf(strerr,"%d Dial Error", deviceId);
		break;
	case DeviceSignalConnErrorTone:
		sprintf(strerr, "%d IP or DNS error", deviceId);
		break;
	case DeviceSignalNetDropTone:
		sprintf(strerr, "%d network error", deviceId);
		break;

	default:
		return;
    
	}

	cpDebug(LOG_DEBUG, "*********** - %s", strerr);

	provideFastBusyToneStart(deviceId);
	reportDeviceInfo(deviceId, ACTION_UNKNOW,strerr);

	
}
void AssistNullDevice::reportHardwareDtmfEvent(AssistDevice *dev)
{
	switch(dev->reportDtmfDigit() )
	{
		case '1' /*0x01*/:
			reportEvent( DeviceEventDtmf1, dev->getDeviceId());
			break;
		case '2' /*0x02*/:
			reportEvent(DeviceEventDtmf2, dev->getDeviceId());
			break;
		case '3'/*0x03*/:
			reportEvent( DeviceEventDtmf3, dev->getDeviceId());
			break;
		case '4'/*0x04*/:
			reportEvent( DeviceEventDtmf4, dev->getDeviceId());
			break;
		case '5'/*0x05*/:
			reportEvent( DeviceEventDtmf5, dev->getDeviceId());
			break;
		case '6'/*0x06*/:
			reportEvent( DeviceEventDtmf6, dev->getDeviceId());
			break;
		case '7'/*0x07*/:
			reportEvent( DeviceEventDtmf7, dev->getDeviceId());
			break;
		case '8'/*0x08*/:
			reportEvent( DeviceEventDtmf8, dev->getDeviceId());
			break;
		case '9'/*0x09*/:
		reportEvent( DeviceEventDtmf9, dev->getDeviceId());
			break;
		case '*'/*0x0A*/:
			reportEvent( DeviceEventDtmfStar, dev->getDeviceId());
			break;
		case '0'/*0x0B*/:
			reportEvent( DeviceEventDtmf0, dev->getDeviceId());
			break;
		case '#'/*0x0C*/:
			reportEvent( DeviceEventDtmfHash, dev->getDeviceId());
			break;
		default:
			break;
	}
}

void AssistNullDevice::setCallId( const Sptr<SipCallId> newCallId , int deviceId)
{
	cpDebug(LOG_DEBUG, "setCallId" );
		
	AssistDevice *dev = getAssistDevice( deviceId);
	if(dev != 0)
	{
		dev->setCallId( newCallId);
		return;
	}
	cpDebug(LOG_DEBUG, "No device with deviceId '%d' ", deviceId);
}


Sptr<SipCallId> AssistNullDevice::getCallId( int deviceId) 
{
	cpDebug(LOG_DEBUG, "getCallId" );
	
	AssistDevice *dev = getAssistDevice( deviceId );
	if(dev != 0)
		return dev->getCallId();
	cpDebug(LOG_DEBUG, "No callId in device with deviceId '%d' ", deviceId);
	return 0;
}

void AssistNullDevice::addCallWaitingId( const Sptr<SipCallId> newCallId , int deviceId)
{
	cpDebug(LOG_DEBUG, "addCallWaitingId" );

	AssistDevice *dev = getAssistDevice( deviceId);
	if(dev != 0)
	{
		dev->addCallWaitingId( newCallId);
		return;
	}
	cpDebug(LOG_DEBUG, "No device with deviceId '%d' ", deviceId);
}

Sptr<SipCallId> AssistNullDevice::getCallWaitingId( int deviceId)//; // const
{
	cpDebug(LOG_DEBUG, "getCallWaitingId" );
	
	AssistDevice *dev = getAssistDevice( deviceId);
	
	if(dev != 0)
		return dev->getCallWaitingId();
	
	cpDebug(LOG_DEBUG, "No callId in device with deviceId '%d' ", deviceId);
	return 0;
}

void AssistNullDevice::removeCallWaitingId( const SipCallId& callId , int deviceId)
{
	AssistDevice *dev = getAssistDevice( deviceId);
	
	if(dev != 0)
	{
		dev->removeCallWaitingId( callId);
		return;
	}
	
	cpDebug(LOG_DEBUG, "No device with deviceId '%d' ", deviceId);
}

void AssistNullDevice::setAudioChannel(int destDeviceId, int sourDeviceId)
{
	

	RtpSession* audioStack;
	AssistRtpTransmitter *as_tran;
	RtpPacket* inRtpPkt;
	DTMFInterface* _DTMFInterface;
		
	AssistDevice *destDev = getAssistDevice(destDeviceId);
	AssistDevice *sourDev = getAssistDevice(sourDeviceId);
	assert(destDev != 0);
	assert(sourDev != 0);

	sourDev->audioMutex.lock();
	audioStack = sourDev->audioStack;
	as_tran = sourDev->as_tran;
	inRtpPkt = sourDev->inRtpPkt;
	_DTMFInterface = sourDev->_DTMFInterface;
	sourDev->audioStack = NULL;
	sourDev->as_tran = NULL;
	sourDev->inRtpPkt = NULL;
	sourDev->_DTMFInterface = NULL;
	sourDev->audioMutex.unlock();
	
	destDev->audioMutex.lock();
	destDev->openDeviceFile();
	destDev->audioStack = audioStack;
	destDev->inRtpPkt = inRtpPkt;
	destDev->as_tran = as_tran;
	destDev->_DTMFInterface = _DTMFInterface;
	destDev->audioMutex.unlock();
	
}

bool  AssistNullDevice::isOnCallWaiting( const SipCallId& callId, int deviceId) 
{
	cpDebug(LOG_DEBUG, "isOnCallWaiting" );
	
	AssistDevice *dev = getAssistDevice( deviceId );
	
	if(dev != 0)
		return dev->isOnCallWaiting(callId);
	
	cpDebug(LOG_DEBUG, "No device with deviceId '%d' ", deviceId);
	return false;
}

Sptr<DigitCollector> AssistNullDevice::getDigitCollector(int deviceId) 
{
	cpDebug(LOG_DEBUG, "get DigitCollector for device with ID '%d'", deviceId);
	AssistDevice *dev = getAssistDevice( deviceId );
	assert(dev!=0);
	return dev->getDigitCollector();
}

string  AssistNullDevice::getPhoneNumber(int deviceId)
{
	string num;

	AssistDevice *dev = getAssistDevice( deviceId );
	assert(dev!=0);
	
	return dev->getPhoneNumber();
}

string  AssistNullDevice::getDeviceName(int deviceId)
{
	string num;

	AssistDevice *dev = getAssistDevice( deviceId );
	assert(dev!=0);
	
	return dev->getDeviceName();
}
int AssistNullDevice::getDeviceId( const SipCallId& callId ) 
{
	int i;
	Sptr <SipCallId> myCallId;
	
	for(i=0; i<span->count; i++)
	{
		as_device_t  *dev =span->devs[i];
		AssistDeviceInfo *devInfo = (AssistDeviceInfo *) dev->private_data ;
		myCallId  = devInfo->getCallId();

			
		if(myCallId!=0)
		{
			if( myCallId->getHost() == callId.getHost()
			&& myCallId->getLocalId() == callId.getLocalId() )
			{
				return devInfo->getDeviceId();
			}
		}
		
		if(devInfo->isOnCallWaiting(callId))
			return devInfo->getDeviceId();
		
	}

	return AS_DEVICE_NOT_DEFINED;
	
}

string  AssistNullDevice::getAllTransferUrl(int deviceId)
{
	if(deviceId >= VIRTUAL_DEVICES_EPOCH)
		return "";
	
	AssistDevice *dev = getAssistDevice( deviceId );
	
	if(dev != 0)
		return dev->getAllTransferUrl();
	
	return "";
}

string  AssistNullDevice::getTimeOutTransferUrl(int deviceId)
{
	if(deviceId >= VIRTUAL_DEVICES_EPOCH)
		return "";
	
	AssistDevice *dev = getAssistDevice( deviceId );
	
	if(dev != 0)
		return dev->getTimeOutTransferUrl();
	
	return "";
}

bool AssistNullDevice::getCallWaitingOn(int deviceId)
{
	if(deviceId >= VIRTUAL_DEVICES_EPOCH)
		return false;
	
	AssistDevice *dev = getAssistDevice( deviceId );
	
	if(dev != 0)
		return dev->getCallWaitingOn();
	
	return false;

}

string  AssistNullDevice::getBusyTransferUrl(int deviceId)
{
	if(deviceId >= VIRTUAL_DEVICES_EPOCH)
		return "";
	
	AssistDevice *dev = getAssistDevice( deviceId );
	
	if(dev != 0)
		return dev->getBusyTransferUrl();
	
	return "";
}

int AssistNullDevice::getPbxDeviceId( const SipCallId& callId ) 
{
	int i;
	Sptr <SipCallId> myCallId;

	for(i=0; i<span->pbx_count; i++)
	{
		AssistPbxDevice* dev = (AssistPbxDevice *)span->pbx_devs[i];
		myCallId  = dev->getCallId();
			
		if(myCallId != 0)
		{
			if(callId == *myCallId)
				return dev->getDeviceId();
		}	
	}
	
	return AS_DEVICE_NOT_DEFINED;
}

int AssistNullDevice::getFreePbxDevice( ) 
{
	int i;
	Sptr <SipCallId> myCallId;

	for(i=0; i<span->pbx_count; i++)
	{
		AssistPbxDevice* dev = (AssistPbxDevice *)span->pbx_devs[i];
		cpDebug(LOG_DEBUG, "free is checked on device %d" ,dev->getDeviceId() );
		myCallId = dev->getCallId();
		if(myCallId == 0)
			return dev->getDeviceId();
	}
	
	return AS_DEVICE_NOT_DEFINED;
}

AssistDevice* AssistNullDevice::getAssistDevice(int deviceId)
{
	int i;
	as_device_t  *dev;
	AssistDevice *devInfo;

	if(deviceId == AS_DEVICE_NOT_DEFINED)
		return (AssistDevice *)span->invalid_dev;
	for(i=0; i<span->count; i++)
	{
		dev = span->devs[i];
		if(dev->id == deviceId)
		{
			devInfo = (AssistDevice *)dev->private_data;
			return devInfo;
		}
	}

	for(i=0; i<span->pbx_count; i++)
	{
		devInfo = (AssistDevice *)span->pbx_devs[i];
		if(devInfo->getDeviceId() == deviceId )
		{
			return devInfo;
		}
	}
	return 0;
}

/* only used to lookup hardware device for local dial, so pbx device is not return */
AssistDevice* AssistNullDevice::getAssistDeviceInfo(const char *deviceName)
{
	int i;
	as_device_t  *dev;
	AssistDeviceInfo *devInfo;

	for(i=0; i<span->count; i++)
	{
		dev = span->devs[i];
		if( strcmp(dev->name, deviceName)==0)
		{
			devInfo = (AssistDeviceInfo *)dev->private_data;
			return devInfo;
		}
	}
	return 0;
}

/* only used to lookup hardware device for local dial, so pbx device is not return */
AssistDevice* AssistNullDevice::getAssistDeviceByPhoneNumber(const char *phone, const SipCallId& callid)
{
	

	int i;
	bool bGet = false;
	bool bPstn = false;
	bool bLocal = false;
	as_device_t  *dev;
	AssistDeviceInfo *devGet = NULL;
	AssistDeviceInfo *idleDev = NULL;
	AssistDeviceInfo *devInfo;
	AssistPbxDevice *pbxDev;
	AssistPbxDevice *idlePbx = NULL;
	string url = "";
	DialMethodsType method;

	cpDebug(LOG_DEBUG, "phone number is %s", phone);
	cpDebug(LOG_DEBUG, "sip call id is %s", callid.encode().logData());
	
	method = DigitCollector::checkDialMethod(phone);
	bPstn = (method == PSTN_DIAL);
	url = phone;
	
	Data tmph=callid.getHost();
	bLocal =  (theSystem.gethostAddress(&tmph) == tmph);
	if(method == INTERNAL_IP_DIAL)
	{
		url = UaConfiguration::instance()->getLocalCountry()+UaConfiguration::instance()->getLocalDistrict()+\
			UaConfiguration::instance()->getLocalDevice()+phone;
	}
	
	for(i=0; i<span->count; i++)
	{
		dev = span->devs[i];
		devInfo = (AssistDeviceInfo *)dev->private_data;

		if(strcmp(devInfo->getPhoneNumber(), url.c_str())==0)
		{
			cpDebug(LOG_DEBUG, "LOCAL DEVICE: %s", url.c_str());
			return devInfo;
		}
		
		if(bPstn && devInfo->pstnUrl == url && dev->phyType == as_pstn_device)
		{
			cpDebug(LOG_DEBUG, "pstn as callee");
			return devInfo;
		}
		
		if(isMyHardware(callid, devInfo->getDeviceId())||isOnCallWaiting(callid, devInfo->getDeviceId()))
		{
			devGet = devInfo;
		}
		else
		{
			if(bPstn && dev->phyType == as_pstn_device && !bGet)
			{
				if(devInfo->getHardwareStatus() == HARDWARE_AVAILABLE)
				{
					idleDev = devInfo;
					bGet = true;
				}
			}	
		}
	}


	if(bPstn)
	{
		if(bGet)
		{
			cpDebug(LOG_DEBUG, "get new pstn device");	
			idleDev->pstnUrl = url;
			return idleDev;
		}

		cpDebug(LOG_DEBUG, "All pstn device is busying!");	
		return (AssistDevice*)span->invalid_dev;
	}

	if(bLocal)
		return (AssistDevice*)span->invalid_dev;;
		
	for(i=0; i<span->pbx_count; i++)
	{
		pbxDev = (AssistPbxDevice *)span->pbx_devs[i];

		if(strcmp(pbxDev->getPhoneNumber(), url.c_str())==0)
		{
			cpDebug(LOG_DEBUG, "pbx as caller, invalid action");
			return pbxDev;
		}
		
		if(pbxDev->getHardwareStatus() == HARDWARE_AVAILABLE)
		{
			if(!bGet)
			{
				idlePbx = pbxDev;
				bGet = true;
			}	
		}
		else
		{
			if((isMyHardware(callid, pbxDev->getDeviceId()) ||isOnCallWaiting(callid, pbxDev->getDeviceId()) ))
			{
				cpDebug(LOG_DEBUG, "got pbx device"); 
				return pbxDev;
			}
		}	
	}

	if(devGet)
	{
		cpDebug(LOG_DEBUG, "got hardware device or pstn device in second dial");
		return devGet;
	}
	
	if(bGet)
	{
		cpDebug(LOG_DEBUG, "get new pbx device");
		idlePbx->pbxUrl = url;
		return idlePbx;
	}

	return (AssistDevice *)span->invalid_dev;

}

int AssistNullDevice::getRtpPort(int deviceId)
{
	int port = 0;

	AssistDevice *devInfo = getAssistDevice( deviceId);
	if(devInfo==0)
	{
		cpLog(LOG_EMERG, "No device with Id as '%d' when getRtpPort", deviceId);
		exit(1);
	}
	else
		port = devInfo->getRtpPort();
	
	return port;
}

void AssistNullDevice::releaseRtpPort(int deviceId)
{
	AssistDevice *devInfo = getAssistDevice( deviceId);
	if(devInfo==0)
	{
		cpLog(LOG_EMERG, "No device with Id as '%d' when releaseRtpPort", deviceId);
		exit(1);
	}
	else
		devInfo->releaseRtpPort() ;
}

HardwareStatusType AssistNullDevice::getHardwareStatus(int deviceId)
{
	if(deviceId==AS_DEVICE_NOT_DEFINED)
		return HARDWARE_UNAVAILABLE;
	
	AssistDevice *devInfo;
	devInfo = getAssistDevice(deviceId);
	if(devInfo==0)
	{
		cpDebug(LOG_DEBUG, "No Assist device with id '%d' exist", deviceId);
		return HARDWARE_UNAVAILABLE;
		//exit(1);
	}
	return devInfo->getHardwareStatus();
}

bool AssistNullDevice::isMyHardware(const SipCallId& callid, int deviceId)
{
	AssistDevice *devInfo;
	devInfo = getAssistDevice(deviceId);
	if(devInfo==0)
	{
		cpDebug(LOG_DEBUG, "No Assist device with id '%d' exist", deviceId);
//		return false;
		exit(1);
	}
	return devInfo->isMyHardware(callid);
}

void AssistNullDevice::providePbxTimerEvent(Sptr<UaHardwareEvent> msg)
{
	AssistPbxDevice  *pbx = (AssistPbxDevice *)getAssistDevice( msg->getDeviceId());
	assert(pbx);
	pbx->pbxTimeoutHandler(msg);
}
int AssistNullDevice::getCallCount(int deviceId)
{
	AssistDevice  *dev = (AssistDevice *)getAssistDevice(deviceId);
	assert(dev);

	return dev->getCallCount();
}

void AssistNullDevice::setCallCount(int count, int deviceId)
{
	AssistDevice  *dev = (AssistDevice *)getAssistDevice(deviceId);
	assert(dev);

	dev->setCallCount(count);
}

void AssistNullDevice::incCallCount(int deviceId)
{
	AssistDevice  *dev = (AssistDevice *)getAssistDevice(deviceId);
	assert(dev);

	dev->incCallCount( );
}

void AssistNullDevice::decCallCount(int deviceId)
{
	AssistDevice  *dev = (AssistDevice *)getAssistDevice(deviceId);
	assert(dev);

	dev->decCallCount( );
}

#if 0
void AssistNullDevice::reportDeviceInfo( )
{
	int i;
	int iPhone;
	int iPstn;
	int iPbx;
	char bufPhone[AS_DEVICE_MAX	];
	char bufPstn[AS_DEVICE_MAX];
	char bufPbx[AS_DEVICE_MAX];
	char bufInvalid[2];
	char bufStr[128];
	AssistDevice *dev;


	iPhone = 0;
	iPstn = 0;
	iPbx = 0;
	
	for(i=0; i<span->count; i++)
	{
		dev = (AssistDevice *)(span->devs[i]->private_data);
		if(dev->type == as_pstn_device)
		{
			if(dev->getCallCount()>0)
			{
				bufPstn[iPstn++] = 'B';
				sprintf(bufStr, "%s is busy on %d calls\n", dev->getDeviceName(), dev->getCallCount());
			}
			else
			{
				bufPstn[iPstn++] = 'I';
				sprintf(bufStr, "%s is ilde\n", dev->getDeviceName());
			}
			LCD_SCREEN_WRITE(bufStr);			
		}
		else
		{
			if(dev->getCallCount()>0)
			{
				bufPhone[iPhone++] = 'B';
				sprintf(bufStr, "%s is busy on %d calls\n", dev->getDeviceName(), dev->getCallCount());
			}
			else
			{
				bufPhone[iPhone++] = 'I';
				sprintf(bufStr, "%s is ilde\n", dev->getDeviceName());
			}
			LCD_SCREEN_WRITE(bufStr);			
		}
	}

	bufPstn[iPstn++] = 0;
	bufPhone[iPhone++] = 0;
	
	for(i=0; i<span->pbx_count; i++)
	{
		dev = (AssistDevice *)(span->pbx_devs[i]);
		if(dev->getCallCount()>0)
		{
			bufPbx[iPbx++] = 'B';
			sprintf(bufStr, "%s is busy on %d calls\n", dev->getDeviceName(), dev->getCallCount());
		}	
		else
		{			
			bufPbx[iPbx++] = 'I';
			sprintf(bufStr, "%s is idle\n", dev->getDeviceName());
		}
		LCD_SCREEN_WRITE(bufStr);	
	}

	bufPbx[iPbx++] = 0;

	dev = (AssistDevice *)span->invalid_dev;

	if(dev->getCallCount()>0)
	{
		bufInvalid[0] = 'B';
		sprintf(bufStr, "There are %d invalid calls\n", dev->getCallCount());
	}
	else
	{
		sprintf(bufStr, "There is no invalid call\n");
		bufInvalid[0] = 'I';
	}

	LCD_SCREEN_WRITE(bufStr);
	
	bufInvalid[1] = 0;

	sprintf(bufStr, "%s\t%s\t%s\t%s\n",bufPhone, bufPstn, bufPbx, bufInvalid);
	
	LCD_STATUS_WRITE(bufStr);
}

#endif 
inline int AssistNullDevice::getDevInfoPosition(int deviceType, int deviceId)
{
//	if (deviceType == as_other_device)
//		return  curline = (curline >= 6)? (curline = 1) : ++curline;
	
	int line_ = devInfoPos[deviceType][deviceId];
	if (line_ == 0)
	{
		//Modify by chensir 
		//devInfoPos[deviceType][deviceId] = (curline >= 6)? (curline = 1) : ++curline;
		devInfoPos[deviceType][deviceId] = (curline >= 4)? (curline = 1) : ++curline;
		return curline;
	}
	return line_;
}

  
void AssistNullDevice::reportDeviceInfo(int deviceId, CallAction action, const string& wrstr)
{
	
	//modify by chensir
	//char bufStr[32] = {0};
	char bufStr[64] = {0};
	int line_ = 1;
	AssistDevice *dev;
	int nstatus = -1;
	int devt = as_other_device;
	int index = 0;
	//Added by chensir at 2005.12.01
	static char cls_flg = 0;

	if (cls_flg != 1)
	{
		as_lcd_clear(0);
		cls_flg = (char)1;
	}
	//end of added
	
	if ((deviceId >= 0 && deviceId <=7) ||
		(deviceId >= PSTN_DEVICES_EPOCH && deviceId <= PSTN_DEVICES_EPOCH+3) )
	{
		deviceId = (deviceId >= PSTN_DEVICES_EPOCH)? deviceId-PSTN_DEVICES_EPOCH : deviceId;
		dev = (AssistDevice *)(span->devs[deviceId]->private_data);
	}
	else if (deviceId >= VIRTUAL_DEVICES_EPOCH && deviceId <= VIRTUAL_DEVICES_EPOCH+1)
	{
		deviceId = deviceId - VIRTUAL_DEVICES_EPOCH;
		dev = (AssistDevice *)(span->pbx_devs[deviceId]);
		
	}
	else 
	{
		goto ERROR;
	}
	
	if (dev == NULL)
	{
		goto ERROR;
	}

	if (dev->type > as_pstn_device || dev->type < as_hardware_device)
	{
		goto ERROR;
	}

	devt = (int)(dev->type);

	switch (devt)
	{
	case as_hardware_device:
		nstatus = deviceId;
		break;
	case as_pstn_device: 
		nstatus = deviceId+10;
		break;
	case as_virtual_device:
		nstatus = deviceId +16;
		break;

	case as_other_device:
	case as_invalid_device:
	default:
		nstatus = -1;
		break;
	}

	//cpLog(LOG_EMERG, "form deviceId = %d | devt = %d | nstatus = %d", deviceId, devt, nstatus);
	
	if (nstatus != -1 && nstatus < 20)
	{
		switch (action)
		{
		case ACTION_UNKNOW:
			
			if (wrstr == "Regist Forbidden")
				bufstatus[nstatus] = 'E'; //device error (not registration)
			else if (wrstr == "Regist OK")
				bufstatus[nstatus] = 'I';
			
			break;
			
		default:
			break;			
		}
	}
	
	if (nstatus == -1)
	{
		//Modify by chensir
		//bufstatus[20] = 'E';
		bufstatus[19] = 'E';
		//line_ = (curline >= 6)? 1 : ++curline;
		//as_lcd_printf(line_, 1, "Error: unknow device.");
		line_ = (curline >= 4)? 1 : ++curline;
		as_lcd_clear(line_);
		as_lcd_printf(line_, 1, "Unknow device.");
		msleep(50);
		
		goto ERROR;
	}

			
	line_ = getDevInfoPosition(devt-1,deviceId);
	if (action != ACTION_UNKNOW)
	{
		if(dev->getCallCount()>0)
		{
			//Modify by chensir. msg too long
			//sprintf(bufStr, "%s_%d is on %d calls", devType[devt-1], deviceId, dev->getCallCount());
			sprintf(bufStr, "%s_%d on %d call", devType[devt-1], deviceId, 
			          dev->getCallCount());
			bufstatus[nstatus] = 'B';
		}
		else
		{
			sprintf(bufStr, "%s_%d is idle", devType[devt-1], deviceId);	
			bufstatus[nstatus] = 'I';
		}
		//Modify by chensir
		as_lcd_clear(line_);
		as_lcd_printf(line_, 1, bufStr);
		msleep(60);
	}
	else
	{
		//Modify by chensir
		as_lcd_clear(line_);
		as_lcd_printf(line_,1,wrstr.c_str());
		msleep(50);
	}

	index = 0;
	for (int i = 0; i < span->count; ++i)
	{
		AssistDevice *devpstn = (AssistDevice *)(span->devs[i]->private_data);
		if (devpstn->type != as_pstn_device)continue;
		
		if(devpstn->getCallCount()>0)
		{
			if (bufstatus[index+10] != 'B')
			{
				bufstatus[index+10] = 'B';
				//Modify by chensir. 
				//sprintf(bufStr, "PSTN_%d is on %d calls", i, devpstn->getCallCount());
				sprintf(bufStr, "PSTN_%d on %d call", index, devpstn->getCallCount());
				line_ =  getDevInfoPosition(as_pstn_device-1, index);
				//Modify by chensir
				as_lcd_clear(line_);
				as_lcd_printf(line_,1,bufStr);
				msleep(50);
			}
		}
		else 
		{
			if (bufstatus[index+10] != 'I')
			{
				bufstatus[index+10] = 'I';
				//Modify by chensir. 
				//sprintf(bufStr, "PSTN_%d is idle", i);
				sprintf(bufStr, "PSTN_%d is idle", index);
				line_ =  getDevInfoPosition(as_pstn_device-1, index);
				//Modify by chensir
				as_lcd_clear(line_);
				as_lcd_printf(line_,1,bufStr);
				msleep(50);
			}
		}

		index++;
		
	}

	index = 0;
	for(int i=0; i < span->pbx_count; i++)
	{
		AssistDevice *devpbx = (AssistDevice *)(span->pbx_devs[i]);
		if(devpbx->getCallCount() >0 )
		{
			if (bufstatus[index+16] != 'B')
			{
				bufstatus[index+16] = 'B';
				//Modify by chensir. 
				//sprintf(bufStr, "PBX_%d is on %d calls", i, devpbx->getCallCount());
				sprintf(bufStr, "PBX_%d on %d call", index, devpbx->getCallCount());
				line_ =  getDevInfoPosition(as_virtual_device-1, index);
				//Modify by chensir
				as_lcd_clear(line_);
				as_lcd_printf(line_,1,bufStr);
				msleep(50);
			}
		}
		else
		{
			if (bufstatus[index+16] == 'B')
			{
				bufstatus[index+16] = 'I';
				//Modify by chensir. 
				//sprintf(bufStr, "PBX_%d is idle", i);
				sprintf(bufStr, "PBX_%d is idle", index);
				
				line_ =  getDevInfoPosition(as_virtual_device-1, index);
				//Modify by chensir
				as_lcd_clear(line_);
				as_lcd_printf(line_,1,bufStr);
				msleep(50);
			}
		}

		
		index++;
	}

	//#endif

	//Modify by chensir
	//bufstatus[20] = 'N';
	//as_lcd_printf(6,1,bufstatus);
	bufstatus[19] = 'N';
	as_lcd_clear(5);
	as_lcd_printf(5, 1, bufstatus);
	msleep(50);
	
	return;

ERROR:

	//Modify by chensir
	//bufstatus[20] = 'E';
	//as_lcd_printf(6,1,bufstatus);
	bufstatus[19] = 'E';
	as_lcd_clear(5);
	as_lcd_printf(5,1,bufstatus);
	msleep(50);
}
#endif
//Add by chensir at 2005.12.01



/*
  * msleep() like sleep(), expect unit is millisecond;
  * RETURNS:N/A
  */
void msleep(int m_sec)
{	
	struct  timeval timo;
	
	timo.tv_sec = m_sec/1000;	
	timo.tv_usec = (m_sec%1000)*1000;	
	select(0, NULL, NULL, NULL, &timo);	
	
	return;
}



/*
  * Clear LCD's screen.
  * if line_num > 0,clear the line of line_num.
  * if ine_num =0,clear all.
  * RETURNS: 
     	success > 0
  	failure < 0
  */
int as_lcd_clear(unsigned int line_num)
{
	char buff[21];
	int	y, ret;

	memset(buff, ' ', sizeof(buff));
	buff[sizeof(buff) -1] = 0;
	if ( line_num == 0)
	{
		for (y = 1; y < 6; y++)
		{/* lizhijie, commented for LCD is not supported , 2006.07.13 */
//			ret = as_lcd_printf(y, 1, buff);
			as_lcd_printf(y, 1, buff);
			msleep(50);
		}
	}
	else
	{
//		ret = as_lcd_printf(line_num, 1, buff);
		as_lcd_printf(line_num, 1, buff);
		msleep(50);
	}
	
	return ret;
}
