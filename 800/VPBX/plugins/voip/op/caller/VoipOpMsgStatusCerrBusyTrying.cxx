/*
* $Id: VoipOpMsgStatusCerrBusyTrying.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/
#include "VoipOpBuilder.hxx"

using namespace Assist;

const Sptr <PbxState> VoipOpMsgStatusCerrBusyTrying::process( const Sptr <PbxEvent> event )
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
	bool haveBusy = false;

	Sptr <Contact> contact;
	Sptr <ContactList> contacts = call->getContactList();
	ContactList::iterator iter = contacts->begin();

	while ( iter != contacts->end() )
	{
		contact = (*iter);
		assert( contact != 0 );
		status = contact->getStatus();

		if ( status < best )
		{
			best = status;
			cpDebug(LOG_DEBUG, "best == %d", best );
		}
		if ( status == SIP_STATUS_CODE_CERROR_BUSY_HERE )
		{
			haveBusy = true;
			cpDebug(LOG_DEBUG, "status == %d", status );
		}
		iter++;
	}

	if ( best >= 300 && haveBusy )
	{
		cpDebug(LOG_DEBUG, "condition: if( best >= 300 && haveBusy ) VALID" );

		//added by lijie 2005-08-24, we should send an ack to far end
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
		//added ends here

		sendCallSignal(event, CALL_RELEASE);

		// reset in VoipOpMsgStatusErrorTrying 
		//		vep->reset( );
		return lookupEPState(event, VOIP_EP_STATE_IDLE );

//		return PBX_STATE_DONE_WITH_EVENT;
	}

	return PBX_STATE_CONTINUE;
}

