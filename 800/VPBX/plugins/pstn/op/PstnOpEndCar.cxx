/*
 * $Id: PstnOpEndCar.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */

#include "PstnOpBuilder.hxx"

using namespace Assist;

/* rx last CAR signal and begin the primaryAnswer to demodulate FSK modem */
/* FXO rx CAR signal when in CAR state */
const Sptr <PbxState> PstnOpEndCar::process( const Sptr <PbxEvent> event )
{
	DeviceEventType eventType = getPstnEventType(event);
	
	if ( eventType != DeviceEventCar && eventType != DeviceEventIncomingEnd )
	{
		return PBX_STATE_CONTINUE;
	}

	if(eventType == DeviceEventCar)
	{/* send DialToneStart signal on FXO port  in order to offhook */
		sendCommand(event, DeviceSignalFxoPrimAnswer);
	}
	else
	{/* IncomingEnd signal : send DialToneStop signal on FXO port in order to onhook */
		sendCommand(event, DeviceSignalFxoIncomingSucess);
	}

	/* not continue so OpStartCall can only response with Hookup in State of CAR */
	return PBX_STATE_DONE_WITH_EVENT;
}

