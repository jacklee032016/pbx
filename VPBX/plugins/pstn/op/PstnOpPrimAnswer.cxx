/*
* $Id: PstnOpPrimAnswer.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "PstnOpBuilder.hxx"

using namespace Assist;
/*
* Catch both Primary answer and Catch Incoming (Soft onhook) from FXS
*/
const Sptr <PbxState> PstnOpPrimAnswer::process( const Sptr <PbxEvent> event )
{
	Sptr <PbxTimerEvent> timeoutEvent =  isTimeout(event);
	if ( timeoutEvent != 0)
	{
		int currentState = event->getEndPoint()->getState()->getStateType();
		if(currentState == PSTN_EP_STATE_CALLID)
		{
			cpDebug(LOG_DEBUG, "No PrimaryAnswer because of Timeout, so enter into RING" );
			return lookupEPState(event, PSTN_EP_STATE_RINGING);
		}	
		else if(currentState == PSTN_EP_STATE_INCOMING )
		{
			cpDebug(LOG_DEBUG, "No IncomingSucess(OnHook) after CAR because of Timeout, so return to IDLE" );

			/* send _CallRelease to CallScheduler */
//			return lookupEPState(event, PSTN_EP_STATE_IDLE );
			return PBX_STATE_CONTINUE;	/* let OpTimeout process this event */
		}
		else
		{
			cpLog(LOG_ERR, "%s is in invalidate state %d", name(), currentState);
			assert(0);
			return PBX_STATE_CONTINUE;
		}
	}

	DeviceEventType eventType = getPstnEventType(event);
	
	if ( eventType != DeviceEventHookUp && eventType != DeviceEventHookDown )
	{
		return PBX_STATE_CONTINUE;
	}

	if(eventType == DeviceEventHookUp)
	{	/* start to send FSK modem caller ID */
		sendCommand(event, DeviceSignalCarCallerID);
		/* restart timer for HookDown event */
		return lookupEPState(event, PSTN_EP_STATE_INCOMING);
	}
	
	/* If event is Incoming Success(an onhook event),return PstnStateRing. */
	cpDebug(LOG_DEBUG, "Incoming success, go to state ringing");
	return lookupEPState(event, PSTN_EP_STATE_RINGING);
}

