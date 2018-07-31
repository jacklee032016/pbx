/*
* $Id: VoipOpMsgStatusErrorTrying.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/
#include "VoipOpBuilder.hxx"

using namespace Assist;

/* process other error include client/server/global error when in TRYING */
const Sptr <PbxState> VoipOpMsgStatusErrorTrying::process( const Sptr <PbxEvent> event )
{
	Sptr <StatusMsg> msg = isStatusMsg(event);
	if ( msg == 0 )
	{
		return PBX_STATE_CONTINUE;
	}

	Sptr <SipContext> call = getSipContext(event);
	assert( call != 0 );

	int status;
	int best = 1000;
	bool haveError = false;

	ContactList::iterator iter = call->getContactList()->begin();
	while ( iter != call->getContactList()->end() )
	{
		status = (*iter)->getStatus();
		if ( status < best )
		{
			best = status;
		}

		// If status == 486, this event will be handled by OpFarEndBusy
		if ( status >= SIP_STATUS_CODE_CERROR_BAD_REQUEST && status != SIP_STATUS_CODE_CERROR_BUSY_HERE )
		{
			haveError = true;
			cpDebug(LOG_DEBUG, "Status %d from farend", status );
		}
		iter++;
	}

	if ( best >= 300 && haveError )
	{
		//UaDevice::instance()->releaseRtpPort( event->getDeviceId() );

		// send an ack
		AckMsg ack( *msg );

		Sptr <Contact> origContact = call->findContact( *msg );
		assert( origContact != 0 );
		Sptr <BaseUrl> baseUrl = origContact->getInviteMsg().getRequestLine().getUrl();
		assert( baseUrl != 0 );
		if( baseUrl->getType() == TEL_URL )
		{
			cpLog( LOG_ERR, "TEL_URL currently not supported\n" );
			assert( 0 );
		}

		// Assume we have a SIP_URL
		Sptr <SipUrl> reqUrl;
		reqUrl.dynamicCast( baseUrl );
		assert( reqUrl != 0 );

		SipRequestLine reqLine = ack.getRequestLine();
		reqLine.setUrl( reqUrl );
		ack.setRequestLine( reqLine );

		Sptr <VoipEndPoint> vep = getVoipEndPoint(event);
		vep->getSipStack()->sendAsync( ack );

		sendCallSignal(event, CALL_RELEASE);
		
		vep->reset( );
		
		return lookupEPState(event, VOIP_EP_STATE_IDLE );

//		return PBX_STATE_DONE_WITH_EVENT;
	}
	return PBX_STATE_CONTINUE;
}

