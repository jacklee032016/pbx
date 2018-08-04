/*
 * $Id: OpFarEndAnswered.cxx,v 1.4 2007/05/06 09:48:54 lizhijie Exp $
 */

#include "global.h"
#include "SipEvent.hxx"
#include "AckMsg.hxx"

#include "OpFarEndAnswered.hxx"
#include "UaStateMachine.hxx"
#include "UaCallInfo.hxx"
#include "UaDevice.hxx"
#include "UaConfiguration.hxx"
#include "RsvpHandler.hxx"

#ifndef NO_POLICY
#include "PepAgent.hxx"
#endif


using namespace Vocal;


OpFarEndAnswered::OpFarEndAnswered()
{
}


OpFarEndAnswered::~OpFarEndAnswered()
{
}


const char* const
OpFarEndAnswered::name() const
{
    return "OpFarEndAnswered";
}


const Sptr < State >
OpFarEndAnswered::process( const Sptr < SipProxyEvent > event )
{
    cpDebug(LOG_DEBUG, "OpFarEndAnswered operation" );

    Sptr < SipEvent > sipEvent;
    sipEvent.dynamicCast( event );
    if ( sipEvent == 0 )
    {
        return 0;
    }
    Sptr < SipMsg > sipMsg = sipEvent->getSipMsg();
    assert( sipMsg != 0 );

    Sptr < StatusMsg > msg;
    msg.dynamicCast( sipMsg );
    if ( msg == 0 )
    {
        return 0;
    }

    if(msg->getStatusLine().getStatusCode() > 200)
    {
	return 0;
    }

    Sptr <UaCallInfo> call;
    call.dynamicCast( event->getCallInfo() );
    assert( call != 0 );

    Sptr<Contact> findContact = call->findContact( *msg );
    cpDebug(LOG_DEBUG, "findContact = %s\n", findContact->computeCallLeg().encode().logData() );

    Sptr<Contact> getContact = call->getContact();
    cpDebug(LOG_DEBUG, "getContact = %s\n", getContact->computeCallLeg().encode().logData() );

    if ( *( call->findContact( *msg ) ) != *( call->getContact() ) )
    {
        cpDebug(LOG_DEBUG, "Not the current call" );
        return 0;
    }

    call->getContact()->update( *msg);

    int status = msg->getStatusLine().getStatusCode();

    if ( status >= 200 )
    {
        // Needs to ack farend
        AckMsg ack( *msg );
        sipEvent->getSipStack()->sendAsync( ack );
    }

    if ( status != 200 )
    {
        return 0;  // Done
    }

    //save the route

    call->setCallerRoute1List( msg->getrecordrouteList() );
    int numContact = msg->getNumContact();
    if ( numContact )
    {
        SipContact contact = msg->getContact( numContact - 1 );
        Sptr < SipRoute > route = new SipRoute;
        route->setUrl( contact.getUrl() );
        call->addRoute1( route );
    }

    // Get remote SDP from message

    Sptr<SipSdp> sdp;
    sdp.dynamicCast ( sipMsg->getContentData(0) );
    if ( sdp == NULL )
    {
        cpLog( LOG_ERR, "No remote SDP in 200" );
    }
    else
    {
        call->setRemoteSdp( new SipSdp( *sdp ) );
    }

    // RSVP stuff ported from old OpFarEndAnswered.cxx

    Sptr < SipSdp > localSdp = call->getLocalSdp();
    Sptr < SipSdp > remoteSdp = call->getRemoteSdp();
    rsvpFarEndAnswered(localSdp, remoteSdp);


#ifndef NO_POLICY
    if (UaConfiguration::instance()->getUsePolicyServer() )
    {
        cpDebug(LOG_DEBUG,"Enable QoS handled in Marshal");
    }
#endif

    Sptr < UaStateMachine > stateMachine;
    stateMachine.dynamicCast( event->getCallInfo()->getFeature() );
    assert( stateMachine != 0 );

    return stateMachine->findState( "StateInCall" );
}

