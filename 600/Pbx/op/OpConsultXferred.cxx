/*
 * $Id: OpConsultXferred.cxx,v 1.3 2007/03/03 18:39:27 lizhijie Exp $
 */

#include "global.h"
#include "SipEvent.hxx"
#include "ByeMsg.hxx"
#include "StatusMsg.hxx"
#include "TransferMsg.hxx"
#include "AckMsg.hxx"
#include "OpConsultXferred.hxx"
#include "UaHardwareEvent.hxx"
#include "UaDevice.hxx"
#include "UaStateMachine.hxx"
#include "UaCallInfo.hxx"


using namespace Vocal;


OpConsultXferred::OpConsultXferred()
{
}


OpConsultXferred::~OpConsultXferred()
{
}


const char* const
OpConsultXferred::name() const
{
    return "OpConsultXferred";
}


const Sptr < State >
OpConsultXferred::process( const Sptr < SipProxyEvent > event )
{
    cpDebug(LOG_DEBUG, "OpConsultXferred operation" );

    Sptr < SipEvent > sipEvent;
    sipEvent.dynamicCast( event );
    if ( sipEvent == 0 )
    {
        return 0;
    }
    Sptr < SipMsg > sipMsg = sipEvent->getSipMsg();
    assert( sipMsg != 0 );

    Sptr < UaCallInfo > call;
    call.dynamicCast( event->getCallInfo() );
    assert( call != 0 );


    //check for 200 message
    Sptr < StatusMsg > msg;
    msg.dynamicCast( sipMsg );
    if ( msg == 0 )
    {
        return 0;
    }

    Sptr < Contact > contact = call->getContact();
    assert( contact != 0 );

    if ( msg->getStatusLine().getStatusCode() != 200 )
    {
        return 0;
    }

    cpDebug(LOG_DEBUG, "Consult Xferred" );

    int statusCode = call->getContact()->getStatus();

    if ( statusCode != 180 && statusCode != 183 )
    {
        cpDebug(LOG_DEBUG, "Consultation transfer" );
        //send the 200 for transfer msg
        Sptr < TransferMsg > xferMsg = call->getXferMsg();
        assert( xferMsg != 0 );
        StatusMsg status(*xferMsg, 200/*OK*/);
        sipEvent->getSipStack()->sendReply( status );
        call->setXferMsg(0);
    }
    else
    {
        cpDebug(LOG_DEBUG, "Converting from Blind Xfer to Consultation Xfer" );
        cpDebug(LOG_DEBUG, "No need to respond to transferMsg" );
    }

    contact->update( *msg );

    //save the route
    if ( msg->getNumRecordRoute() )
    {
        if ( *(call->getRingInvite() ) == contact->getInviteMsg() )
        {
            call->setCallerRoute1List( msg->getrecordrouteList() );
        }
        else
        {
            call->setCallerRoute2List( msg->getrecordrouteList() );
        }

        int numContact = msg->getNumContact();
        if ( numContact )
        {
            SipContact sipContact = msg->getContact( numContact - 1 );
            Sptr < SipRoute > route = new SipRoute;
            route->setUrl( sipContact.getUrl() );
            if ( *(call->getRingInvite() ) == contact->getInviteMsg() )
                call->addRoute1( route );
            else
                call->addRoute2( route );
        }
    }


    //ACK the 200
    AckMsg ack( *msg );
    sipEvent->getSipStack()->sendAsync( ack );

    // Get remote SDP from message
    Sptr<SipSdp> sdp;
    sdp.dynamicCast ( msg->getContentData(0) );
    if ( sdp == NULL )
    {
        cpLog( LOG_ERR, "No remote SDP in 200 Status" );
    }
    else
    {
        call->setRemoteSdp( new SipSdp( *sdp ) );
    }

    return 0;
}

