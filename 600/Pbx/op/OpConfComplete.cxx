/*
 * $Id: OpConfComplete.cxx,v 1.3 2007/03/03 18:39:27 lizhijie Exp $
 */

#include "global.h"
#include "SipEvent.hxx"
#include "ByeMsg.hxx"
#include "AckMsg.hxx"
#include "OpConfComplete.hxx"
#include "UaDevice.hxx"
#include "UaConfiguration.hxx"
#include "UaCallInfo.hxx"
#include "UaStateMachine.hxx"
#include "CancelMsg.hxx"

#ifndef NO_POLICY
#include "PepAgent.hxx"
#endif


using namespace Vocal;


OpConfComplete::OpConfComplete()
{
}


OpConfComplete::~OpConfComplete()
{
}


const char* const
OpConfComplete::name() const
{
    return "OpConfComplete";
}


const Sptr < State >
OpConfComplete::process( const Sptr < SipProxyEvent > event )
{
    cpDebug(LOG_DEBUG, "OpConfComplete operation" );

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

    Sptr < Contact > contact = call->getContact();
    assert( contact != 0 );

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

    //check if we need to respond to the invite message
    if ( inviteMsg != 0 )
    {
       // the following comparison statement is equavalent to 
       // using != operator
       // != operator is not used because it is not define.
       if ( !(inviteMsg->computeCallLeg() == call->getContact()->getInviteMsg().computeCallLeg() ) )
            return 0;

        Sptr<SipSdp> sipSdp;
	sipSdp.dynamicCast ( inviteMsg->getContentData(0));
        if ( sipSdp->getSdpDescriptor().isHold() == true )
            return 0;

        call->getContact()->update( *msg);

        //save the route
        if ( msg->getNumRecordRoute() )
        {
            call->setCallerRoute1List( msg->getrecordrouteList() );
            int numContact = msg->getNumContact();
            if ( numContact )
            {
                SipContact contact = msg->getContact( numContact - 1 );
                Sptr < SipRoute > route = new SipRoute;
                route->setUrl( contact.getUrl() );
                call->addRoute1( route );
            }
        }

        int status = msg->getStatusLine().getStatusCode();

        if ( status >= 200 )
        {

            //cout << "++++++++++++++ status received++++++++++++" << endl;
            //cout << msg->encode().logData() << endl << endl;
            cpDebug(LOG_DEBUG, "++++++++++++++ status received++++++++++++");
	     cpDebug(LOG_DEBUG, "%s\n", msg->encode().logData() );
            // Ack the status message
            cpDebug(LOG_DEBUG, "Ack Invite" );
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

		Sptr< BaseUrl > baseUrl = siproute.getUrl();
                reqLine.setUrl( baseUrl );
                ack.setRequestLine( reqLine );
            }

            //cout << "++++++++++++++ Ack sent++++++++++++" << endl;
            //cout << ack.encode().logData() << endl << endl;
            
	    cpDebug(LOG_DEBUG,  "++++++++++++++ Ack sent++++++++++++");
           cpDebug(LOG_DEBUG,  "%s\n", ack.encode().logData());

            sipEvent->getSipStack()->sendAsync( ack );
        }

        if ( status != 200 )
        {
            return 0;
        }

        // Cancel all outstanding invites
        Sptr < ContactList > contacts = call->getContactList();
        ContactList::iterator iter = contacts->begin();
        while ( iter != contacts->end() )
        {
            Sptr < Contact > sipContact = (*iter);
            assert( sipContact != 0 );
            if ( sipContact->getStatus() < 200 )
            {
                cpDebug(LOG_DEBUG, "CANCEL contact" );
                CancelMsg msg( sipContact->getInviteMsg() );
                sipEvent->getSipStack()->sendAsync( msg );
            }
            iter++;
        }

        // Get remote SDP from message
        Sptr<SipSdp> sdp;
	sdp.dynamicCast ( sipMsg->getContentData(0) );
        if ( sdp == NULL )
        {
            cpLog( LOG_ERR, "No remote SDP in ACK" );
        }
        else
        {
            call->setRemoteSdp( new SipSdp( *sdp ) );
        }

#ifdef HAS_RSVPLIB
        // RSVP stuff ported from old OpFarEndAnswered.cxx
        Sptr < SipSdp > localSdp = call->getLocalSdp();
        Sptr < SipSdp > remoteSdp = call->getRemoteSdp();
#endif

#ifndef NO_POLICY
        if (UaConfiguration::instance()->getUsePolicyServer() )
        {
            cpLog( LOG_ERR, "Enable QoS handled by Marshal" );
        }
#endif

#ifdef HAS_RSVPLIB
        if (UaConfiguration::instance()->getRsvpOn() )
        {
            //send RSVP message
            SuaRsvp &suaRsvpSender = UaDevice::instance()->getSuaRsvpSender();
            SuaRsvp &suaRsvpReserv = UaDevice::instance()->getSuaRsvpReserv();

            NetworkAddress myAddress;
            NetworkAddress remoteAddress;
            LocalScopeAllocator lo;
            myAddress.setHostName( localSdp->getConnAddress().getData(lo) );
            myAddress.setPort( localSdp->getRtpPort() );

            remoteAddress.setHostName( remoteSdp->getConnAddress().getData(lo) );
            remoteAddress.setPort( remoteSdp->getRtpPort() );

            remoteAddress.getSockAddr(suaRsvpReserv.src_addr);

            suaRsvpReserv.session(&myAddress);
            myAddress.setPort(0);
            suaRsvpReserv.reserve(&myAddress);

            suaRsvpSender.session(&remoteAddress);

            myAddress.setPort( localSdp->getRtpPort() );
            suaRsvpSender.sender(&myAddress);
        }
#endif

    }

    if ( contact->getStatus() != 200 )
    {
        return 0;
    }

    if ( call->getContactList()->size() != 0 )
    {
        //clear the invite contact and add mcu
        Sptr < Contact > pContact( call->findContact( *inviteMsg) );
        call->removeContact( pContact );
        call->addContact( contact );
    }
    else
    {
        cpDebug(LOG_DEBUG, "empty list, returning",
               call->getContactList()->size() );
        return 0;
    }

    cpDebug(LOG_DEBUG, "Complete Conferencing" );

    Sptr < UaStateMachine > stateMachine;
    stateMachine.dynamicCast( event->getCallInfo()->getFeature() );
    assert( stateMachine != 0 );

    return stateMachine->findState( "StateInCall" );
}

