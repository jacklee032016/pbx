/*
 * $Id: CallOpConnectAckBeforeConnect.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */

#include "CallOpBuilder.hxx"

using namespace Assist;

/* process ConnectAck before enter into state of CONNECT. for the purpose of activate audio thread */
const Sptr <PbxState> CallOpConnectAckBeforeConnect::process( Sptr <PbxEvent> _event )
{
	if( checkCallMsg(_event) != CALL_CONNECT_ACK )
	{
		return PBX_STATE_CONTINUE;
	}
	
	Sptr <CallInfo> call = getCallInfo(_event);
	if(call == 0)
		return PBX_STATE_CONTINUE;

	Sptr <EndPoint> srcEp = call->getSrcEP();
	Sptr <EndPoint> destEp = call->getDestEP();
	assert(srcEp!=0 && destEp!= 0);
	
	Sptr <AudioChannel> srcAudio = srcEp->getAudio();
	Sptr <AudioChannel> destAudio = destEp->getAudio();
	assert( srcAudio!= 0 && destAudio != 0);

	srcAudio->setAudioPeer( destAudio);

	return call->findState( CALL_STATE_CONNECT );
}

