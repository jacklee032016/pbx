/*
 * $Id: CallOpProceeding.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */
#include "CallOpBuilder.hxx"

using namespace Assist;

const Sptr <PbxState> CallOpProceeding::process( Sptr <PbxEvent> _event )
{
//	call_cause_t err = CALL_CAUSE_NO_ERROR;

	if( checkCallMsg(_event)  != CALL_PROCEEDING )
	{
		return PBX_STATE_CONTINUE;
	}

	/*
	* STEP 2 : get CallInfo for this callEvent 
	* STEP 1 and 2 are common for all CallEvent 
	*/
	Sptr <CallInfo> call = getCallInfo(_event);
	if(call == 0)
		return PBX_STATE_CONTINUE;

	/* STEP 3 : CallEvent process here */
	if(call->getState()->getStateType() == CALL_STATE_PROCEEDING)
		return PBX_STATE_DONE_WITH_EVENT;

	/* otherwise forward PROCEEDING to src EP later */	
	Sptr <EndPoint> srcEp = call->getSrcEP();

	forwardCallEvent(srcEp, _event);
	
	return call->findState(CALL_STATE_PROCEEDING);
}


