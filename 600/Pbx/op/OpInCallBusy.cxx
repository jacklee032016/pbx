/*
 * $Id: OpInCallBusy.cxx,v 1.2 2007/03/02 04:19:28 lizhijie Exp $
 */

#include "global.h"
#include <SipEvent.hxx>

#include "OpInCallBusy.hxx"
#include "UaStateMachine.hxx"
#include "UaCallInfo.hxx"


///
using namespace Vocal;


OpInCallBusy::OpInCallBusy()
{}


OpInCallBusy::~OpInCallBusy()
{}


///

const char* const
OpInCallBusy::name() const
{
    return "OpInCallBusy";
}


///
const Sptr < State >
OpInCallBusy::process( const Sptr < SipProxyEvent > event )
{
    cpDebug(LOG_DEBUG, "OpInCallBusy operation" );

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

    int status;
    int best = 1000;
    bool haveBusy = false;

    Sptr < Contact > contact;
    Sptr < ContactList > contacts = call->getContactList();
    ContactList::iterator iter = contacts->begin();
    while ( iter != contacts->end() )
    {
        contact = (*iter);
        assert( contact != 0 );
        status = contact->getStatus();

        if ( status < best )
        {
            best = status;
        }
        if ( status == 486 )
        {
            haveBusy = true;
        }
        iter++;
    }
    if ( best >= 300 && haveBusy )
    {
        Sptr < UaStateMachine > stateMachine;
        stateMachine.dynamicCast( event->getCallInfo()->getFeature() );
        assert( stateMachine != 0 );

        return stateMachine->findState( "StateInCallBusy" );
    }
    return 0;
}
