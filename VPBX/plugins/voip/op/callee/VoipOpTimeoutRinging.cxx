/*
* $Id: VoipOpTimeoutRinging.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/
#include "VoipOpBuilder.hxx"

//#include "pbx_globals.h"

using namespace Assist;

/* process Timeout event when in RINGING : send 487 */
const Sptr <PbxState> VoipOpTimeoutRinging::process( const Sptr <PbxEvent> event )
{
	if ( isTimeout(event) == false )
       	return 0;

	Sptr <SipContext> context = getSipContext(event);
	assert( context != 0 );

	Sptr <Contact> contact = context->getContact();
	assert( contact != 0 );

	Sptr <InviteMsg> msg = context->getRingInvite();
	assert( msg != 0 );

//	if (UaDevice::instance()->isMyHardware(msg->getCallId(), event->getDeviceId())) 
	StatusMsg status( *msg, SIP_STATUS_CODE_CERROR_REQUEST_TERMINATED );

	Sptr <VoipEndPoint> vep = getVoipEndPoint(event);
	vep->getSipStack()->sendReply( status );

	sendCallSignal(event, CALL_RELEASE);
	
	vep->reset();
	return lookupEPState(event, VOIP_EP_STATE_IDLE );

//	return PBX_STATE_DONE_WITH_EVENT;
}

