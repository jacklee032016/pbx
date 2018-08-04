/*
 * $Id: ConsoleOpInCallReDial.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */

#include "ConsoleOpBuilder.hxx"

using namespace Assist;

const Sptr <PbxState> ConsoleOpInCallReDial::process( const Sptr <PbxEvent> event )
{
	Sptr <DigitEvent> digitEvent = isDigit(event);
	if ( digitEvent == 0 )
	{
		return PBX_STATE_CONTINUE;
	}

	setAudioChannel(event, console_audio_silence);

	Sptr <EndPoint> master = getMasterEndPoint(event);
	Sptr <EndPoint> slave = getSlaveEndPoint(event);

	if( slave->getCallId()==0)
		slave->setCallId( new CallId() );

	/* send a SETUP CallEvent to CallScheduler */
	CallEvent *callEvent = new CallEvent();
	CommonMsg *callMsg = new _CallSetup();
	callMsg->setTo( master->getDialing().c_str() );
	callMsg->setFrom((const char *) master->getPeerNumber().c_str() );

 	callEvent->setCallMsg(callMsg) ;
	callEvent->setCallId( slave->getCallId() );
	callEvent->setEndPoint( slave );

	/*Master enter into CONNECT state */
	Sptr <PbxState> newState = master->findState( CONSOLE_EP_STATE_CONNECT);
	master->setState(newState );

	/* Slave enter into OUT_CALLING state */
	Sptr <PbxState> newSlaveState = slave->findState( CONSOLE_EP_STATE_OUT_CALLING);
	slave->setState(newSlaveState );

	/* require CallScheduler begin the second call */
	sendSchedulerEvent( callEvent);
	/* startup a timerEvent in Slave */
//	setTimer( callEvent, PbxConfiguration::instance()->getConsoleCallingTimeout() );

	return newState;
}

