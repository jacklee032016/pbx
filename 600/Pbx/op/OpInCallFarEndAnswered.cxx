/*
 * $Id: OpInCallFarEndAnswered.cxx,v 1.3 2007/03/03 18:39:27 lizhijie Exp $
 */

#include "global.h"
#include "SipEvent.hxx"
#include "AckMsg.hxx"
#include "OpInCallFarEndAnswered.hxx"
#include "OpFarEndAnswered.hxx"
#include "UaStateMachine.hxx"
#include "UaCallInfo.hxx"
#include "SipTransceiver.hxx"
#include "SipSdp.hxx"
#include "SystemInfo.hxx"


using namespace Vocal;


OpInCallFarEndAnswered::OpInCallFarEndAnswered()
{
}


OpInCallFarEndAnswered::~OpInCallFarEndAnswered()
{
}


const char* const
OpInCallFarEndAnswered::name() const
{
    return "OpInCallFarEndAnswered";
}


const Sptr < State >
OpInCallFarEndAnswered::process( const Sptr < SipProxyEvent > event )
{
    cpDebug(LOG_DEBUG, "OpInCallFarEndAnswered operation" );

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

    Sptr < UaCallInfo > call;
    call.dynamicCast( event->getCallInfo() );
    assert( call != 0 );

    if ( !(call->findContact( *msg ) == call->getContact()) )
    {
        return 0;
    }

    call->getContact()->update( *msg);

    int status = msg->getStatusLine().getStatusCode();

    if ( status == 200 || status >= 400 )
    {
        // Needs to ack farend
        cpDebug(LOG_DEBUG, "sending an Ack");
        AckMsg ack( *msg );
        sipEvent->getSipStack()->sendAsync( ack );
    }

    if ( status != 200 )
    {
        return 0;  // Done
    }

    // check to see if this is contact1.  this could happen if you did a
    // flash while in StateInCallTrying.  in this case, OpInCallStopTrying
    // would be called and contact2 cancelled.
    // the current contact is always set to 

    Sptr < Contact > contact = call->getContact();
    assert( contact != 0 );

    Sptr < InviteMsg > invite2 = call->getRing2Invite();
    if ( invite2 == 0 )
    {
	//if no invite message to the second call, just return
	//this scenario shoud not happen
	cpLog( LOG_ERR, "No invite message to the second call" );
	return 0;    
    }
    
    InviteMsg contactInviteMsg = contact->getInviteMsg();

    // the following comparison statement is equavalent to 
    // if ( *invite2 != contactInviteMsg )
    // != operator is not used because it is not define.
    if ( !(*invite2 == contactInviteMsg)  )
    {
	//if the invite message to the second call does not correspond to 
	//the current contact, just return;
	return 0;
    }

    // Pull out contactToUrl
    Sptr< BaseUrl > baseUrl = contactInviteMsg.getTo().getUrl();
    assert( baseUrl != 0 );
    if( baseUrl->getType() == TEL_URL )
    {
	cpLog( LOG_ERR, "TEL_URL currently not supported\n" );
	assert( 0 );
    }
    // Assume we have a SIP_URL
    Sptr< SipUrl > contactToUrl;
    contactToUrl.dynamicCast( baseUrl );
    assert( contactToUrl != 0 );

    // Pull out msgToUrl
    baseUrl = msg->getTo().getUrl();
    assert( baseUrl != 0 );
    if( baseUrl->getType() == TEL_URL )
    {
	cpLog( LOG_ERR, "TEL_URL currently not supported\n" );
	assert( 0 );
    }
    // Assume we have a SIP_URL
    Sptr< SipUrl > msgToUrl;
    msgToUrl.dynamicCast( baseUrl );
    assert( msgToUrl != 0 );

    // the following comparison statement is equavalent to 
    // if ( *contactToUrl != *msgToUrl )
    // != operator is not used because it is not define.
    if ( !(*contactToUrl == *msgToUrl) )
    {   // the 200 message is coming from our sole contact (call 1)
        // so, we need to return without entering StateIn2Calls
        return 0;
    }

    // otherwise, this is contact2
    cpDebug(LOG_DEBUG, "Far End Answered");
    cpDebug(LOG_DEBUG, "200 received from Contact2" );

    // save the route
    call->setCallerRoute2List( msg->getrecordrouteList() );
    int numContact = msg->getNumContact();
    if ( numContact )
    {
        SipContact contact = msg->getContact( numContact - 1 );
        Sptr < SipRoute > route = new SipRoute;
        route->setUrl( contact.getUrl() );
        call->addRoute2( route );
    }

    // Get remote SDP from message
    Sptr<SipSdp> sdp;
    sdp.dynamicCast( sipMsg->getContentData(0) );
    if ( sdp == 0 )
    {
        cpLog( LOG_ERR, "No remote SDP in ACK" );
    }
    else
    {
        call->setRemote2Sdp( new SipSdp( *sdp ) );
    }

    Sptr < UaStateMachine > stateMachine;
    stateMachine.dynamicCast( event->getCallInfo()->getFeature() );
    assert( stateMachine != 0 );

    return stateMachine->findState( "StateIn2Calls" );

}

