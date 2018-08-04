/*
 * $Id: OpDigitsResp.cxx,v 1.2 2007/03/02 04:19:27 lizhijie Exp $
 */

#include "global.h"

#include "SipEvent.hxx"
#include "AckMsg.hxx"
#include "StatusMsg.hxx"
#include "OpDigitsResp.hxx"
#include "SipTransceiver.hxx"

///
using namespace Vocal;


OpDigitsResp::OpDigitsResp()
{}


OpDigitsResp::~OpDigitsResp()
{}


///

const char* const
OpDigitsResp::name() const
{
    return "OpDigitsResp";
}


///
const Sptr < State >
OpDigitsResp::process( const Sptr < SipProxyEvent > event )
{
    cpDebug(LOG_DEBUG, "OpDigitsResp operation" );

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

    cpDebug(LOG_DEBUG, "Digit Response?" );

    if ( msg->getStatusLine().getStatusCode() == 200 )
    {
        AckMsg ack( *msg );
        sipEvent->getSipStack()->sendAsync( ack );
    }
    return 0;
}
