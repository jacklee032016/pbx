/*
 * $Id: OpCwPbxRing.cxx,v 1.2 2007/03/02 04:19:28 lizhijie Exp $
 */

#include "global.h"
#include "OpCwPbxRing.hxx"
#include "UaHardwareEvent.hxx"
#include "UaCallInfo.hxx"
#include "UaDevice.hxx"
#include "UaStateMachine.hxx"
#include "UaConfiguration.hxx"


///
using namespace Vocal;


OpCwPbxRing::OpCwPbxRing()
{}


OpCwPbxRing::~OpCwPbxRing()
{}


///

const char* const
OpCwPbxRing::name() const
{
    return "OpCwPbxRing";
}


///
const Sptr < State >
OpCwPbxRing::process( const Sptr < SipProxyEvent > event )
{
    cpDebug(LOG_DEBUG, "OpCwPbxRing operation" );

    Sptr < UaDeviceEvent > deviceEvent;
    deviceEvent.dynamicCast( event );
    if ( deviceEvent == 0 )
    {
        return 0;
    }
    if ( deviceEvent->type != DeviceEventHookDown )
    {
        return 0;
    }

    cpDebug(LOG_DEBUG, "Convert call waiting beep to actual ringing" );

    int deviceId = UaDevice::instance()->getDeviceId(*(deviceEvent->callId));
	
    Sptr < UaCallInfo > call;
    call.dynamicCast( event->getCallInfo() );
    assert( call != 0 );

    if(call->getCalleeIdle())
    {
    	Sptr < UaHardwareEvent > signal = new UaHardwareEvent( UaDevice::getDeviceQueue() );
    	signal->type = HardwareSignalType;
	signal->setDeviceId(event->getDeviceId());
	signal->signalOrRequest.signal = DeviceSignalBusyStart;
     	UaDevice::getDeviceQueue()->add( signal );
    }
    Sptr < SipCallId > call2Id = UaDevice::instance()->getCallWaitingId(deviceId);
	
    if ( call2Id != 0 )
    {
        Sptr < SipCallId > callId = deviceEvent->callId;
        if ( *callId == *call2Id )
        {
		//grab the hardware and remove itself from call waiting list
		UaDevice::instance()->setCallId( call2Id , deviceId );
		UaDevice::instance()->removeCallWaitingId( *call2Id, deviceId );

             // stop cal waiting beep
             Sptr < UaHardwareEvent > signalCwbeep = new UaHardwareEvent( UaDevice::getDeviceQueue() );
             signalCwbeep->type = HardwareSignalType;
             signalCwbeep->signalOrRequest.signal = DeviceSignalStopCallWaitingBeep;

		signalCwbeep->setDeviceId( deviceId );
			
            UaDevice::getDeviceQueue()->add( signalCwbeep );

		//virtual device stop send call waiting	
		Sptr < UaHardwareEvent > signal = new UaHardwareEvent( UaDevice::getDeviceQueue() );
             signal->type = HardwareSignalType;
             signal->signalOrRequest.signal = DeviceSignalStopCallWaitingBeep;

		signal->setDeviceId( event->getDeviceId() );
			
            UaDevice::getDeviceQueue()->add( signal );

            // ring the status of the phone
            Sptr < UaHardwareEvent > signalRing = new UaHardwareEvent( UaDevice::getDeviceQueue() );
            signalRing->type = HardwareSignalType;
	      signalRing->signalOrRequest.signal = DeviceSignalQueryStatus;
	     signalRing->setDeviceId( deviceId );
			
            UaDevice::getDeviceQueue()->add( signalRing );
	}
        else
            cpLog( LOG_ERR, "The call on call waiting is not this call" );
    }
    else
        cpLog( LOG_ERR, "No call on cal waiting" );

    return 0;
}
