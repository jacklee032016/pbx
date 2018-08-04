/*
* $Id: VoipOpMsgReInviteInCall.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/
#include "DialingValidator.hxx"

#include "VoipOpBuilder.hxx"

using namespace Assist;

using Assist::SDP::SdpSession;

/* process Re-Invite msg when in IN_CALL state */
const Sptr <PbxState> VoipOpMsgReInviteInCall::process( const Sptr <PbxEvent> event )
{
	if(getSipMsgType(event) != SIP_INVITE )
	{
		return PBX_STATE_CONTINUE;
	}

	Sptr <InviteMsg> msg;
	msg.dynamicCast( isSipMsg(event) );
	if ( msg == 0 )
	{
		return PBX_STATE_CONTINUE;
	}

	Sptr <SipContext> context = getSipContext(event);
	assert( context != 0 );
	Sptr <VoipEndPoint> vep = getVoipEndPoint(event);

	Sptr <Contact> contact = context->findContact( *msg );
	if ( contact == 0 )
	{
		context->setRingInvite( new InviteMsg( *msg ) );
		context->setContactMsg(msg);
	}

	//save route info
	context->setCalleeRoute1List( msg->getrecordrouteList() );
	int numContact = msg->getNumContact();
	if ( numContact )
	{
		SipContact contact = msg->getContact( numContact - 1 );
		Sptr < SipRoute > route = new SipRoute;
		route->setUrl( contact.getUrl() );
		context->addRoute1( route );
	}

	cpDebug(LOG_DEBUG,  "%s", (msg->encode().logData()) );
	//re-invite
	//return a 200 ok status message
	StatusMsg status(*msg, SIP_STATUS_CODE_SUCCESS_OK );

	Data hip=msg->getRequestLine().getHost();

	cpDebug(LOG_DEBUG,  "%s", (status.encode().logData()) );
	// Add myself to the contact
	Sptr< SipUrl > myUrl = new SipUrl;
	//myUrl->setUserValue( PbxConfiguration::instance()->getUserName(), "phone" );
	
	string callernum = vep->getPhoneNumber();

	myUrl->setUserValue(Data(callernum),   dial_phone);
	myUrl->setHost( hip );
	myUrl->setPort( PbxConfiguration::instance()->getLocalSipPort() );
	SipContact me;
	me.setUrl( myUrl );
	status.setNumContact( 0 );    // Clear
	status.setContact( me );

	//check SDP
	Sptr<SipSdp> remoteSdp;
	remoteSdp.dynamicCast ( msg->getContentData(0) );
	if ( !remoteSdp )
	{
		cpLog( LOG_ERR, "No SDP. No action taken");
		return 0;
	}

	//check for hold
	SdpSession sdpDesc = remoteSdp->getSdpDescriptor();

	if ( sdpDesc.isHold() )
	{
		cerr << "going on hold" << endl;
	}
	else
	{
		cerr << "going off hold" << endl;

		// if not hold, save the new remote SDP
		context->setRemoteSdp( new SipSdp( *remoteSdp ) );

		// Set local SDP to send to far end
		SipSdp localSdp;
		Data host = hip;
		if(PbxConfiguration::instance()->getNATAddress() != "")
		{
			host = PbxConfiguration::instance()->getNATAddress();
		}
		
		setStandardSdp(localSdp, host, vep->getRtpPort() );
		status.setContentData( &localSdp, 0 );
		context->setLocalSdp( new SipSdp(localSdp) );
	}

	getVoipEndPoint(event)->getSipStack()->sendReply( status );
	
	return PBX_STATE_CONTINUE;
}

