/*
 * $Id: OpRecvdXfer.cxx,v 1.2 2007/03/02 04:19:28 lizhijie Exp $
 */

#include "global.h"
#include <SipEvent.hxx>
#include <TransferMsg.hxx>

#include "OpRecvdXfer.hxx"
#include "UaStateMachine.hxx"
#include "UaCallInfo.hxx"
#include "SipTransceiver.hxx"


///
using namespace Vocal;


OpRecvdXfer::OpRecvdXfer()
{}


OpRecvdXfer::~OpRecvdXfer()
{}


///

const char* const
OpRecvdXfer::name() const
{
    return "OpRecvdXfer";
}


///
const Sptr < State >
OpRecvdXfer::process( const Sptr < SipProxyEvent > event )
{
    cpDebug(LOG_DEBUG, "OpRecvdXfer operation" );

    Sptr < SipEvent > sipEvent;
    sipEvent.dynamicCast( event );
    if ( sipEvent == 0 )
    {
        return 0;
    }
    Sptr < SipMsg > sipMsg = sipEvent->getSipMsg();
    assert( sipMsg != 0 );

    Sptr < TransferMsg > msg;
    msg.dynamicCast( sipMsg );
    if ( msg == 0 )
    {
        return 0;
    }

    cpDebug(LOG_DEBUG, "Being Transferred");

    StatusMsg status( *msg, 100/*Trying*/ );
    sipEvent->getSipStack()->sendReply( status );

    Sptr < UaStateMachine > stateMachine;
    stateMachine.dynamicCast( event->getCallInfo()->getFeature() );
    assert( stateMachine != 0 );

    return stateMachine->findState( "StateBeingXferred" );

}
