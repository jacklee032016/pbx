#include "global.h"
#include "OpRevert.hxx"

#include "UaDeviceEvent.hxx"
#include "SipContext.hxx"
#include "UaStateMachine.hxx"
#include "UaHardwareEvent.hxx"
#include "UaDevice.hxx"
#include "PbxConfiguration.hxx"
#include "SystemInfo.hxx"
#include "SipSdp.hxx"
#include "SipRecordRoute.hxx"
#include "SipVia.hxx"

using namespace Assist;


const Sptr < State > VoipOpRevert::process( const Sptr < SipProxyEvent > event )
{
    cpDebug(LOG_DEBUG, "OpRevert operation" );

    Sptr < UaDeviceEvent > deviceEvent;
    deviceEvent.dynamicCast( event );
    if ( deviceEvent == 0 )
    {
        return 0;
    }

    if ( deviceEvent->type != DeviceEventFlash &&
            deviceEvent->type != DeviceEventHookUp )
    {
        return 0;
    }

    cpDebug(LOG_DEBUG, "Revert back to First Call" );

    //send invite with correct sdp

    //resume the audio channel for the previous call
    Sptr < SipContext > call;
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
        cpDebug(LOG_DEBUG, "Re-Invite Callee" );

        const StatusMsg& msg = contact->getStatusMsg();
        if ( &msg != 0 )
        {
		hip=msg.getFrom().getHost();
            reInvite = new InviteMsg( msg );

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
        cpDebug(LOG_DEBUG, "Re_Invite callee" );

        const InviteMsg& msg = contact->getInviteMsg();
        if ( &msg != 0 )
        {
            string sipPort = PbxConfiguration::instance()->getLocalSipPort();
            reInvite = new InviteMsg( msg.getFrom().getUrl(),
                                      atoi( sipPort.c_str() ) );
	hip=msg.getTo().getHost();// to or request line, just a question
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
    sipVia.setPort( atoi( PbxConfiguration::instance()
			  ->getLocalSipPort().c_str() ) );
    reInvite->flushViaList();
    reInvite->setVia( sipVia, 0 );

    // Set Contact: header
    Sptr< SipUrl > myUrl = new SipUrl;
    //myUrl->setUserValue( PbxConfiguration::instance()->getUserName(), "phone" );
    string callernum = UaDevice::instance()->getPhoneNumber(event->getDeviceId());
    myUrl->setUserValue(Data(callernum),   dial_phone);
    myUrl->setHost( hip );
    myUrl->setPort( PbxConfiguration::instance()->getLocalSipPort() );
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

    //get local sdp
    Sptr < SipSdp > localSdp = call->getLocalSdp();
    assert( localSdp != 0 );
    Sptr<SipSdp> sipSdp;
    sipSdp.dynamicCast ( reInvite->getContentData( 0 ) );
    assert( sipSdp != 0 );
    sipSdp->setSdpDescriptor( localSdp->getSdpDescriptor() );

    deviceEvent->getSipStack()->sendAsync( *reInvite );

    return 0;
}
