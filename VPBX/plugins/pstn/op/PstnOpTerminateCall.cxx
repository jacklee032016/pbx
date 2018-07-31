/*
* $Id: PstnOpTerminateCall.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "PstnOpBuilder.hxx"

using namespace Assist;

const Sptr <PbxState> PstnOpTerminateCall::process( const Sptr <PbxEvent> event )
{
	DeviceEventType eventType = getPstnEventType(event);
	
	if ( eventType != DeviceEventHookDown )
	{
		return PBX_STATE_CONTINUE;
	}

	cpLog(LOG_DEBUG, "-----EP %s Hookup", event->getEndPoint()->getPhoneNumber());


	sendCallSignal(event, CALL_DISCONNECT);

	/* wait CallReleaseComplete and than deactivate audio channel*/
	return PBX_STATE_DONE_WITH_EVENT;
}

