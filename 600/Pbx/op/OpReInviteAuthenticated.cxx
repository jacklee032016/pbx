/*
 * $Id: OpReInviteAuthenticated.cxx,v 1.4 2007/03/06 19:37:35 lizhijie Exp $
 */

#include "global.h"
#include "SipEvent.hxx"
#include "AckMsg.hxx"
#include "OpReInviteAuthenticated.hxx"
#include "UaCallInfo.hxx"
#include "UaConfiguration.hxx"
#include "Authenticate.hxx"
#include "SipTransceiver.hxx"


using namespace Vocal;


OpReInviteAuthenticated::OpReInviteAuthenticated()
{
}


OpReInviteAuthenticated::~OpReInviteAuthenticated()
{
}


const char* const
OpReInviteAuthenticated::name() const
{
    return "OpReInviteAuthenticated";
}


const Sptr <State> OpReInviteAuthenticated::process( const Sptr <SipProxyEvent> event )
{
	cpDebug(LOG_DEBUG, "OpReInviteAuthenticated operation" );

	Sptr <SipEvent> sipEvent;
	sipEvent.dynamicCast( event );
	if ( sipEvent == 0 )
	{
		return 0;
	}
	Sptr < SipMsg > sipMsg = sipEvent->getSipMsg();
	assert( sipMsg != 0 );

	Sptr < StatusMsg > msg;
	msg.dynamicCast( sipMsg );
	if ( msg == 0 )
	{
		return 0;
	}

	switch ( msg->getStatusLine().getStatusCode() )
	{
		case 407:     // Proxy Authentication Required
		{
			cpDebug(LOG_DEBUG, "Proxy Authentication Required" );
		}
		break;
		/* add ed by wuaiwu 2005/07/07 */
		case 401:
		{
			cpDebug(LOG_DEBUG, "Local Authentication Required" );
		}
		break;
		default:
		{
			return 0;
		}
	}

	int cseq = sipMsg->getCSeq().getCSeqData().convertInt();

#if 0
	if( cseq > 1 )
	{
		// this is not the first call sequence so we must have messed
		// up.  ack and hang up.
		return 0;
	}
#endif

	Sptr <UaCallInfo> call;
	call.dynamicCast( event->getCallInfo() );
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
	sipEvent->getSipStack()->sendAsync( ack );

	// Check for Authentication loop
	/* if( origContact->getStatus() == 407 )
	{
	cpLog( LOG_ERR, "Authentication Loop. Wrong password?" );
	return 0;
	}*/

	Sptr <InviteMsg> inviteMsg = new InviteMsg( origContact->getInviteMsg() );

	// Use the same Call Id in the new INVITE
	inviteMsg->setCallId( sipMsg->getCallId() );
	// Increment CSeq
	SipCSeq newCSeq = inviteMsg->getCSeq();

	/**
	*There is some question as to whether we should give a
	* unique callId.  For now, we'll just increase the CSeq.
	*/
	newCSeq.setCSeq( ++cseq );
	inviteMsg->setCSeq( newCSeq );

	// do the authentication thing
#if 0/* lizhijie, 2007.03.06 for authen with SIP Server */	
	Data user = msg->getFrom().getUser().convertString();
#else
	Data user = UaConfiguration::instance()->getUserName();
#endif

	if(authenticateMessage( *msg, *inviteMsg, user, UaConfiguration::instance()->getPassword() ))
	{
		// went OK
		sipEvent->getSipStack()->sendAsync( *inviteMsg );
	}
	else
	{
		// xxx there was an authentication problem, so we need to abort out
	}

	call->setContactMsg(inviteMsg);


	// Update the Ring INVITE message
	call->setRingInvite( new InviteMsg( *inviteMsg ) );
	return 0;
}

