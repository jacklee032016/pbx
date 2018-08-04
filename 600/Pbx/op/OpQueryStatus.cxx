/*
 * $Id: OpQueryStatus.cxx,v 1.2 2007/03/02 04:19:28 lizhijie Exp $
 */

#include "global.h"
#include <cassert>
#include <cpLog.h>
#include "OpQueryStatus.hxx"
#include "SipEvent.hxx"
#include "UaHardwareEvent.hxx"
#include "UaDevice.hxx"
#include "UaCallInfo.hxx"


///
using namespace Vocal;


OpQueryStatus::OpQueryStatus()
{}


///

const Sptr < State >
OpQueryStatus::process( const Sptr < SipProxyEvent > event )
{
    cpDebug(LOG_DEBUG, "Start Query Status" );

    Sptr < SipEvent > sipEvent;
    sipEvent.dynamicCast( event );
    if ( sipEvent == 0 )
    {
        return 0;
    }
	
    Sptr < SipMsg > sipMsg = sipEvent->getSipMsg();
    assert( sipMsg != 0 );

    Sptr < InviteMsg > msg;
    msg.dynamicCast( sipMsg );
    if ( msg == 0 )
    {
        cpDebug(LOG_DEBUG, "Not an INVITE" );
        return 0;
    }

	
    Sptr < UaCallInfo > call;
    call.dynamicCast( event->getCallInfo() );
    assert( call != 0 );
	
   	Sptr < UaHardwareEvent > signal =
	new UaHardwareEvent( UaDevice::getDeviceQueue() );
    signal->type = HardwareSignalType;
		signal->setDeviceId( event->getDeviceId() );

	    call->setCalleeIdle(false);
	
   if (UaDevice::instance()->isMyHardware(msg->getCallId(), event->getDeviceId()))
   {
        signal->signalOrRequest.signal = DeviceSignalQueryStatus;
   }
   else
   {
		StatusMsg status(*msg, 182);

  	    	event->getSipStack()->sendReply( status );
		signal->signalOrRequest.signal = DeviceSignalStartCallWaitingBeep;
   }
     
    UaDevice::getDeviceQueue()->add( signal );

	
    return 0;
}


///
OpQueryStatus::~OpQueryStatus()
{}


const char* const
OpQueryStatus::name() const
{
    return "OpQueryStatus";
}
