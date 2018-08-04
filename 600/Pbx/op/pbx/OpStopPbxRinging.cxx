/*
 * $Id: OpStopPbxRinging.cxx,v 1.2 2007/03/02 04:19:28 lizhijie Exp $
 */

#include "global.h"
#include <cassert>
#include <cpLog.h>
#include <SipEvent.hxx>
#include <CancelMsg.hxx>
#include "OpStopPbxRinging.hxx"
#include "SdpHandler.hxx"
#include "UaHardwareEvent.hxx"
#include "UaDevice.hxx"
#include "UaCallContainer.hxx"
#include "UaCallInfo.hxx"
#include "UaTimerEvent.hxx"
#include "pbx_globals.h"
#include "UaDevice.hxx"

///
using namespace Vocal;


OpStopPbxRinging::OpStopPbxRinging()
{}


///
const Sptr < State >
OpStopPbxRinging::process( const Sptr < SipProxyEvent > event )
{
    cpDebug(LOG_DEBUG, "Stop Pbx Ringing" );

    // figure out if the call is going to be cancelled or become active
    bool callCancel = true;
	
    Sptr < UaCallContainer > calls;
    calls.dynamicCast( event->getCallContainer() );
     assert( calls != 0 );
	 
     Sptr < UaCallInfo > call;
    call.dynamicCast( event->getCallInfo() );
    assert( call != 0 );

    Sptr < UaHardwareEvent > signal = new UaHardwareEvent( UaDevice::getDeviceQueue() );
    signal->type = HardwareSignalType;

    SipCallId callId = call->getContact()->getInviteMsg().getCallId();
    int deviceId = UaDevice::instance()->getDeviceId( callId);  
    signal->setDeviceId(deviceId);	   


    Sptr < UaDeviceEvent > deviceEvent;
    deviceEvent.dynamicCast( event );

	if(deviceEvent !=0)
	{
		if ( deviceEvent->type == DeviceEventHookUp )
		{
			event->setDeviceId(deviceId);
			callCancel = false;
		}
	}
	

   if ( UaDevice::instance()->isMyHardware( callId, deviceId) )
    {
	//added by lijie
	//fix, just refer to OpStopRing.cxx ~tr
	signal->signalOrRequest.signal = DeviceSignalRingStop;
	
        if ( callCancel )
        {
	       calls->deleteCall( callId, deviceId );
		UaDevice::instance()->setCallId( 0 , deviceId );
		stopAudio(event->getDeviceId());
        }		
    }
    else
    {
        signal->signalOrRequest.signal = DeviceSignalStopCallWaitingBeep;
        if ( callCancel )
        {
	       calls->deleteCall( callId, deviceId );
		UaDevice::instance()->removeCallWaitingId( callId, deviceId);
		stopAudio(event->getDeviceId());
        }		
    }

    UaDevice::getDeviceQueue()->add( signal );

    return 0;
}


///
OpStopPbxRinging::~OpStopPbxRinging()
{}


///
const char* const
OpStopPbxRinging::name() const
{
    return "OpStopPbxRinging";
}
