/*
 * $Id: PstnOpDeviceStatus.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */

#include "PstnOpBuilder.hxx"

using namespace Assist;

const Sptr <PbxState> PstnOpDeviceStatus::process( const Sptr <PbxEvent> event )
{
	DeviceEventType eventType = getPstnEventType(event);
	
	if ( eventType != DeviceEventBusy && eventType != DeviceEventIdle && eventType != DeviceEventRingerOff )
	{
		return PBX_STATE_CONTINUE;
	}

	Sptr <EndPoint> ep = event->getEndPoint();
	assert(ep != 0);
	if( eventType == DeviceEventRingerOff )
	{/* start send Caller ID compatible with chinese standard */
		sendCommand(event, DeviceSignalCarCallerID);
		return PBX_STATE_DONE_WITH_EVENT;
	}
	else if ( eventType == DeviceEventBusy )
	{

		Sptr <CallEvent> releaseEvent = new CallEvent();
		Sptr <_CallRelease> releaseMsg = new _CallRelease;
		releaseMsg->setCause(CALL_CAUSE_BUSY_HERE);
		releaseEvent->setCallMsg(releaseMsg);
		releaseEvent->setEndPoint(ep );

		sendSchedulerEvent(releaseEvent);
	
		ep->reset();
		return lookupEPState(event, PSTN_EP_STATE_IDLE);
	}

	/* for DeviceEventIdle */

	int state = ep->getState()->getStateType();
	if( state == PSTN_EP_STATE_CALLID )
	{
		sendCommand(event, DeviceSignalCar );

		return PBX_STATE_DONE_WITH_EVENT;
	}
	else if(state == PSTN_EP_STATE_RINGING)
	{
		sendCommand(event, DeviceSignalRingStart);

		call_method_t  method = CALL_UNKNOWN;
		if(ep->checkExt() )
		{
			method = CALL_ALERTING;
		}
		else
		{/* FXO : after dialing out, ringback is play be carrier switch. eg. reply CONNECT without ALERTING */
			method = CALL_PROCEEDING;
		}

		sendCallSignal(event, method);

		return PBX_STATE_DONE_WITH_EVENT;
	}	

	cpLog(LOG_ERR, "%s is in invalidate state", name() );
	assert(0);

	return PBX_STATE_DONE_WITH_EVENT;
}

