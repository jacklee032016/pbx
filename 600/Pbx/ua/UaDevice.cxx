/*
 * $Id: UaDevice.cxx,v 1.5 2007/03/28 19:03:39 lizhijie Exp $
 */

#include "global.h"
#include "UaDevice.hxx"

//#include "AssistVoiceDevice.hxx"
#include "AssistNullDevice.hxx"
#ifdef HAS_PHONEDEV
#include "QuickNetDevice.hxx"
#include "PhoneCardDevice.hxx"
#endif
#ifdef HAS_SOUNDCARD
#include "SoundCardDevice.hxx"
#endif

#if 0
/* commentted by lizhijie ,2005.02.11 */
#include "NullHwDevice.hxx"
#endif
#include "UaConfiguration.hxx"
#ifdef HAS_VOICEMAIL
#include "VmcpDevice.hxx"
#endif

#if 0
#if !defined(WIN32)
#include "SimpleExtDevice.hxx"
#endif
#endif
#if 0
#include "DupCardDevice.hxx"
#endif

using namespace Vocal;

///
Sptr < Fifo < Sptr<SipProxyEvent> > > UaDevice::deviceQueue;

///
Sptr<ResGwDevice> UaDevice::uaDevice;

VMutex UaDevice::mutDevice;
bool UaDevice::bExitFlag = false;
bool UaDevice::bCanExit = true;

///
Sptr<ResGwDevice> UaDevice::instance()
{
    assert( uaDevice != 0 );
    return uaDevice;
}  // UaDevice::instance


void UaDevice::instance( const char* useDevice,
                    Sptr < Fifo < Sptr<SipProxyEvent> > > callProcessingQueue )
{
	assert( uaDevice == 0 );

	pthread_t pthid;

	deviceQueue = new Fifo < Sptr<SipProxyEvent> >;
	assert( deviceQueue != 0 );


	if(strcmp(useDevice, "ASSIST") == 0)
	{
		assist_app_type appType= UaConfiguration::instance()->getAppType();
		
		const char *deviceName =  UaConfiguration::instance()->getDeviceName().c_str();
		if(UaConfiguration::instance()->getAppType() == assist_app_slic )
		{
			cpDebug(LOG_DEBUG, "Create Assist SLIC Device %", deviceName );
		}	
		else
		{
			cpDebug(LOG_DEBUG, "Create Assist PCM Device %", deviceName );
		}
#if 0
		uaDevice = new AssistVoiceDevice( deviceName, deviceQueue,  callProcessingQueue );
#endif
		as_span_t *span = as_check_hardware(deviceName, appType, UaConfiguration::instance()->getG711Law(), 
			UaConfiguration::instance()->getPstnCount(), UaConfiguration::instance()->getLogLevel() );

		if( UaConfiguration::instance()->getAppType() == assist_app_pcm )
			pthread_create(&pthid, NULL, pcm_Thread, NULL);
//		else 
//			pthread_create(&pthid, NULL, slic_Thread, NULL);

		uaDevice = new AssistNullDevice("PBX virtual device", span,  deviceQueue,  callProcessingQueue, appType);
		assert( uaDevice != 0 );	
	}


#ifdef HAS_VOICEMAIL
    if(strcmp(useDevice, "VOICEMAIL") == 0)
    {
        cpDebug(LOG_DEBUG, "Create Voicemail Device" );
        uaDevice = new VmcpDevice( "vm", 8024, deviceQueue,
                                   callProcessingQueue );
        assert( uaDevice != 0 );
    }
#endif

} // UaDevice::instance


///
Sptr < Fifo < Sptr<SipProxyEvent> > >
UaDevice::getDeviceQueue()
{
    return deviceQueue;
}    // UaDevice::instance


///
UaDevice::UaDevice()
{}    // UaDevice::UaDevice

UaDevice::~UaDevice()
{}    // UaDevice::~UaDevice

//added by lijie 2005-07-07
void UaDevice::LockDevice() 
{
	mutDevice.lock();
}
void UaDevice::UnlockDevice() 
{
	mutDevice.unlock();
}

void UaDevice::setExitFlag(bool bExit) 
{
	bExitFlag = bExit;
}

bool UaDevice::getExitFlag() 
{
	return	bExitFlag;
}

void UaDevice::setCanExit(bool bExit)
{
	bCanExit = bExit;
}

bool UaDevice::isCanExit()
{
	return	bCanExit;
}

//added ends here
