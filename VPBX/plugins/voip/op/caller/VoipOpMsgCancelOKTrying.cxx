/*
* $Id: VoipOpMsgCancelOKTrying.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/
#include "VoipOpBuilder.hxx"

#include "AckMsg.hxx"
using namespace Assist;

/* process status msg response for CANCEL send by myself when in state of TRYING */
const Sptr <PbxState> VoipOpMsgCancelOKTrying::process( const Sptr <PbxEvent> event )
{
	Sptr <StatusMsg> msg = isStatusMsg(event);
	if ( msg == 0 )
	{
		return PBX_STATE_CONTINUE;
	}

	if(msg->getStatusLine().getStatusCode() > SIP_STATUS_CODE_SUCCESS_OK)
	{
		return PBX_STATE_CONTINUE;
	}

	Sptr <VoipEndPoint> vep = getVoipEndPoint(event);
	Sptr <SipContext> context = getSipContext(event);
	assert( context != 0 );

	Sptr <Contact> findContact = context->findContact( *msg );
	cpDebug(LOG_DEBUG, "findContact = %s\n", findContact->computeCallLeg().encode().logData() );

	Sptr <Contact> getContact = context->getContact();
	cpDebug(LOG_DEBUG, "getContact = %s\n", getContact->computeCallLeg().encode().logData() );

	if ( *( context->findContact( *msg ) ) != *( context->getContact() ) )
	{
		cpDebug(LOG_DEBUG, "Not for the current call" );
		return PBX_STATE_CONTINUE;
	}

	context->getContact()->update( *msg);
	int status = msg->getStatusLine().getStatusCode();

#if 0
	if ( status == SIP_STATUS_CODE_SUCCESS_OK )
	{
		// Needs to ack farend
		AckMsg ack( *msg );
		vep->getSipStack()->sendAsync( ack );
	}
	if ( status == SIP_STATUS_CODE_SUCCESS_OK )
	{
		return PBX_STATE_DONE_WITH_EVENT;
	}
#endif

	if ( status == SIP_STATUS_CODE_SUCCESS_OK)//SIP_STATUS_CODE_CERROR_REQUEST_TERMINATED )
	{
		vep->reset();
		return lookupEPState(event, VOIP_EP_STATE_IDLE);
	}
	return PBX_STATE_DONE_WITH_EVENT;
}

