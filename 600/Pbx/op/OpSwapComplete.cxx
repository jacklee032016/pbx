/*
 * $Id: OpSwapComplete.cxx,v 1.2 2007/03/02 04:19:28 lizhijie Exp $
 */

#include "global.h"
#include <SipEvent.hxx>
#include <AckMsg.hxx>
#include <StatusMsg.hxx>

#include "OpSwapComplete.hxx"
#include "UaDeviceEvent.hxx"
#include "UaDevice.hxx"
#include "UaCallInfo.hxx"
#include "UaStateMachine.hxx"

///
using namespace Vocal;


OpSwapComplete::OpSwapComplete()
{}


OpSwapComplete::~OpSwapComplete()
{}


///

const char* const
OpSwapComplete::name() const
{
    return "OpSwapComplete";
}


///
const Sptr < State >
OpSwapComplete::process( const Sptr < SipProxyEvent > event )
{
    cpDebug(LOG_DEBUG, "OpSwapComplete operation" );

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

    Sptr < Contact > contact = call->findContact( *msg );
    if ( contact == 0 )
    {
        //not a re-invite
        return 0;
    }

    int status = msg->getStatusLine().getStatusCode();

    if ( status >= 200 || status >= 400 )
    {
        // Needs to ack farend
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
            reqLine.setUrl( siproute.getUrl() );
            ack.setRequestLine( reqLine );
        }

        sipEvent->getSipStack()->sendAsync( ack );
    }
    return 0;
}
