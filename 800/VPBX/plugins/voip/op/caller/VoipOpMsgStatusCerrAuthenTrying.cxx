/*
* $Id: VoipOpMsgStatusCerrAuthenTrying.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/
#include "Authenticate.hxx"
#include "AckMsg.hxx"

#include "VoipOpBuilder.hxx"

using namespace Assist;

const Sptr <PbxState> VoipOpMsgStatusCerrAuthenTrying::process( const Sptr <PbxEvent> event )
{
	Sptr <StatusMsg> msg = isStatusMsg(event);
	if ( msg == 0 )
	{
		return PBX_STATE_CONTINUE;
	}

	switch ( msg->getStatusLine().getStatusCode() )
	{
		case SIP_STATUS_CODE_CERROR_PROXY_AUTHEN_NEEDED:     // Proxy Authentication Required
		{
			cpDebug(LOG_DEBUG, "Authentication Required" );
		}
		break;
		/* add ed by wuaiwu 2005/07/07 */
		case SIP_STATUS_CODE_CERROR_UNAUTHORIZED:
		{
			cpDebug(LOG_DEBUG, "Local Authentication Required" );
		}
		break;
		default:
		{
			return PBX_STATE_CONTINUE;
		}
	}

	int cseq = msg->getCSeq().getCSeqData().convertInt();

#if 0
	if( cseq > 1 )
	{
		// this is not the first call sequence so we must have messed up.  ack and hang up.
		return PBX_STATE_CONTINUE;
	}
#endif

	Sptr <SipContext> call = getSipContext(event);
	assert( call != 0 );

	Sptr <Contact> origContact = call->findContact( *msg );
	assert( origContact != 0 );
	AckMsg ack( *msg );

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
	getVoipEndPoint(event)->getSipStack()->sendAsync( ack );

	// Check for Authentication loop
	/* if( origContact->getStatus() == 407 )
	{
		cpLog( LOG_ERR, "Authentication Loop. Wrong password?" );
		return 0;
	}*/

	Sptr <InviteMsg> inviteMsg = new InviteMsg( origContact->getInviteMsg() );

	// Use the same Call Id in the new INVITE
	inviteMsg->setCallId( msg->getCallId() );

	// Increment CSeq
	SipCSeq newCSeq = inviteMsg->getCSeq();

	/***There is some question as to whether we should give a
	* unique callId.  For now, we'll just increase the CSeq.  */
	newCSeq.setCSeq( ++cseq );
	inviteMsg->setCSeq( newCSeq );

	// do the authentication thing
	Data user = msg->getFrom().getUser().convertString();

	if(authenticateMessage( *msg, *inviteMsg, user, PbxConfiguration::instance()->getPassword() ))
	{
		// went OK
		getVoipEndPoint(event)->getSipStack()->sendAsync( *inviteMsg );
	}
	else
	{
		// xxx there was an authentication problem, so we need to abort out
	}

	call->setContactMsg(inviteMsg);
	// Update the Ring INVITE message
	call->setRingInvite( new InviteMsg( *inviteMsg ) );

	return PBX_STATE_CONTINUE;
}

