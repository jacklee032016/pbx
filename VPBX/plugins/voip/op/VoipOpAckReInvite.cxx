/*
 * $Id: VoipOpAckReInvite.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */

#include "global.h"
#include "SipEvent.hxx"
#include "AckMsg.hxx"
#include "StatusMsg.hxx"
#include "OpAckReInvite.hxx"
#include "UaDeviceEvent.hxx"
#include "UaDevice.hxx"
#include "SipContext.hxx"
#include "UaStateMachine.hxx"
#include "SipSdp.hxx"
#include "SystemInfo.hxx"

using namespace Assist;


using Assist::SDP::SdpSession;


const Sptr <State> VoipOpAckReInvite::process( const Sptr <SipProxyEvent> event )
{
    cpDebug(LOG_DEBUG, "OpAckReInvite operation" );

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

    Sptr < SipContext > call;
    call.dynamicCast( event->getCallInfo() );
    assert( call != 0 );

    Sptr < Contact > contact = call->findContact( *msg );
    if ( contact == 0 )
    {
        //not a re-invite
        return 0;
    }

    int status = msg->getStatusLine().getStatusCode();
    bool firstCall = false;

    if ( status >= 200 )
    {
        // Needs to ack farend
        AckMsg ack( *msg );


        vector < SipRoute* > routeList;
        // add the route info
        if ( *(call->getRingInvite()) == contact->getInviteMsg() )
        {
            cpDebug(LOG_DEBUG, "adding route info for the first call" );
            routeList = call->getRoute1List();
            firstCall = true;
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
    }

    if ( status == 200 )  // need to check previous INVITE for 'hold'
    {
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
	localSdp.dynamicCast( inviteMsg->getContentData(0) );

        // check for valid sdp
        if ( !localSdp )
        {
            cpLog( LOG_ERR, "No SDP. No action taken");
            return 0;
        }

        //check for hold
        SdpSession sdpDesc = localSdp->getSdpDescriptor();
        Sptr < UaHardwareEvent > signal =
            new UaHardwareEvent( UaDevice::getDeviceQueue() );

        if ( sdpDesc.isHold() )
        {
            // if hold, return in same state
            return 0;
        }
        else
        {
            // if not hold, save SDP info & return to StateInCall

            Sptr<SipSdp> remoteSdp;
	    remoteSdp.dynamicCast ( msg->getContentData(0) );

            // check if in first/second call before setting remote SDP
            if ( firstCall == true )
            {
                call->setRemoteSdp( new SipSdp( *remoteSdp ) );
            }
            else
            {
                call->setRemote2Sdp( new SipSdp( *remoteSdp ) );
            }

            Sptr < UaStateMachine > stateMachine;
            stateMachine.dynamicCast( event->getCallInfo()->getFeature() );
            assert( stateMachine != 0 );
            return stateMachine->findState( "StateInCall" );
        }
    }
    return 0;
}

