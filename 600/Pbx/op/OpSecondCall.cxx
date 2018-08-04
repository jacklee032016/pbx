/*
 * $Id: OpSecondCall.cxx,v 1.2 2007/03/02 04:19:28 lizhijie Exp $
 */

#include "global.h"
#include <cpLog.h>
#include <InviteMsg.hxx>
#include <SipCSeq.hxx>

#include "OpSecondCall.hxx"
#include "UaDeviceEvent.hxx"
#include "UaCallInfo.hxx"
#include "UaStateMachine.hxx"
#include "UaDevice.hxx"
#include "UaConfiguration.hxx"
#include "UaCallContainer.hxx"
#include "SystemInfo.hxx"
#include "SipSdp.hxx"
#include "SipRecordRoute.hxx"
#include "SipVia.hxx"

using namespace Vocal;


using Vocal::SDP::SdpSession;

///
OpSecondCall::OpSecondCall()
{}


OpSecondCall::~OpSecondCall()
{}


///

const char* const
OpSecondCall::name() const
{
    return "OpSecondCall";
}


///
const Sptr < State >
OpSecondCall::process( const Sptr < SipProxyEvent > event )
{
    cpDebug(LOG_DEBUG, "OpSecondCall operation" );

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

    Sptr < SipCallId > call2Id = UaDevice::instance()->getCallWaitingId(deviceEvent->getDeviceId());
    if ( call2Id != 0 )
    {
        // don't initiate second call if there is a cal on call waiting
        return 0;
    }

    // no call on call waiting, treat flashhook as the start
    // of a transfer or conference call
    if ( UaConfiguration::instance()->getXferMode() == XferOff )
    {
        //flashhook disregarded if xfer o cnference feature is off
        return 0;
    }

    cpDebug(LOG_DEBUG, "Switch to Second Call" );

    Sptr < UaCallInfo > call;
    call.dynamicCast( event->getCallInfo() );
    assert( call != 0 );

    // Put current contact on hold
    Sptr < InviteMsg > reInvite;

    Sptr < Contact > contact = call->getContact();
    assert( contact != 0 );

	Data hip;
    int status = contact->getStatus();
    if ( status == 200 )
    {
        cpDebug(LOG_DEBUG, "Put callee on hold" );

        const StatusMsg& msg = contact->getStatusMsg();
        if ( &msg != 0 )
        {
            reInvite = new InviteMsg( msg );
	hip=msg.getFrom().getHost();
            //add SDP
            Sptr < SipSdp > localSdp = call->getLocalSdp();
            assert( localSdp != 0 );
            SipSdp sipSdp = *localSdp;
            reInvite->setContentData( &sipSdp );
        }
        else
        {
            cpLog( LOG_ERR , "No status message for re-INVITE" );
        }
    }
    else
    {
        cpDebug(LOG_DEBUG, "Put caller on hold" );

        const InviteMsg& msg = contact->getInviteMsg();
        if ( &msg != 0 )
        {
            string sipPort = UaConfiguration::instance()->getLocalSipPort();
            reInvite = new InviteMsg( msg.getFrom().getUrl(),
                                      atoi( sipPort.c_str() ) );
	hip=msg.getTo().getHost();
            SipFrom from( msg.getTo().getUrl() );
            reInvite->setFrom( from );
            //TODO Check if it is necessary to set To:
            reInvite->setCallId( msg.getCallId() );

            // Convert RecordRoute to reverse Route
            int numRecordRoute = msg.getNumRecordRoute();
            SipRecordRoute recordroute;
            SipRoute route;

            for ( int i = 0; i < numRecordRoute; i++ )
            {
                recordroute = msg.getRecordRoute( i );
                route.setUrl( recordroute.getUrl() );
                reInvite->setRoute( route );     // to beginning
            }

            int numContact = msg.getNumContact();
            if ( numContact )
            {
                SipContact contact = msg.getContact( numContact - 1 );
                route.setUrl( contact.getUrl() );
                reInvite->setRoute( route );     // to beginning
            }

        }
        else
        {
            cpLog( LOG_ERR , "No INVITE message for re-INVITE" );
        }
    }
    assert( reInvite != 0 );

    SipVia sipVia;
    sipVia.setprotoVersion( "2.0" );
    sipVia.setHost( hip );
    sipVia.setPort( atoi( UaConfiguration::instance()->getLocalSipPort().c_str() ) );
    reInvite->flushViaList();
    reInvite->setVia( sipVia, 0 );

    // Set Contact: header
    Sptr< SipUrl > myUrl = new SipUrl;
    //myUrl->setUserValue( UaConfiguration::instance()->getUserName(), "phone" );
    string callernum = UaDevice::instance()->getPhoneNumber(event->getDeviceId());
    myUrl->setUserValue(Data(callernum),   dial_phone);
    myUrl->setHost( hip );
    myUrl->setPort( UaConfiguration::instance()->getLocalSipPort() );
    SipContact me;
    me.setUrl( myUrl );
    reInvite->setNumContact( 0 );    // Clear
    reInvite->setContact( me );

    //TODO Is it going to be a problem if the other side also use the next
    //TODO CSeq at the same time?
    unsigned int cseq = contact->getCSeqNum();
    contact->setCSeqNum( ++cseq );
    SipCSeq sipCSeq = reInvite->getCSeq();
    sipCSeq.setCSeq( cseq );
    reInvite->setCSeq( sipCSeq );

    Sptr<SipSdp> sipSdp;
    sipSdp.dynamicCast ( reInvite->getContentData( 0 ) );
    assert ( sipSdp != 0 );
    SdpSession sdpDesc = sipSdp->getSdpDescriptor();
    sdpDesc.setHold();
    sipSdp->setSdpDescriptor( sdpDesc );

    deviceEvent->getSipStack()->sendAsync( *reInvite );

    Sptr < UaStateMachine > stateMachine;
    stateMachine.dynamicCast( event->getCallInfo()->getFeature() );
    assert( stateMachine != 0 );

    return stateMachine->findState( "StateInCallDialing" );
}
