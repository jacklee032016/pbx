/*
 * $Id: ConsoleOpInCallSetup.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */
 
#include "ConsoleOpBuilder.hxx"

#include "PbxAgent.hxx"
#include "PbxConfiguration.hxx"

using namespace Assist;

/*
* when EP is in IDLE, receive SETUP call event from CallScheduler 
* and feedback a ALERTING call to CallSchduler, begin a timer
*/
const Sptr <PbxState> ConsoleOpInCallSetup::process( const Sptr <PbxEvent> event )
{
	Sptr <CommonMsg> commMsg = isCallMsg( event);
	if(commMsg == 0)
		return PBX_STATE_CONTINUE;
	if(commMsg->getMethod() != CALL_SETUP )
	{
		return PBX_STATE_CONTINUE;
	}

	Sptr <CallEvent> callEvent = isCallEvent(event);
	if(callEvent == 0)
		return PBX_STATE_CONTINUE;

	event->getEndPoint()->setPeerSignalType( commMsg->getPeerSignalType() );
	
	cpLog(LOG_DEBUG_STACK, "'%s' is handled in %s", commMsg->_Describer(), name() );
	Sptr <EndPoint> master =  getMasterEndPoint(event);
	
	master->setPeerNumber( commMsg->getFrom() );
	
	/* feedback ALERTING call to call scheduler */
 	callEvent->setCallMsg( new _CallAlerting() );
	callEvent->setCallId( master->getCallId() );
	callEvent->setEndPoint( master);

	sendSchedulerEvent( callEvent);

	/* start timer for connect */
	Sptr <PbxTimerEvent> timerEvent = new PbxTimerEvent(PbxAgent::instance()->getGateway()->getQueue() );
	timerEvent->setEndPoint( master);
	timerEvent->startTimer(timerEvent, PbxConfiguration::instance()->getAnswerRestartTimer() );

	return master->findState( CONSOLE_EP_STATE_IN_ALERTING);
}

