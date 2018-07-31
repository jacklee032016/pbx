/*
* $Id: VoipOpMsgReInviteRinging.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/
#include "VoipOpBuilder.hxx"

using namespace Assist;

/* process duplex SIP INVITE msg when in RINGING state */
const Sptr <PbxState> VoipOpMsgReInviteRinging::process( const Sptr <PbxEvent> event )
{
	if(getSipMsgType(event) != SIP_INVITE)
	{
		return PBX_STATE_CONTINUE;
	}

	Sptr <InviteMsg> inviteMsg;
	inviteMsg.dynamicCast( isSipMsg(event) );
	assert( inviteMsg != 0 );

	Sptr <SipContext> context = getSipContext(event);
	assert( context != 0 );

	Sptr <Contact> contact = context->findContact( *inviteMsg );
	if ( contact == 0 )
	{
		context->setRingInvite( new InviteMsg( *inviteMsg ) );
		context->setContactMsg( inviteMsg);
	}

	//save route info
	context->setCalleeRoute1List( inviteMsg->getrecordrouteList() );
	int numContact = inviteMsg->getNumContact();
	if ( numContact )
	{
		SipContact contact = inviteMsg->getContact( numContact - 1 );
		Sptr <SipRoute> route = new SipRoute;
		route->setUrl( contact.getUrl() );
		context->addRoute1( route );
	}

	//re-invite
	//return a 180 Ringing message
	StatusMsg status( *inviteMsg, SIP_STATUS_CODE_INFO_TRYING );
	getVoipEndPoint(event)->getSipStack()->sendReply( status );

    //TODO Call log
    // Show caller information
#if 0    
	if ( !UaCommandLine::instance()->getBoolOpt( "voicemail" ) )
	{
		cout << endl << VTime::strftime("%y/%m/%d %H:%M:%S")
			<< " Call " << msg->getFrom().encode().logData() << endl;
	}
	else
#endif		
	{
		cpDebug(LOG_DEBUG, "Call:%s", inviteMsg->getFrom().encode().logData());
	}

	return PBX_STATE_DONE_WITH_EVENT;
}

