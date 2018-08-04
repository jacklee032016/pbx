/*
 * $Id: CallOpDisconnect.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */

#include "CallOpBuilder.hxx"

using namespace Assist;

const Sptr <PbxState> CallOpDisconnect::process( Sptr <PbxEvent> _event )
{
	if( checkCallMsg(_event) != CALL_DISCONNECT )
	{
		return PBX_STATE_CONTINUE;
	}

	/*************** 1 : get callInfo of this event */
	Sptr <CallInfo> call = getCallInfo(_event);
	if(call == 0)
		return PBX_STATE_CONTINUE;

	/*************** 2 : check who an i */
	Sptr <EndPoint>  myself = _event->getEndPoint();
	Sptr <EndPoint> src = call->getSrcEP();
	Sptr <EndPoint>  peer = call->getDestEP();

	/************** 3 : deactivate audio channel before */
	Sptr <EndPoint> srcEp = call->getSrcEP();
	Sptr <EndPoint> destEp = call->getDestEP();
	assert(srcEp!=0 && destEp!= 0);
	
	Sptr <AudioChannel> srcAudio = srcEp->getAudio();
	Sptr <AudioChannel> destAudio = destEp->getAudio();
	assert( srcAudio!= 0 && destAudio != 0);
//	cpLog(LOG_DEBUG, "CallEvent '%s' is proceesing in CallOperator '%s'", callMsg->_Describer(), name() );

	/* release audio and deactivate them */
	srcAudio->releaseAudioPeer( );

//	cpLog(LOG_DEBUG, "CallEvent '%s' is proceesing in CallOperator '%s'", callMsg->_Describer(), name() );

	/************* 4 : in order to deactivate audioChannel later than reset audioPeer, feedback ReleaseComplete for the originator */
	sendCallSignalToMgr(myself, CALL_RELEASE_COMPLETE);


	/************* 5 : forward this _CALL_DISCONNECT for peer */
	/* srcEp must be the first, because that Call can be in state of IDLE, no destEP is asssigned */
	if( src== NULL || peer==NULL )
	{
		cpLog(LOG_ERR, "No EndPoint is found for this RELEASE event");

		return call->findState(CALL_STATE_IDLE);
	}
	else if( myself== peer)		
	{
		peer = src;
	}

	/* forward DISCONNECT to peer EP */	
	forwardCallEvent(peer,  _event);

	return call->findState(CALL_STATE_IDLE);
}

