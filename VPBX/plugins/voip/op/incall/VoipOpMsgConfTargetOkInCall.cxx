/*
 * $Id: VoipOpMsgConfTargetOkInCall.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */

#include "ByeMsg.hxx" 
#include "VoipOpBuilder.hxx"

using namespace Assist;

const Sptr <PbxState> VoipOpMsgConfTargetOkInCall::process( const Sptr <PbxEvent> event )
{
	cpDebug(LOG_DEBUG, "OpConfTargetOk operation" );

	// OpConfTargetOk can be called when in StateInCall, so first check
	// ua config mode for Conference setting
	if ( PbxConfiguration::instance()->getXferMode() != ConfOn )
	{
		cpDebug(LOG_DEBUG, "Not in Conference mode" );
		return 0;
	}

	Sptr <StatusMsg> msg;
	msg.dynamicCast( isSipMsg(event) );
	if ( msg == 0 )
	{
		return PBX_STATE_CONTINUE;
	}

	Sptr <SipContext> context = getSipContext(event);
	assert( context != 0 );

	//check 200 OK;
	int status = msg->getStatusLine().getStatusCode();
	if ( status != SIP_STATUS_CODE_SUCCESS_OK )
	{
		return PBX_STATE_CONTINUE;
	}

	if ( msg->getCSeq().getMethod() !=SIP_TRANSFER/*"TRANSFER"*/ )
	{
		return PBX_STATE_CONTINUE;
	}

	Sptr <Contact> contact = context->findContact( *msg );
	if ( contact == 0 )
	{
		return PBX_STATE_CONTINUE;
	}

	// Send BYE to conferencee
	status = contact->getStatus();

	SipVia sipVia;
	sipVia.setprotoVersion( "2.0" );
	sipVia.setHost( msg->getFrom().getHost() );
	sipVia.setPort( atoi( PbxConfiguration::instance()->getLocalSipPort().c_str() ) );
	sipVia.setTransport( PbxConfiguration::instance()->getSipTransport() );

	Sptr <ByeMsg> bye;
	if ( status == SIP_STATUS_CODE_SUCCESS_OK )
	{
		cpDebug(LOG_DEBUG, "BYE callee" );
		bye = new ByeMsg( contact->getStatusMsg() );
	}
	else
	{
		cpDebug(LOG_DEBUG, "BYE caller" );
		bye = new ByeMsg( contact->getInviteMsg() );
	}
	assert( bye != 0 );
	
	bye->flushViaList();
	bye->setVia( sipVia, 0 );
	unsigned int cseq = contact->getCSeqNum();
	contact->setCSeqNum( ++cseq );
	SipCSeq sipCSeq = bye->getCSeq();
	sipCSeq.setCSeq( cseq );
	bye->setCSeq( sipCSeq );

	getVoipEndPoint(event)->getSipStack()->sendAsync( *bye );

	// remove this contact from the contact list
	context->removeContact( contact );

	return PBX_STATE_CONTINUE;
}

