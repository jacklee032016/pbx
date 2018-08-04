/*
 * $Id: OpSwapCalls.cxx,v 1.3 2007/03/02 09:19:50 lizhijie Exp $
 */

#include "global.h"
#include "OpSwapCalls.hxx"
#include "UaDeviceEvent.hxx"
#include "UaCallInfo.hxx"
#include "UaStateMachine.hxx"
#include "UaHardwareEvent.hxx"
#include "UaDevice.hxx"
#include "UaConfiguration.hxx"
#include "Sdp2Session.hxx"
#include "Sdp2Media.hxx"
#include "SipRecordRoute.hxx"
#include "SipVia.hxx"

using namespace Vocal;


using Vocal::SDP::SdpSession;
using Vocal::SDP::SdpMedia;
using Vocal::SDP::MediaAttributes;
using Vocal::SDP::ValueAttribute;
using Vocal::SDP::SdpRtpMapAttribute;


OpSwapCalls::OpSwapCalls()
{
}


OpSwapCalls::~OpSwapCalls()
{
}


const char* const
OpSwapCalls::name() const
{
    return "OpSwapCalls";
}


const Sptr < State >
OpSwapCalls::process( const Sptr < SipProxyEvent > event )
{
    cpDebug(LOG_DEBUG, "OpSwapCalls operation" );

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

    Sptr < SipCallId > call2Id = UaDevice::instance()->getCallWaitingId(event->getDeviceId());
    if ( call2Id != 0 )
    {
        // don't initiate second call if there is a cal on call waiting
        return 0;
    }

    cpDebug(LOG_DEBUG, "Swap the call" );

    if ( !putContactOnhold( deviceEvent ) )
    {
        cpLog( LOG_WARNING, "Cannot put contact on hold" );
        return 0;
    }

    restoreHeldCall( deviceEvent );

    return 0;
}


bool
OpSwapCalls::putContactOnhold( const Sptr < UaDeviceEvent > deviceEvent )
{
    Sptr < UaCallInfo > call;
    call.dynamicCast( deviceEvent->getCallInfo() );
    assert( call != 0 );

    //put the current contact on hold
    Sptr < Contact > contact = call->getContact();
    if ( contact == 0 )
    {
        cpLog( LOG_ERR, "No current contact");
        return false;
    }
    Sptr < InviteMsg > reInvite;

    //get the current invite msg
    Sptr < InviteMsg > inviteMsg = call->getRingInvite();
    // the following comparison statement is equavalent to 
    // if ( *inviteMsg != contact->getInviteMsg() )
    // != operator is not used because it is not define.
    if ( !(*inviteMsg == contact->getInviteMsg()) )
    {
        cpDebug(LOG_DEBUG, "Putting the 2nd call on hold" );
        firstCall = true;
    }
    else
    {
        cpDebug(LOG_DEBUG, "Putting the 1st call on hold" );
        firstCall = false;
    }
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
    sipVia.setPort( atoi( UaConfiguration::instance()
			  ->getLocalSipPort().c_str() ) );
    reInvite->flushViaList();
    reInvite->setVia( sipVia, 0 );

    // Set Contact: header
    Sptr< SipUrl > myUrl = new SipUrl;
    //myUrl->setUserValue( UaConfiguration::instance()->getUserName(), "phone" );
    string callernum = UaDevice::instance()->getPhoneNumber(deviceEvent->getDeviceId());
    myUrl->setUserValue(Data(callernum),   dial_phone);
    myUrl->setHost( hip);
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

    //sending re-invite
    deviceEvent->getSipStack()->sendAsync( *reInvite );

    Sptr < UaHardwareEvent > signal =
	new UaHardwareEvent( UaDevice::getDeviceQueue() );
    signal->type = HardwareAudioType;

    struct HardwareAudioRequest* request = &(signal->signalOrRequest.request);

    request->type = AudioStop;

    UaDevice::getDeviceQueue()->add( signal );

    return true;
}


void
OpSwapCalls::restoreHeldCall( const Sptr < UaDeviceEvent > deviceEvent )
{
    Sptr < UaCallInfo > call;
    call.dynamicCast( deviceEvent->getCallInfo() );
    assert( call != 0 );

    // Find contact data of held call
    Sptr < InviteMsg > inviteMsg;
    if ( !firstCall )
    {
        cpDebug(LOG_DEBUG, "Restoring 2nd call");
        inviteMsg = call->getRing2Invite();
    }
    else
    {
        cpDebug(LOG_DEBUG, "Restoring 1st call");
        inviteMsg = call->getRingInvite();
    }

    Sptr < Contact > contact = call->findContact( *inviteMsg );
    if ( contact == 0 )
    {
        cpLog( LOG_ERR, "No contact data" );
        return ;
    }
    call->setContact( contact );

    // Restore media
    Sptr < InviteMsg > reInvite;
	Data hip;
    int status = contact->getStatus();
    if ( status == 200 )
    {
        cpDebug(LOG_DEBUG, "Restore callee" );

        const StatusMsg& msg = contact->getStatusMsg();
        if ( &msg != 0 )
        {
            //cout << endl
            //<< "Switch back to call "
            //<< msg.getTo().encode().logData()
            //<< endl;
	     cpDebug(LOG_DEBUG, "\nSwitch back to call %s", msg.getTo().encode().logData()); 

            reInvite = new InviteMsg( msg );
	hip=msg.getFrom().getHost();
            // Add Sdp for later use
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
        cpDebug(LOG_DEBUG, "Restore caller" );

        const InviteMsg& msg = contact->getInviteMsg();
        if ( &msg != 0 )
        {

            //cout << endl
            //<< "Switch back to call "
            //<< msg.getFrom().encode().logData()
            //<< endl;

	     cpDebug(LOG_DEBUG, "\nSwitch back to call %s", msg.getFrom().encode().logData()); 

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
    sipVia.setPort( atoi( UaConfiguration::instance()
			  ->getLocalSipPort().c_str() ) );
    reInvite->flushViaList();
    reInvite->setVia( sipVia, 0 );

    // Set Contact: header
    Sptr< SipUrl > myUrl = new SipUrl;
    //myUrl->setUserValue( UaConfiguration::instance()->getUserName() );
    string callernum = UaDevice::instance()->getPhoneNumber(deviceEvent->getDeviceId());
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

    //get local sdp
    Sptr < SipSdp > localSdp = call->getLocalSdp();
    assert( localSdp != 0 );
    Sptr<SipSdp> sipSdp;
    sipSdp.dynamicCast ( reInvite->getContentData( 0 ) );
    assert( sipSdp != 0 );
    sipSdp->setSdpDescriptor( localSdp->getSdpDescriptor() );

    deviceEvent->getSipStack()->sendAsync( *reInvite );

    Sptr < SipSdp > remoteSdp;
    if ( firstCall )
    {
        remoteSdp = call->getRemoteSdp();
    }
    else
    {
        remoteSdp = call->getRemote2Sdp();
    }

    if ( remoteSdp == 0)
    {
        cpLog( LOG_ERR, "No remote SDP" );
        //TODO send BYE ???
        return ;
    }

    Sptr < UaHardwareEvent > signal =
	new UaHardwareEvent( UaDevice::getDeviceQueue() );
    signal->type = HardwareAudioType;

    struct HardwareAudioRequest* request = &(signal->signalOrRequest.request);

    request->type = AudioStart;

    // Set remote host and port
    LocalScopeAllocator lo;
    strcpy( request->remoteHost,
            remoteSdp->getConnAddress().getData(lo) );
    request->remotePort = remoteSdp->getRtpPort();

    // Set local host and port
    request->localPort = localSdp->getRtpPort();
    strcpy( request->localHost,
            localSdp->getConnAddress().getData(lo) );

    //TODO Why echo cancellation always true?
    request->echoCancellation = true;

    // Set rtpPacketSize to default in ua.cfg file
    int rtpPacketSize = UaConfiguration::instance()->getNetworkRtpRate();

    // Check local, not remote ???
    // How is this related to Network_RTP_Rate in configuration file???
#ifndef __AS_RELEASE__
		int tmp;
#endif
		cpDebug(LOG_DEBUG, "Local SDP:\n%s", localSdp->encodeBody(tmp).logData() );

    SdpSession sdpDesc = localSdp->getSdpDescriptor();
    list < SdpMedia* > mediaList;
    mediaList = sdpDesc.getMediaList();
    list < SdpMedia* > ::iterator mediaIterator = mediaList.begin();
    MediaAttributes* mediaAttrib;
    mediaAttrib = (*mediaIterator)->getMediaAttributes();

    if ( mediaAttrib != 0 )
    {
        vector < ValueAttribute* > * valueAttribList =
	    mediaAttrib->getValueAttributes();
        if ( valueAttribList->size() == 0 )
        {
            cpDebug(LOG_DEBUG, "No value attribute in media" );
        }
        vector < ValueAttribute* > ::iterator attribIterator =
	    valueAttribList->begin();
        while ( attribIterator != valueAttribList->end() )
        {
            char* attribName = (*attribIterator)->getAttribute();
            if ( strcmp( attribName, "ptime" ) == 0 )
            {
                rtpPacketSize = Data( (*attribIterator)->getValue() ).convertInt();
                break;
            }
            attribIterator++;
        }
        request->rtpPacketSize = rtpPacketSize;
    }
    else
    {
        cpDebug(LOG_DEBUG, "No media attributes in local SDP" );
    }

    //modified by lijie 2005-06-03	
    //request->sendRingback = false;

    cpDebug(LOG_DEBUG, "Start audio( duplex ):" );
    cpDebug(LOG_DEBUG, "localHost     = %s", request->localHost);
    cpDebug(LOG_DEBUG, "localPort     = %d", request->localPort);
    cpDebug(LOG_DEBUG, "remoteHost    = %s", request->remoteHost);
    cpDebug(LOG_DEBUG, "remotePort    = %d", request->remotePort);
    cpDebug(LOG_DEBUG, "rtpPacketSize = %d", request->rtpPacketSize);

    UaDevice::getDeviceQueue()->add( signal );
}
