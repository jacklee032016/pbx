/*
 * $Id: OpInCallFastBusy.cxx,v 1.2 2007/03/02 04:19:28 lizhijie Exp $
 */

#include "global.h"
#include <SipEvent.hxx>

#include "OpInCallFastBusy.hxx"
#include "UaStateMachine.hxx"
#include "UaCallInfo.hxx"


///
using namespace Vocal;


OpInCallFastBusy::OpInCallFastBusy()
{}


OpInCallFastBusy::~OpInCallFastBusy()
{}


///

const char* const
OpInCallFastBusy::name() const
{
    return "OpInCallFastBusy";
}


///
const Sptr < State >
OpInCallFastBusy::process( const Sptr < SipProxyEvent > event )
{
    cpDebug(LOG_DEBUG, "OpInCallFastBusy operation" );

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
        if ( status >= 500 )
        {
            haveBusy = true;
        }
        iter++;
    }
    if ( haveBusy )
    {
        Sptr < UaStateMachine > stateMachine;
        stateMachine.dynamicCast( event->getCallInfo()->getFeature() );
        assert( stateMachine != 0 );

        return stateMachine->findState( "StateInCallFastBusy" );
    }

    return 0;
}
