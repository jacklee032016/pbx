/*
 * $Id: VoipOpMsgAckInCall.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */
#include "VoipOpBuilder.hxx"

using namespace Assist;

const Sptr <PbxState> VoipOpMsgAckInCall::process( const Sptr <PbxEvent> event )
{
	if(getSipMsgType(event) != SIP_ACK)
	{
		return PBX_STATE_CONTINUE;
	}

	Sptr <SipMsg> sipMsg = isSipMsg(event);
	assert( sipMsg != 0 );

	Sptr <SipContext> context = getSipContext(event);
	assert( context != 0 );

	Sptr <AckMsg> ack;
	ack.dynamicCast( sipMsg);
	assert( ack != 0 );
	context->setAckMsg( ack );

	// Get remote SDP from call info
	Sptr <SipSdp> remoteSdp = context->getRemoteSdp();
	if ( remoteSdp != 0 ) // && remoteSdp->isValidSdp() )
	{
	// TODO For now, do nothing regardless if ACK contains sdp.
	// TODO In the future, if ACK has sdp, we should stop the previous
	// TODO media stream and start a new one with the new sdp.
	}
	else
	{
		// Get remote SDP from message
		Sptr <SipSdp> sdp;
		sdp.dynamicCast ( sipMsg->getContentData(0) );
		if ( sdp == NULL )
		{
			cpLog( LOG_ERR, "No remote SDP in INVITE and ACK" );

			// TODO: add Accept Media type and arrange with UAc
			StatusMsg Unsupported(*ack, SIP_STATUS_CODE_CERROR_MEDIA_UNSUPPORT);
			getVoipEndPoint(event)->getSipStack()->sendReply( Unsupported );

			event->getEndPoint()->reset();
			
			return lookupEPState(event, VOIP_EP_STATE_IDLE );
		}
		else
		{
			remoteSdp = new SipSdp( *sdp );
			context->setRemoteSdp( remoteSdp );
		}
	}
	
	// TODO: check the remote sdp media, return 415 if not support 

	rsvpAckHandler(context->getLocalSdp(), context->getRemoteSdp());

	return PBX_STATE_CONTINUE;
}

