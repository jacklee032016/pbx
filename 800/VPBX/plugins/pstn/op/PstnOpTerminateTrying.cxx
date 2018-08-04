/*
 * $Id: PstnOpTerminateTrying.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */

#include "PstnOpBuilder.hxx"
#include "PbxTimerEvent.hxx"

using namespace Assist;

/* just exit operator of state TRYING, so RELEASE and other special operaros can not be send out */
const Sptr <PbxState> PstnOpTerminateTrying::process( Sptr <PbxEvent> event )
{
	sendCommand(event, DeviceSignalLocalRingbackStop);

#if 0
	if(!isTimeout(event))
	{
		DeviceEventType eventType = getPstnEventType(event);
		
		if ( eventType != DeviceEventHookDown )
		{
			return PBX_STATE_CONTINUE;
		}
	}
	else	
	{   
		Sptr <PbxTimerEvent> timerEvent;
		timerEvent.dynamicCast( event );
		if ( timerEvent == 0 )
		{
			return PBX_STATE_CONTINUE;
		}
	}

	sendCallSignal(event, CALL_RELEASE);
#endif	
	return PBX_STATE_CONTINUE;
}

