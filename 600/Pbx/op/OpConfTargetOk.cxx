/*
 * $Id: OpConfTargetOk.cxx,v 1.2 2007/03/02 04:19:27 lizhijie Exp $
 */

#include "global.h"
#include <SipEvent.hxx>
#include <ByeMsg.hxx>

#include "OpConfTargetOk.hxx"
#include "UaCallInfo.hxx"
#include "UaConfiguration.hxx"
#include "SipTransceiver.hxx"
#include "SipVia.hxx"

///
using namespace Vocal;


OpConfTargetOk::OpConfTargetOk()
{}


OpConfTargetOk::~OpConfTargetOk()
{}


///

const char* const
OpConfTargetOk::name() const
{
    return "OpConfTargetOk";
}


///
const Sptr < State >
OpConfTargetOk::process( const Sptr < SipProxyEvent > event )
{
    cpDebug(LOG_DEBUG, "OpConfTargetOk operation" );

    // OpConfTargetOk can be called when in StateInCall, so first check
    // ua config mode for Conference setting
    if ( UaConfiguration::instance()->getXferMode() != ConfOn )
    {
        cpDebug(LOG_DEBUG, "Not in Conference mode" );
        return 0;
    }

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

    //check 200 OK;
    int status = msg->getStatusLine().getStatusCode();
    if ( status != 200 )
    {
        return 0;
    }

    // check method (Transfer)
    if ( msg->getCSeq().getMethod() != "TRANSFER" )
    {
        return 0;
    }

    Sptr < Contact > contact = call->findContact( *msg );
    if ( contact == 0 )
    {
        return 0;
    }

    // Send BYE to conferencee
    status = contact->getStatus();

    SipVia sipVia;
    sipVia.setprotoVersion( "2.0" );
    sipVia.setHost( msg->getFrom().getHost() );	
    sipVia.setPort( atoi( UaConfiguration::instance()->getLocalSipPort().c_str() ) );
    sipVia.setTransport( UaConfiguration::instance()->getSipTransport() );

    Sptr < ByeMsg > bye;

    if ( status == 200 )
    {
        cpDebug(LOG_DEBUG, "BYE callee" );
        bye = new ByeMsg( contact->getStatusMsg() );
    }
    else
    {
        cpDebug(LOG_DEBUG, "BYE caller" );
        bye = new ByeMsg( contact->getInviteMsg() );
    }

    assert( bye != 0 );
    bye->flushViaList();
    bye->setVia( sipVia, 0 );

    unsigned int cseq = contact->getCSeqNum();
    contact->setCSeqNum( ++cseq );
    SipCSeq sipCSeq = bye->getCSeq();
    sipCSeq.setCSeq( cseq );
    bye->setCSeq( sipCSeq );

    sipEvent->getSipStack()->sendAsync( *bye );

    // remove this contact from the contact list
    call->removeContact( contact );

    return 0;
}
