/*
 * $Id: OpStopPbxDialing.cxx,v 1.2 2007/03/02 04:19:28 lizhijie Exp $
 */

#include "global.h"
#include <cassert>
#include <cpLog.h>
#include <SipEvent.hxx>
#include <ByeMsg.hxx>
#include <CancelMsg.hxx>
#include "OpStopPbxDialing.hxx"
#include "UaHardwareEvent.hxx"
#include "UaDevice.hxx"
#include "UaCallInfo.hxx"
#include "UaCallContainer.hxx"
#include "UaTimerEvent.hxx"
#include "pbx_globals.h"
#include "SdpHandler.hxx"

///
using namespace Vocal;


OpStopPbxDialing::OpStopPbxDialing()
{}


///
const Sptr < State >
OpStopPbxDialing::process( const Sptr < SipProxyEvent > event )
{
    cpDebug(LOG_DEBUG, "Stop Pbx Ringing" );

    // figure out if the call is going to be cancelled or become active
    bool callCancel = false;
	
    Sptr < UaCallContainer > calls;
    calls.dynamicCast( event->getCallContainer() );
     assert( calls != 0 );
	 
    Sptr < UaDeviceEvent > deviceEvent;
    deviceEvent.dynamicCast( event );

    if(deviceEvent !=0)
    {
    	if ( deviceEvent->type == DeviceEventHookDown)
	{
		callCancel = true;
	}
    }

    Sptr < SipEvent > sipEvent;
    sipEvent.dynamicCast( event );
    if ( sipEvent != 0 )
    {
        Sptr < SipMsg > sipMsg = sipEvent->getSipMsg();
        assert( sipMsg != 0 );
	 Sptr < ByeMsg > msg;
    	 msg.dynamicCast( sipMsg );
        if ( msg != 0 )
    	{
		callCancel = true;
    	}	
    }
   

  
	
    Sptr < UaCallInfo > call;
    call.dynamicCast( event->getCallInfo() );
    assert( call != 0 );

    SipCallId callId = call->getContact()->getInviteMsg().getCallId();

    if(callCancel)
    {
	stopAudio(event->getDeviceId());		
    }
	
    return 0;
}


///
OpStopPbxDialing::~OpStopPbxDialing()
{}


///
const char* const
OpStopPbxDialing::name() const
{
    return "OpStopPbxDialing";
}
