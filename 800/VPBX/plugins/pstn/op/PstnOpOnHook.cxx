/*
 * $Id: PstnOpOnHook.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */

#include "PstnOpBuilder.hxx"

using namespace Assist;

const Sptr <PbxState> PstnOpOnHook::process( const Sptr <PbxEvent> event )
{
	DeviceEventType eventType = getPstnEventType(event);
	
	if ( eventType != DeviceEventHookDown )
	{
		return PBX_STATE_CONTINUE;
	}
	
	sendCommand(event, DeviceSignalBusyStop);

	Sptr <EndPoint> ep = event->getEndPoint();
	assert(ep != 0);
	int state = ep->getState()->getStateType();
	
	if(state == PSTN_EP_STATE_ERROR)
	{/* After rx DISCONNECT from scheduler and wait hook-on event */
		ep->reset();
		
		return lookupEPState(event, PSTN_EP_STATE_IDLE);
	}
	else if( state == PSTN_EP_STATE_DIALING )
	{/*DIALING state : no callInfo is known to call scheduler */
#if 0
	if( state == PSTN_EP_STATE_IDLE )
	if( state == PSTN_EP_STATE_TRYING)
#endif
		ep->reset();
		return lookupEPState(event, PSTN_EP_STATE_IDLE);
	}

	/* wait reply from scheduler to remove callId and CallInfo */
	sendCallSignal(event, CALL_RELEASE);
//	return PBX_STATE_DONE_WITH_EVENT;
	ep->reset();
	return lookupEPState(event, PSTN_EP_STATE_IDLE);
}

