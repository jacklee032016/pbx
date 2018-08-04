/*
* $Id: VoipOpMsgStatusSuccessTrying.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/
#include "VoipOpBuilder.hxx"

#include "AckMsg.hxx"
using namespace Assist;

const Sptr <PbxState> VoipOpMsgStatusSuccessTrying::process( const Sptr <PbxEvent> event )
{
	Sptr <StatusMsg> msg = isStatusMsg(event);
	if ( msg == 0 )
	{
		return PBX_STATE_CONTINUE;
	}

	if(msg->getStatusLine().getStatusCode() > SIP_STATUS_CODE_SUCCESS_OK)
	{
		return PBX_STATE_CONTINUE;
	}

	Sptr <SipContext> call = getSipContext(event);
	assert( call != 0 );

	Sptr <Contact> findContact = call->findContact( *msg );
	cpDebug(LOG_DEBUG, "findContact = %s\n", findContact->computeCallLeg().encode().logData() );

	Sptr <Contact> getContact = call->getContact();
	cpDebug(LOG_DEBUG, "getContact = %s\n", getContact->computeCallLeg().encode().logData() );

	if ( *( call->findContact( *msg ) ) != *( call->getContact() ) )
	{
		cpDebug(LOG_DEBUG, "Not the current call" );
		return PBX_STATE_CONTINUE;
	}

	call->getContact()->update( *msg);
	int status = msg->getStatusLine().getStatusCode();
	if ( status >= SIP_STATUS_CODE_SUCCESS_OK )
	{
		// Needs to ack farend
		AckMsg ack( *msg );
		getVoipEndPoint(event)->getSipStack()->sendAsync( ack );
	}

	if ( status != SIP_STATUS_CODE_SUCCESS_OK )
	{
		return PBX_STATE_DONE_WITH_EVENT;  // Done
	}

	//save the route
	call->setCallerRoute1List( msg->getrecordrouteList() );
	int numContact = msg->getNumContact();
	if ( numContact )
	{
		SipContact contact = msg->getContact( numContact - 1 );
		Sptr <SipRoute> route = new SipRoute;
		route->setUrl( contact.getUrl() );
		call->addRoute1( route );
	}

	// Get remote SDP from message
	Sptr <SipSdp> sdp;
	sdp.dynamicCast (msg->getContentData(0) );
	if ( sdp == NULL )
	{
		cpLog( LOG_ERR, "No remote SDP in 200" );
	}
	else
	{
		call->setRemoteSdp( new SipSdp( *sdp ) );
	}

	// RSVP stuff ported from old OpFarEndAnswered.cxx
	Sptr <SipSdp> localSdp = call->getLocalSdp();
	Sptr <SipSdp> remoteSdp = call->getRemoteSdp();

	rsvpFarEndAnswered(localSdp, remoteSdp);


#ifndef NO_POLICY
	if (PbxConfiguration::instance()->getUsePolicyServer() )
	{
		cpDebug(LOG_DEBUG,"Enable QoS handled in Marshal");
	}
#endif

	Sptr <AudioChannel> audio = event->getEndPoint()->getAudio();
	assert( audio != 0);
	audio->activate();

	sendCallSignal(event, CALL_CONNECT);

	return lookupEPState(event, VOIP_EP_STATE_INCALL);
}

