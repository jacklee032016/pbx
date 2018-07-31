/*
 * $Id: ConsoleOpInCallAnswerTimer.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */

#include "ConsoleOpBuilder.hxx"

using namespace Assist;

const Sptr <PbxState> ConsoleOpInCallAnswerTimer::process( const Sptr <PbxEvent> event )
{
	Sptr <PbxTimerEvent> timerEvent = isTimeout( event);
	if ( timerEvent == 0 )
	{
		return PBX_STATE_CONTINUE;
	}

//	cpLog(LOG_DEBUG_STACK, "'%s' is handled in %s", timerEvent->name(), name() );
	Sptr <EndPoint> master = event->getEndPoint();

	/* 1 : create a CONNECT callEvent and send to CallScheduler */
	CallEvent *callEvent = new CallEvent();
 	callEvent->setCallMsg( new _CallConnect() );
	callEvent->setCallId( master->getCallId() );
	callEvent->setEndPoint( master);

	sendSchedulerEvent(callEvent);

	Sptr <AudioChannel> audio = master->getAudio();
	assert( audio!=0);
	audio->activate();

	//setTimer(event, PbxConfiguration::instance()->getInitialDigitTimeout() );
	
	return lookupEPState(event, CONSOLE_EP_STATE_IN_GREETING );
}

