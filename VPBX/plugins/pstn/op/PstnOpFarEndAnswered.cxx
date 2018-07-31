/*
 * $Id: PstnOpFarEndAnswered.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */

#include "PstnOpBuilder.hxx"

using namespace Assist;

const Sptr <PbxState> PstnOpFarEndAnswered::process( const Sptr <PbxEvent> event )
{
	if(checkCallMsg(event) != CALL_CONNECT )
	{
		return PBX_STATE_CONTINUE;
	}

	if(! event->getEndPoint()->checkExt() )
	{/* notify FXO offhook, so stop ringback tone in peer switch */
		sendCommand(event, DeviceSignalLocalRingbackStop);
	}

	/* activate callee's audio channel before audioThread can process it */
	Sptr <AudioChannel> audio = event->getEndPoint()->getAudio();
	assert(audio != 0);
	audio->activate();

	/* reply ConnectAck which make CallScheduler enter CONNECT state and activate audioThread for my audioChannel */
	sendCallSignal(event, CALL_CONNECT_ACK);
	
	return lookupEPState(event, PSTN_EP_STATE_INCALL);
}

