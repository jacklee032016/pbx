/*
 * $Id: OpRingingInvite.cxx,v 1.3 2007/03/03 18:39:28 lizhijie Exp $
 */

#include "global.h"
#include "SipEvent.hxx"
#include "InviteMsg.hxx"
#include "OpRingingInvite.hxx"
#include "UaCallInfo.hxx"
#include "SipTransceiver.hxx"
#if 0
#include "UaCommandLine.hxx"  
#endif

using namespace Vocal;


OpRingingInvite::OpRingingInvite()
{
}


OpRingingInvite::~OpRingingInvite()
{
}


const char* const
OpRingingInvite::name() const
{
    return "OpRingingInvite";
}


const Sptr < State >
OpRingingInvite::process( const Sptr < SipProxyEvent > event )
{
    cpDebug(LOG_DEBUG, "OpRingingInvite operation" );

    Sptr < SipEvent > sipEvent;
    sipEvent.dynamicCast( event );
    if ( sipEvent == 0 )
    {
        return 0;
    }
    Sptr < SipMsg > sipMsg = sipEvent->getSipMsg();
    assert( sipMsg != 0 );

    Sptr < InviteMsg > msg;
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
        call->setRingInvite( new InviteMsg( *msg ) );
	call->setContactMsg(msg);
    }

    //save route info
    call->setCalleeRoute1List( msg->getrecordrouteList() );
    int numContact = msg->getNumContact();
    if ( numContact )
    {
        SipContact contact = msg->getContact( numContact - 1 );
        Sptr < SipRoute > route = new SipRoute;
        route->setUrl( contact.getUrl() );
        call->addRoute1( route );
    }

    //re-invite
    //return a 180 Ringing message
    StatusMsg status( *msg, 100 /* trying */ );
    sipEvent->getSipStack()->sendReply( status );

    //TODO Call log
    // Show caller information
#if 0    
    if ( !UaCommandLine::instance()->getBoolOpt( "voicemail" ) )
    {
    cout << endl << VTime::strftime("%y/%m/%d %H:%M:%S")
    << " Call " << msg->getFrom().encode().logData() << endl;
    }
    else
#endif		
    {
        cpDebug(LOG_DEBUG, "Call:%s", msg->getFrom().encode().logData());
    }

    return 0;
}

