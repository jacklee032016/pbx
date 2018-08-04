/*
 * $Id: CallOpForwardSetupInCall.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */

#include "CallOpBuilder.hxx"

using namespace Assist;

/* in order to begin the second call, forward Setup msg when in state of CALL */
const Sptr <PbxState> CallOpForwardSetupInCall::process( Sptr <PbxEvent> _event )
{
	if( checkCallMsg(_event) != CALL_SETUP )
	{
		return PBX_STATE_CONTINUE;
	}

	Sptr <CallInfo> call = getCallInfo(_event);
	if(call == NULL)
		return PBX_STATE_CONTINUE;


	/* in fact, SETUP msg can only be send by srcEp */	
	Sptr <EndPoint> peerEp = call->getSrcEP();
	if( _event->getEndPoint() == peerEp)
	{
		peerEp = call->getDestEP();
	}
	
	forwardCallEvent( peerEp, _event);

	return PBX_STATE_DONE_WITH_EVENT;
}

