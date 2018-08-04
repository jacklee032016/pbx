/*
 * $Id: OpDeviceBusy.cxx,v 1.3 2007/03/03 18:39:27 lizhijie Exp $
 */

#include "global.h"
#include "OpDeviceBusy.hxx"
#include "ResGwDevice.hxx"

#include "UaDeviceEvent.hxx"
#include "UaStateMachine.hxx"
#include "UaCallInfo.hxx"
#include "UaDevice.hxx"
#include "UaConfiguration.hxx"
#include "SdpHandler.hxx"
#include "Sdp2Media.hxx"
#include "SystemInfo.hxx"
#include "SipSdp.hxx"
#include "UaCallContainer.hxx"

#include "pbx_globals.h"

using namespace Vocal;


OpDeviceBusy::OpDeviceBusy()
{
}


OpDeviceBusy::~OpDeviceBusy()
{
}


const char* const
OpDeviceBusy::name() const
{
    return "OpDeviceBusy";
}


const Sptr < State >
OpDeviceBusy::process( const Sptr < SipProxyEvent > event )
{
    cpDebug(LOG_DEBUG, "OpDeviceBusy operation" );

    Sptr < UaDeviceEvent > deviceEvent;
	
    deviceEvent.dynamicCast( event );
    if ( deviceEvent == 0 )
    {
        return 0;
    }
	
    if ( deviceEvent->type != DeviceEventBusy )
    {
        return 0;
    }

    Sptr < UaCallInfo > call;
    call.dynamicCast( event->getCallInfo() );
    assert( call != 0 );

    Sptr < InviteMsg > msg = call->getRingInvite();
    assert( msg != 0 );

    //Sptr < UaCallContainer > calls;
    //calls.dynamicCast( deviceEvent->getCallContainer() );
   // assert( calls != 0 );

	StatusMsg status( *msg, 486 );

	 deviceEvent->getSipStack()->sendReply( status );

//removed by lijie 2005-06-03   

#if 0	
      calls->deleteCall( *(UaDevice::instance()->getCallId( event->getDeviceId() )) ,event->getDeviceId());
	
    // Clear call id in device
    UaDevice::instance()->setCallId( 0 ,event->getDeviceId() );
#endif

    Sptr < UaStateMachine > stateMachine;
    stateMachine.dynamicCast( event->getCallInfo()->getFeature() );
    assert( stateMachine != 0 );
	
    call->setCalleeIdle(false);

    return stateMachine->findState( "StateIdle" );
	
}

