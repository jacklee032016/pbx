/*
 * $Id: OpBusyAck.cxx,v 1.2 2007/03/02 04:19:27 lizhijie Exp $
 */

//#include "global.h"
//#include "SipEvent.hxx"
//#include "AckMsg.hxx"

#include "OpBusyAck.hxx"
//#include "UaCallContainer.hxx"
//#include "UaStateMachine.hxx"


///
using namespace Vocal;


OpBusyAck::OpBusyAck()
{}


OpBusyAck::~OpBusyAck()
{}


///

const char* const
OpBusyAck::name() const
{
    return "OpBusyAck";
}


///
const Sptr < State >
OpBusyAck::process( const Sptr < SipProxyEvent > event )
{
    // modify by wuaiwu, 2005/10/17

	return 0;
   
#if 0 
    cpDebug(LOG_DEBUG, "OpBusyAck operation" );

    Sptr < SipEvent > sipEvent;
    sipEvent.dynamicCast( event );
    if ( sipEvent == 0 )
    {
        return 0;
    }
    Sptr < SipMsg > sipMsg = sipEvent->getSipMsg();
    assert( sipMsg != 0 );

    Sptr < AckMsg > msg;
    msg.dynamicCast( sipMsg );
    if ( msg == 0 )
    {
        return 0;
    }


#if 0
    // Remove call info from call container
    Sptr < UaCallContainer > calls;
    calls.dynamicCast( event->getCallContainer() );
    assert( calls != 0 );
	
    calls->deleteCall( msg->getCallId(), event->getDeviceId() );

    Sptr < UaStateMachine > stateMachine;
    stateMachine.dynamicCast( event->getCallInfo()->getFeature() );
    assert( stateMachine != 0 );

    return stateMachine->findState( "StateIdle" );
#endif 
#endif 
    
}
