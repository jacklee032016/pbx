/*
 * $Id: OpInCallStopTrying.cxx,v 1.2 2007/03/02 04:19:28 lizhijie Exp $
 */

#include "global.h"
#include <cassert>

#include <cpLog.h>

#include "CancelMsg.hxx"
#include "OpInCallStopTrying.hxx"
#include "UaDeviceEvent.hxx"
#include "UaCallInfo.hxx"
#include "UaStateMachine.hxx"
#include "UaDevice.hxx"
#include "UaConfiguration.hxx"


///
using namespace Vocal;


OpInCallStopTrying::OpInCallStopTrying()
{}


///

const Sptr < State >
OpInCallStopTrying::process( Sptr < SipProxyEvent > event )
{
    cpDebug(LOG_DEBUG, "OpInCallStopTrying operation" );

    Sptr < UaDeviceEvent > deviceEvent;
    deviceEvent.dynamicCast( event );
    if ( deviceEvent == 0 )
    {
        return 0;
    }

    if ( deviceEvent->type != DeviceEventFlash )
    {
        return 0;
    }

    Sptr < UaCallInfo > call;
    call.dynamicCast( event->getCallInfo() );
    assert( call != 0 );

    //get the invite message to the second call
    Sptr < InviteMsg > inviteMsg = call->getRing2Invite();
    if ( inviteMsg == 0 )
    {
        cpLog( LOG_ERR, "No invite to cancel.  No action taken");
        return 0;
    }


    //remove this person from the contact list
    Sptr < Contact > contact = call->findContact( *inviteMsg );
    if ( contact != 0 )
    {
        call->removeContact( contact );
    }

    //cancel the invite
    cpDebug(LOG_DEBUG, "CANCEL contact" );
    CancelMsg msg( *inviteMsg );
    deviceEvent->getSipStack()->sendAsync( msg );

    //set the current contact back to the first one
    inviteMsg = call->getRingInvite();
    assert( inviteMsg != 0 );
    contact = call->findContact( *inviteMsg );
    assert( contact != 0 );
    call->setContact( contact );

    return 0;
}


///
OpInCallStopTrying::~OpInCallStopTrying()
{}


///

const char* const
OpInCallStopTrying::name() const
{
    return "OpInCallStopTrying";
}
