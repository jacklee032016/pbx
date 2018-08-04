/*
 * $Id: OpHoldTargetOk.cxx,v 1.2 2007/03/02 04:19:28 lizhijie Exp $
 */

#include "global.h"
#include "SipEvent.hxx"
#include "AckMsg.hxx"
#include "OpHoldTargetOk.hxx"
#include "UaDeviceEvent.hxx"
#include "UaStateMachine.hxx"
#include "UaCallInfo.hxx"
#include "UaDevice.hxx"
#include "UaConfiguration.hxx"

using namespace Vocal;


using Vocal::SDP::SdpSession;

OpHoldTargetOk::OpHoldTargetOk()
{
}


OpHoldTargetOk::~OpHoldTargetOk()
{
}


const char* const
OpHoldTargetOk::name() const
{
    return "OpHoldTargetOk";
}


const Sptr < State >
OpHoldTargetOk::process( const Sptr < SipProxyEvent > event )
{
    cpDebug(LOG_DEBUG, "OpHoldTargetOk operation" );

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

    Sptr < Contact > contact = call->findContact( *msg );
    if ( contact == 0 )
    {
        //not a re-invite
        return 0;
    }

    int status = msg->getStatusLine().getStatusCode();

    if ( status != 200 )
    {
        return 0;
    }

    cpDebug(LOG_DEBUG, "Ack hold target");

    // Needs to ack farend
    AckMsg ack( *msg );

    vector < SipRoute* > routeList;
    // add the route info
    if ( *(call->getRingInvite()) == contact->getInviteMsg() )
    {
        cpDebug(LOG_DEBUG, "adding route info for the first call" );
        routeList = call->getRoute1List();
    }
    else
    {
        cpDebug(LOG_DEBUG, "adding route info for the second call" );
        routeList = call->getRoute2List();
    }

    if ( routeList.size() > 0 )
    {
        ack.setRouteList( routeList );
        //
        SipRoute siproute = ack.getRoute(0);
        ack.removeRoute(0);

        SipRequestLine reqLine = ack.getRequestLine();
        reqLine.setUrl( siproute.getUrl() );
        ack.setRequestLine( reqLine );
    }

    sipEvent->getSipStack()->sendAsync( ack );

    // get previous INVITE message
    Sptr < SipMsgQueue > sipReceive = sipEvent->getSipMsgQueue();

    assert( sipReceive != 0 );

    Sptr < SipMsg > prevMsg;
    Sptr < InviteMsg > inviteMsg;
    SipMsgQueue::reverse_iterator i = sipReceive->rbegin();
    i++;     // skip the status message we just received
    while ( i != sipReceive->rend() )
    {
        prevMsg = *i;
        inviteMsg.dynamicCast( prevMsg );
        if ( inviteMsg != 0 )
        {
            cpDebug(LOG_DEBUG, "Found invite" );
            break;
        }
        ++i;
    }
    if ( inviteMsg == 0 )
    {
        return 0;
    }

    // extract sdp info from INVITE
    Sptr<SipSdp> localSdp;
    localSdp.dynamicCast ( inviteMsg->getContentData(0) );
    // check for valid sdp
    if ( !localSdp )
    {
        cpLog( LOG_ERR, "No SDP. No action taken");
        return 0;
    }

    //check for hold
    SdpSession sdpDesc = localSdp->getSdpDescriptor();
    if ( sdpDesc.isHold() )
    {
        // if hold, return in same state
        return 0;
    }

    Sptr<SipSdp> remoteSdp;
    remoteSdp.dynamicCast ( msg->getContentData(0) );
    call->setRemoteSdp( new SipSdp( *remoteSdp ) );

    // grab the hardware adn remove itself from call waiting list
    Sptr < SipCallId > callId = new SipCallId( msg->getCallId() );

	assert ( callId != 0 );
	UaDevice::instance()->setCallId( callId , event->getDeviceId()  );
	UaDevice::instance()->removeCallWaitingId( *callId, event->getDeviceId() );

    Sptr < UaStateMachine > stateMachine;
    stateMachine.dynamicCast( event->getCallInfo()->getFeature() );
    assert( stateMachine != 0 );

    Sptr < ContactList > contactList = call->getContactList();
    assert( contactList != 0 );

    if ( contactList->size() == 1 )
        return stateMachine->findState( "StateInCall" );
    else if ( contactList->size() == 2 )
        return stateMachine->findState( "StateIn2Calls" );
    else
    {
        cpLog( LOG_ERR, "either no active contact or too many contacts");
        return stateMachine->findState( "StateError" );
    }

    return 0;
}
