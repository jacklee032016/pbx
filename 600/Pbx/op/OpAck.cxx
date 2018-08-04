/*
 * $Id: OpAck.cxx,v 1.3 2007/03/03 18:39:27 lizhijie Exp $
 */

#include "global.h"
#include "SipEvent.hxx"

#include "OpAck.hxx"
#include "UaDeviceEvent.hxx"
#include "UaDevice.hxx"
#include "UaCallInfo.hxx"
#include "UaConfiguration.hxx"
#ifdef HAS_RSVPLIB
#include "SuaRsvp.hxx"
#endif
#include "SdpHandler.hxx"
#include "RsvpHandler.hxx"

#include "UaStateMachine.hxx"


using namespace Vocal;


OpAck::OpAck()
{
}


OpAck::~OpAck()
{
}


const char* const
OpAck::name() const
{
    return "OpAck";
}


const Sptr < State >
OpAck::process( const Sptr < SipProxyEvent > event )
{
    cpDebug(LOG_DEBUG, "OpAck operation" );

    Sptr < SipEvent > sipEvent;
    sipEvent.dynamicCast( event );
    if ( sipEvent == 0 )
    {
        return 0;
    }
    Sptr < SipMsg > sipMsg = sipEvent->getSipMsg();
    assert( sipMsg != 0 );

    if ( sipMsg->getType() != SIP_ACK )
    {
        return 0;
    }

    Sptr < UaCallInfo > call;
    call.dynamicCast( event->getCallInfo() );
    assert( call != 0 );

    // Set ACK in call
    Sptr< AckMsg > ack;
    ack.dynamicCast( sipMsg );
    // We can assert here, because we checked for SIP_ACK above.
    assert( ack != 0 );
    call->setAckMsg( ack );

   Sptr < UaStateMachine > stateMachine;
    stateMachine.dynamicCast( event->getCallInfo()->getFeature() );
    assert( stateMachine != 0 );

    // Get remote SDP from call info
    Sptr < SipSdp > remoteSdp = call->getRemoteSdp();
    if ( remoteSdp != 0 ) // && remoteSdp->isValidSdp() )
    {
        // TODO For now, do nothing regardless if ACK contains sdp.
        // TODO In the future, if ACK has sdp, we should stop the previous
        // TODO media stream and start a new one with the new sdp.
    }
    else
    {
        // Get remote SDP from message
        Sptr<SipSdp> sdp;
	 sdp.dynamicCast ( sipMsg->getContentData(0) );
        if ( sdp == NULL )
        {
		cpLog( LOG_ERR, "No remote SDP in INVITE and ACK" );

		// TODO: add Accept Media type and arrange with UAc
		StatusMsg Unsupported(*ack,415);
		sipEvent->getSipStack()->sendReply( Unsupported );
		return stateMachine->findState( "StateError");
		
        }
        else 
        {
            remoteSdp = new SipSdp( *sdp );
            call->setRemoteSdp( remoteSdp );
		
        }
    }


	// TODO: check the remote sdp media, return 415 if not support 

    rsvpAckHandler(call->getLocalSdp(), call->getRemoteSdp());

   
   /* if(event->getDeviceId() >= VIRTUAL_DEVICES_EPOCH )
    {
		return stateMachine->findState( "StatePbxDialing" );
    }
    else 
    {
		return stateMachine->findState( "StateInCall" );
    }*/

    return 0;
}

