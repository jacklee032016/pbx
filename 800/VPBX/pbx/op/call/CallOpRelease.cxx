/*
 * $Id: CallOpRelease.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */
#include "CallOpBuilder.hxx"

using namespace Assist;

const Sptr <PbxState> CallOpRelease::process( Sptr <PbxEvent> _event )
{
	if( checkCallMsg(_event) != CALL_RELEASE )
	{
		return PBX_STATE_CONTINUE;
	}

	/* 1 : get callInfo of this event */
	Sptr <CallInfo> call = getCallInfo(_event);
	if(call == 0)
		return PBX_STATE_CONTINUE;

	/* 2 : check who an i */
	Sptr <EndPoint>  myself = _event->getEndPoint();
	Sptr <EndPoint> src = call->getSrcEP();
	Sptr <EndPoint>  peer = call->getDestEP();

	/* srcEp must be the first, because that Call can be in state of IDLE, no destEP is asssigned */
	if( myself == src )
	{
		//peer = call->getDestEP();
	}
	else if( myself == peer)
	{
		peer = src;
	}
	else
	{
		cpLog(LOG_ERR, "No EndPoint is found for this RELEASE event");
		return PBX_STATE_DONE_WITH_EVENT;
	}

	if(peer != NULL)
	{	
		/* forward RELEASE to peer EP */	
		forwardCallEvent( peer, _event);
	}

	/* no synchronizatin needed when remove callId */
//	sendCallSignalToMgr(myself, CALL_RELEASE_COMPLETE);

	return call->findState(CALL_STATE_IDLE);
}

