/*
 * $Id: OpReInvite.cxx,v 1.3 2007/03/03 18:39:28 lizhijie Exp $
 */

#include "global.h"
#include "cpLog.h"
#include "InviteMsg.hxx"
#include "SipEvent.hxx"
#include "StatusMsg.hxx"
#include "OpReInvite.hxx"
#include "UaDevice.hxx"
#include "UaHardwareEvent.hxx"
#include "UaCallInfo.hxx"
#include "UaConfiguration.hxx"
#include "SdpHandler.hxx"
#include "SystemInfo.hxx"
#include "SipSdp.hxx"

using namespace Vocal;


using Vocal::SDP::SdpSession;


OpReInvite::OpReInvite()
{
}


OpReInvite::~OpReInvite()
{
}


const char* const
OpReInvite::name() const
{
    return "OpReInvite";
}


const Sptr < State >
OpReInvite::process( const Sptr < SipProxyEvent > event )
{
    cpDebug(LOG_DEBUG, "OpReInvite operation" );

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

// TMP JACK
        cpDebug(LOG_DEBUG,  "%s", (msg->encode().logData()) );
    //re-invite
    //return a 200 ok status message
    StatusMsg status(*msg, 200 /*OK*/ );

	Data hip=msg->getRequestLine().getHost();
    cpDebug(LOG_DEBUG,  "%s", (status.encode().logData()) );
    // Add myself to the contact
    Sptr< SipUrl > myUrl = new SipUrl;
    //myUrl->setUserValue( UaConfiguration::instance()->getUserName(), "phone" );
    string callernum = UaDevice::instance()->getPhoneNumber(event->getDeviceId());
    myUrl->setUserValue(Data(callernum),   dial_phone);
    myUrl->setHost( hip );
    myUrl->setPort( UaConfiguration::instance()->getLocalSipPort() );
    SipContact me;
    me.setUrl( myUrl );
    status.setNumContact( 0 );    // Clear
    status.setContact( me );

    //check SDP
    Sptr<SipSdp> remoteSdp;
    remoteSdp.dynamicCast ( sipMsg->getContentData(0) );

    if ( !remoteSdp )
    {
        cpLog( LOG_ERR, "No SDP. No action taken");
        return 0;
    }

    //check for hold
    SdpSession sdpDesc = remoteSdp->getSdpDescriptor();

	Sptr < UaHardwareEvent > signal = new UaHardwareEvent( UaDevice::getDeviceQueue() );
	signal->setDeviceId( event->getDeviceId() );

    if ( sdpDesc.isHold() )
    {
        cerr << "going on hold" << endl;
    }
    else
    {
        cerr << "going off hold" << endl;

        // if not hold, save the new remote SDP
        call->setRemoteSdp( new SipSdp( *remoteSdp ) );

        // Set local SDP to send to far end
        SipSdp localSdp;

        Data host = hip;
        if(UaConfiguration::instance()->getNATAddress() != "")
        {
            host = UaConfiguration::instance()->getNATAddress();
        }
		
		setStandardSdp(localSdp, host, UaDevice::instance()->getRtpPort( event->getDeviceId() ));
        status.setContentData( &localSdp, 0 );
        call->setLocalSdp( new SipSdp(localSdp) );
    }

    sipEvent->getSipStack()->sendReply( status );
    return 0;
}

