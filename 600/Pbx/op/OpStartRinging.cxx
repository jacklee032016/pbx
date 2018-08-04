/*
 * $Id: OpStartRinging.cxx,v 1.2 2007/03/02 04:19:28 lizhijie Exp $
 */

#include "global.h"
#include <cassert>
#include <cpLog.h>
#include "OpStartRinging.hxx"
#include "UaHardwareEvent.hxx"
#include "UaDevice.hxx"
#include "UaCallInfo.hxx"


///
using namespace Vocal;


OpStartRinging::OpStartRinging()
{}


///

const Sptr < State >
OpStartRinging::process( const Sptr < SipProxyEvent > event )
{
    cpDebug(LOG_DEBUG, "Start Ringing" );

    Sptr < UaCallInfo > call;
    call.dynamicCast( event->getCallInfo() );
    assert( call != 0 );

    Sptr < UaHardwareEvent > signal =
	new UaHardwareEvent( UaDevice::getDeviceQueue() );
    signal->type = HardwareSignalType;

	signal->setDeviceId( event->getDeviceId() );
	if ( UaDevice::instance()->isMyHardware(
		call->getContact()->getInviteMsg().getCallId(), event->getDeviceId() ) )
    {
        signal->signalOrRequest.signal = DeviceSignalRingStart;
    }
    else
    {
        signal->signalOrRequest.signal = DeviceSignalStartCallWaitingBeep;
    }

    UaDevice::getDeviceQueue()->add( signal );

    return 0;
}


///
OpStartRinging::~OpStartRinging()
{}


const char* const
OpStartRinging::name() const
{
    return "OpStartRinging";
}
