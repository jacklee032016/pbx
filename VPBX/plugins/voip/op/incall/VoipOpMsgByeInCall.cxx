/*
* $Id: VoipOpMsgByeInCall.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/
#include "ByeMsg.hxx"
#include "VoipOpBuilder.hxx"

using namespace Assist;

/* process BYE msg when in INCALL */
const Sptr <PbxState> VoipOpMsgByeInCall::process( const Sptr <PbxEvent> event )
{
	if(getSipMsgType(event) != SIP_BYE )
	{
		return PBX_STATE_CONTINUE;
	}

	Sptr <ByeMsg> msg;
	msg.dynamicCast( isSipMsg(event) );
	assert( msg != 0 );

	int statusno = SIP_STATUS_CODE_SUCCESS_OK;

	Sptr <SipContext> context = getSipContext(event);
	assert( context != 0 );
	Sptr <VoipEndPoint> vep = getVoipEndPoint(event);

	Sptr <Contact> curContact;
	Sptr <Contact> byeContact;

	curContact = context->getContact();
	assert( curContact != 0 );
	byeContact = context->findContact( *msg );

	if ( byeContact == 0 )
	{
		statusno = SIP_STATUS_CODE_CERROR_TMP_UNAVAILABLE;
		cpDebug(LOG_DEBUG, "Bye message is not for a known contact" );
	}

	if ( *curContact != *byeContact )
	{
		statusno = SIP_STATUS_CODE_CERROR_TMP_UNAVAILABLE;
		cpDebug(LOG_DEBUG, "Not current contact" );
	}

	if (msg->getCSeq().getCSeq() <= context->getRingInvite()->getCSeq().getCSeq())
	{
		statusno = SIP_STATUS_CODE_CERROR_BAD_REQUEST;	
	}

	StatusMsg status( *msg, SIP_STATUS_CODE_SUCCESS_OK /* statusno*/ );
	
	vep->getSipStack()->sendReply( status );

	//if (statusno != 200) return 0;

    // Show caller information
#if 0    
	if ( !UaCommandLine::instance()->getBoolOpt( "voicemail" ) )
	{
		cout << endl << VTime::strftime("%y/%m/%d %H:%M:%S")
			<< " Call end " << msg->getFrom().encode().logData() << endl;
	}
	else
#endif		
	{
		cpDebug(LOG_DEBUG, "Call end: %s", msg->getFrom().encode().logData());
	}

#ifdef HAS_RSVPLIB
    // RSVP stuff ported from ol OpEndCall.cxx
	if ( PbxConfiguration::instance()->getRsvpOn() )
	{
		UaDevice::instance()->getSuaRsvpSender().close();
		UaDevice::instance()->getSuaRsvpReserv().close();
	}
#endif

#ifndef NO_POLICY
	if ( PbxConfiguration::instance()->getUsePolicyServer() )
	{
		cpDebug(LOG_DEBUG,"Disable QoS handled by Marshal");
	}
#endif


#if WITH_CALLWAIT
	int nextState = VOIP_EP_STATE_CALL_ENDED;

	SipCallId callId = msg->getCallId();
	// do the following only if the call is on call waiting
	if ( UaDevice::instance()->isOnCallWaiting( callId, event->getDeviceId() ) )
	{
		// delete the call from call container
		calls->deleteCall( callId, event->getDeviceId() );
		UaDevice::instance()->removeCallWaitingId( callId,  event->getDeviceId() );
		nextState = VOIP_EP_STATE_IDLE;
		cpDebug(LOG_DEBUG, "\nno more calls on call waiting");

	}
#endif
	
#if __CDR_OPEN__  
	if (SimulateDB::instance() != NULL)
	{
		int deviceID = event->getDeviceId();
		SimulateDB::instance()->addCdrClient(msg, deviceID, CALL_END);
	}
#endif

	sendCallSignal(event, CALL_DISCONNECT);

//	return lookupEPState(event, VOIP_EP_STATE_IDLE);
	return PBX_STATE_DONE_WITH_EVENT;
}

