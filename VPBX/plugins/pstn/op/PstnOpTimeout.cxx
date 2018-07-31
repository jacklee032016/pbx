/*
 * $Id: PstnOpTimeout.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */

#include "PstnOpBuilder.hxx"
#include "EpTimerEvent.hxx"
#include "DigitTimeoutEvent.hxx"

using namespace Assist;

const Sptr <PbxState> PstnOpTimeout::process( const Sptr <PbxEvent> event )
{
	Sptr <DigitTimeoutEvent> digitTimer = NULL;
	digitTimer.dynamicCast( event);
	
	if(isTimeout(event) == 0 && digitTimer == 0 )
		return PBX_STATE_CONTINUE;

	Sptr <EndPoint> ep = event->getEndPoint();
	Sptr <PstnEndPoint> pep;
	pep.dynamicCast(ep);
	assert(pep != 0);

	Sptr <PbxState> nextState;
//If onhook,when timeout,just go to idle.
//if offhook,when timeout,go to error and listen busy tone.
	if (pep->getMyDev()->state == AS_DEVICE_STATE_ONHOOK)
	{
		ep->reset();
		
		nextState = lookupEPState(event,  PSTN_EP_STATE_IDLE);
		assert(nextState != 0);
	}
	else
	{
		nextState = lookupEPState(event, PSTN_EP_STATE_ERROR);
		assert(nextState != 0);
	}
	
	return nextState;
}

