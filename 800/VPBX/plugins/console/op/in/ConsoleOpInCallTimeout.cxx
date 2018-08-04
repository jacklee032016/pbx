/*
 * $Id: ConsoleOpInCallTimeout.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */

#include "ConsoleOpBuilder.hxx"

#include "DigitTimeoutEvent.hxx"

using namespace Assist;

/* for both PbxTimerEvent and DigitTimerEvent (also represent wrong dial) , only for Master EP */
const Sptr <PbxState> ConsoleOpInCallTimeout::process( const Sptr <PbxEvent> event )
{
	assert(event!=0);

	Sptr <DigitTimeoutEvent> digitTimer = NULL;
	digitTimer.dynamicCast( event);
	
	if(isTimeout(event) == 0 && digitTimer == 0 )
		return PBX_STATE_CONTINUE;

	cpLog(LOG_DEBUG_STACK, "'%s' is handled in %s", event->name(), name() );
	Sptr <EndPoint> master = getMasterEndPoint(event);

	/* 1 : notify CallScheduler to release CallInfo */
	CallEvent *callEvent = new CallEvent();
 	callEvent->setCallMsg( new _CallDisConnect() );
	callEvent->setCallId( master->getCallId() );
	callEvent->setEndPoint( master);

	sendSchedulerEvent(callEvent);

	master->reset();
	
	return lookupEPState(event, CONSOLE_EP_STATE_IDLE );
}

