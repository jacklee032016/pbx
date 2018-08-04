/*
 * $Id: PstnOpStartCall.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */

#include "PstnOpBuilder.hxx"

using namespace Assist;

const Sptr <PbxState> PstnOpStartCall::process( const Sptr <PbxEvent> event )
{
	DeviceEventType eventType = getPstnEventType(event);
	
	if ( eventType != DeviceEventHookUp && eventType != DeviceEventCar )
	{
		return PBX_STATE_CONTINUE;
	}

	Sptr <EndPoint> ep = event->getEndPoint();
	
	if( ep->getCallId() == 0)
	{
		ep->setCallId(new CallId);
	}
	else
	{
		cpDebug(LOG_WARNING, "CallId has allocated to PstnEndPoint %s", ep->getName() );
	}
	assert( ep->getCallId() != 0);
	cpLog(LOG_DEBUG, "callid in PstnEndPoint %s is %s", ep->getName(), ep->getCallId()->encode().logData());
	
	if ( eventType == DeviceEventCar )
	{
		cpLog( LOG_DEBUG, "DeviceEventCAR arrived" );
		return lookupEPState(event, PSTN_EP_STATE_CAR);
	}

	cpLog( LOG_DEBUG, "DeviceEventHookUp arrived" );
	return lookupEPState(event, PSTN_EP_STATE_DIALING);
}

