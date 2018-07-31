/*
* $Id: VoipOpMsgCancelByeRinging.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/
#include "CancelMsg.hxx"
#include "ByeMsg.hxx"
#include "VoipOpBuilder.hxx"

using namespace Assist;

/* process both msg CANCEL and BYE when in state of RINGING */
const Sptr <PbxState> VoipOpMsgCancelByeRinging::process( const Sptr <PbxEvent> event )
{
	Method methodType = getSipMsgType(event);

	if( methodType != SIP_CANCEL && methodType != SIP_BYE )
		return PBX_STATE_CONTINUE;

	Sptr <SipMsg> sipMsg = isSipMsg(event);
	assert( sipMsg != 0 );


	Sptr <SipContext> context = getSipContext(event);
	assert( context != 0 );

	Sptr <VoipEndPoint> vep =getVoipEndPoint(event);

	Sptr <StatusMsg> status;
	Sptr <CancelMsg> cancelMsg;
	cancelMsg.dynamicCast( sipMsg );

	Sptr <ByeMsg> byeMsg;
	byeMsg.dynamicCast( sipMsg );
#if 0
    // either a cancel or a bye message will cancel the ringing
    // and terminate the call
	if ( cancelMsg != 0 )
	{
		status = new StatusMsg(*cancelMsg, 200/*OK*/);
		contact = call->findContact( *cancelMsg );
	}
	else if ( byeMsg != 0 )
	{
		status = new StatusMsg(*byeMsg, 200/*OK*/);
		contact = call->findContact( *byeMsg );
	}
	else
	{
		return 0;
	}
	sipEvent->getSipStack()->sendReply( *status );

	Sptr < InviteMsg > msg = call->getRingInvite();
	assert( msg != 0 );
	Sptr < StatusMsg > inviteStatus = new StatusMsg(*msg, 487/*Request Terminated*/);
	sipEvent->getSipStack()->sendReply( *inviteStatus );
#endif 

	int statusno = SIP_STATUS_CODE_SUCCESS_OK;

	Sptr <Contact> contact = context->findContact( *sipMsg );;
	if ( contact == 0 )
	{
		cpDebug(LOG_DEBUG, "No known contact for this messagemake" );
		statusno = SIP_STATUS_CODE_CERROR_CALLLEG_NOT_EXIST;
	}

	Sptr <Contact> curContact = context->getContact();
	assert( curContact != 0 );
	if ( *curContact != *contact )
	{
        // CANCEL not for current contact
		cpDebug(LOG_DEBUG, "CANCEL not for current contact" );
		statusno = SIP_STATUS_CODE_CERROR_CALLLEG_NOT_EXIST;
    	}

	Sptr <SipCommand> sipCmd;
	sipCmd.dynamicCast(sipMsg);

	status = new StatusMsg(*sipCmd, statusno);
	vep->getSipStack()->sendReply( *status );

	if (statusno == SIP_STATUS_CODE_CERROR_CALLLEG_NOT_EXIST)
	{
	  	return PBX_STATE_CONTINUE;
	}

	Sptr <InviteMsg> msg = context->getRingInvite();
       assert( msg != 0 );
	Sptr < StatusMsg > inviteStatus = new StatusMsg(*msg, SIP_STATUS_CODE_CERROR_REQUEST_TERMINATED );
	vep->getSipStack()->sendReply( *inviteStatus );

	cpDebug(LOG_DEBUG, "Cancel Ringing" );

#if 0  //removed by lijie 2005-06-03
    // Remove call info from call container
    Sptr < UaCallContainer > calls;
    calls.dynamicCast( event->getCallContainer() );
    assert( calls != 0 );

    //calls->deleteCall( *(UaDevice::instance()->getCallId()) );
    SipCallId callId = call->getContact()->getInviteMsg().getCallId();
	
	calls->deleteCall( callId, event->getDeviceId() );
#endif

	if (PbxConfiguration::instance()->getProvideRingback() &&
		context->getContact()->getStatus() == SIP_STATUS_CODE_INFO_PROGRESS )
	{
	}
	else
	{
		// release the rtp port if one was reserve
		//UaDevice::instance()->releaseRtpPort( event->getDeviceId() );
	}

#if __CDR_OPEN__
	if (SimulateDB::instance() != NULL)
	{
		int deviceID = event->getDeviceId();
		SimulateDB::instance()->addCdrClient(sipMsg, deviceID, CALL_END);
	}
#endif

	sendCallSignal(event, CALL_RELEASE);

	vep->reset();
	
	return lookupEPState(event, VOIP_EP_STATE_IDLE );
//	return PBX_STATE_DONE_WITH_EVENT;
}

