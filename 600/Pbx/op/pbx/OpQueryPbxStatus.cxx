/*
 * $Id: OpQueryPbxStatus.cxx,v 1.2 2007/03/02 04:19:28 lizhijie Exp $
 */

#include "global.h"
#include <cassert>
#include <cpLog.h>
#include "OpQueryPbxStatus.hxx"
#include "SipEvent.hxx"
#include "UaHardwareEvent.hxx"
#include "UaDevice.hxx"
#include "UaCallInfo.hxx"
#include "UaDigitTimerEvent.hxx"

///
using namespace Vocal;


OpQueryPbxStatus::OpQueryPbxStatus()
{}


///

const Sptr < State >
OpQueryPbxStatus::process( const Sptr < SipProxyEvent > event )
{
    cpDebug(LOG_DEBUG, "Start Query Pbx Status" );

    Sptr < InviteMsg > msg;
	
     Sptr < UaDigitTimerEvent > timerEvent;

     timerEvent.dynamicCast( event );
     if ( timerEvent == 0 )
     {
	 return 0;
     }
	 
    Sptr < UaCallInfo > call;
    call.dynamicCast( event->getCallInfo() );
    assert( call != 0 );

    msg = call->getRingInvite();
    assert( msg != 0 );
			
    Sptr < UaHardwareEvent > signal =
    new UaHardwareEvent( UaDevice::getDeviceQueue() );
    signal->type = HardwareSignalType;

     int deviceId =UaDevice::instance()->getDeviceId( *(timerEvent->callId));

	signal->setDeviceId(deviceId );
		
	if (UaDevice::instance()->isMyHardware(*(timerEvent->callId), deviceId))
   	{
		signal->signalOrRequest.signal = DeviceSignalQueryStatus;
		UaDevice::getDeviceQueue()->add( signal );
	}
	else
	{
		signal->signalOrRequest.signal = DeviceSignalStartCallWaitingBeep;
		UaDevice::getDeviceQueue()->add( signal );

		Sptr < UaHardwareEvent > signal2 =
    		new UaHardwareEvent( UaDevice::getDeviceQueue() );
    		signal2->type = HardwareSignalType;		
		signal2->setDeviceId(event->getDeviceId());
		signal2->signalOrRequest.signal = DeviceSignalStartCallWaitingBeep;
		UaDevice::getDeviceQueue()->add( signal2 );
	}

	call->setCalleeIdle(false);

	event->setDeviceId(deviceId);
	return 0;	
     
}


///
OpQueryPbxStatus::~OpQueryPbxStatus()
{}


const char* const
OpQueryPbxStatus::name() const
{
    return "OpQueryPbxStatus";
}
