/*
 * $Id: OpTimeOver.cxx,v 1.3 2007/03/07 18:24:08 lizhijie Exp $
 */

#include "global.h"
#include "OpDeviceBusy.hxx"
#include "ResGwDevice.hxx"
#include "UaCallInfo.hxx"
#include "UaDevice.hxx"
#include "UaDeviceEvent.hxx"
#include "UaStateMachine.hxx"
#include "UaCallInfo.hxx"
#include "UaDevice.hxx"
#include "UaConfiguration.hxx"
#include "SdpHandler.hxx"
#include "Sdp2Media.hxx"
#include "UaTimerEvent.hxx"
#include "SystemInfo.hxx"
#include "SipSdp.hxx"
#include "UaCallContainer.hxx"
#include "OpTimeOver.hxx"
#include "pbx_globals.h"

using namespace Vocal;

OpTimeOver::OpTimeOver()
{}


OpTimeOver::~OpTimeOver()
{}


const char* const OpTimeOver::name() const
{
    return "OpTimeOver";
}


///
const Sptr < State >
OpTimeOver::process( const Sptr < SipProxyEvent > event )
{
    cpDebug(LOG_DEBUG, "OpTimeOver operation" );

    /*if ( isTimeout(event) == false )
        return 0;*/

    Sptr < UaTimerEvent > uaTimerEvent;
    uaTimerEvent.dynamicCast(event);
	if (uaTimerEvent == 0) return 0;
	
    assert(uaTimerEvent != 0);
	
    Sptr < UaCallInfo > call;
    call.dynamicCast( event->getCallInfo() );
    assert( call != 0 );

    Sptr < Contact > contact = call->getContact();
    assert( contact != 0 );

    Sptr < InviteMsg > msg = call->getRingInvite();
    assert( msg != 0 );

    Sptr < UaStateMachine > stateMachine;
    stateMachine.dynamicCast( event->getCallInfo()->getFeature() );
    assert( stateMachine != 0 );
			
   if (UaDevice::instance()->isMyHardware(msg->getCallId(), event->getDeviceId())) 
   {
 
	StatusMsg status( *msg, 487 );
      uaTimerEvent->getSipStack()->sendReply( status );
      	return stateMachine->findState( "StateIdle" );
		
   }
   else
   {
	 return 0;
   }

}
