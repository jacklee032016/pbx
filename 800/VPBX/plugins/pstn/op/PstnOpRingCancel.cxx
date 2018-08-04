/*
* $Id: PstnOpRingCancel.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "PstnOpBuilder.hxx"

using namespace Assist;

const Sptr <PbxState> PstnOpRingCancel::process( const Sptr <PbxEvent> event )
{
	call_method_t method = checkCallMsg(event);
	if(method != CALL_RELEASE && method != CALL_DISCONNECT)
	{
		cpDebug (LOG_DEBUG, "Not Disconnect or release event");
		return PBX_STATE_CONTINUE;
	}

	sendCommand(event, DeviceSignalRingStop);	

	cpLog(LOG_DEBUG, "Ring Canceled");

	event->getEndPoint()->reset();
	return lookupEPState(event, PSTN_EP_STATE_IDLE);
}

