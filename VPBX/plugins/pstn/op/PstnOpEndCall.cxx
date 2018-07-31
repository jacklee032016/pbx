/*
* $Id: PstnOpEndCall.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "PstnOpBuilder.hxx"

using namespace Assist;

const Sptr <PbxState> PstnOpEndCall::process( const Sptr <PbxEvent> event )
{
	if(checkCallMsg(event) != CALL_DISCONNECT )
	{
		return PBX_STATE_CONTINUE;
	}

	Sptr <EndPoint> ep = event->getEndPoint();
	assert(ep!=0);
	
	Sptr <AudioChannel> audio = ep->getAudio();
	assert(audio!=0);
	audio->deactivate();

	/* for FXO, it is on-hook; for FXS, it is busy tone */
	sendCommand(event, DeviceSignalFastBusyStart );

	Sptr <PbxState> nextState;
	if (!ep->checkExt())
	{/* FXO */
		ep->reset();
		nextState = lookupEPState(event,  PSTN_EP_STATE_IDLE);
	}
	else
	{/* FXS : to play busy tone */
		nextState = lookupEPState(event, PSTN_EP_STATE_ERROR);
	}
	return nextState;
}

