/*
 * $Id: CallOpConnectAckInConnect.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */

#include "PbxAgent.hxx"
#include "CallContainer.hxx"

#include "CallOpBuilder.hxx"

using namespace Assist;

/* process ConnectAck in state of CONNECT. only used in the second call of console's master EndPoint  */
const Sptr <PbxState> CallOpConnectAckInConnect::process( Sptr <PbxEvent> _event )
{
	if( checkCallMsg(_event) != CALL_CONNECT_ACK )
	{
		return PBX_STATE_CONTINUE;
	}

	Sptr <_CallConnectAck> connAckMsg = NULL;
	connAckMsg.dynamicCast( isCallMsg( _event));
	if( connAckMsg==0) /* not the msg handled in this operator */
	{	
		cpLog(LOG_ERR, "No CONNECT_ACK CallEvent in this callEvent");
		return PBX_STATE_CONTINUE;
	}
	

	Sptr <EndPoint> masterEp = connAckMsg->getMasterEndPoint();
	Sptr <EndPoint> slaveEp = connAckMsg->getSlaveEndPoint();

	assert( masterEp != 0 && slaveEp != 0);
	
	Sptr <CallContainer> calls = PbxAgent::instance()->getScheduler()->getCallContainer();
	assert( calls != 0);
	
	/* get callInfo for both end of console */
	Sptr <CallInfo>  firstCall = calls->findCall( masterEp->getCallId() );
	if( firstCall == 0)
		return PBX_STATE_CONTINUE;

	Sptr <CallInfo> secondCall = calls->findCall( slaveEp->getCallId() );
	assert(secondCall != 0);
	
	/* replace the DestEP of firstCall with DestEP in secondCall */
	Sptr <EndPoint>  destEp = secondCall->getDestEP();
	assert(destEp!= 0);
	destEp->setCallId(firstCall->getCallId() );
	firstCall->setDestEP( destEp );

	/* replace audio channel in srcEp and dest Ep */
	Sptr <EndPoint> srcEp = firstCall->getSrcEP();
	assert(srcEp != 0);

	Sptr <AudioChannel> srcAudio = srcEp->getAudio();
	assert(srcAudio != 0);
	Sptr <AudioChannel> destAudio = destEp->getAudio();
	assert(destAudio != 0);

	/* mutex protected should be enhanced here , lizhijie, 2006.06.04 */
	srcAudio->releaseAudioPeer();/* free connect with masterEP's audio */
	srcAudio->setAudioPeer( destAudio); /* reconnect with dest's audio */

	/* remove secondCall. because Slave not feedback ConnectAck for this second call, 
	so this call is always in state of little than CONNECT */
//	calls->removeCall( secondCall->getCallId() );
	secondCall->setState(secondCall->findState(CALL_STATE_IDLE));

	/* return CONNECT_ACK to masterEp, because of CONNECT_ACK is send with MasterEP */
	sendGatewayEvent(_event);
	
	return PBX_STATE_CONTINUE;
}

