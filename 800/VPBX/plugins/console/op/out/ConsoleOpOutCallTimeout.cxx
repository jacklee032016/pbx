/*
 * $Id: ConsoleOpOutCallTimeout.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */

#include "ConsoleOpBuilder.hxx"

using namespace Assist;

const Sptr <PbxState> ConsoleOpOutCallTimeout::process( const Sptr <PbxEvent> event )
{
	if(isTimeout(event) == 0)
		return PBX_STATE_CONTINUE;

	cpLog(LOG_DEBUG_STACK, "'%s' is handled in %s", event->name(), name() );
	Sptr <EndPoint> slave = event->getEndPoint();

	/* 1 : notify CallScheduler to release CallInfo */
	CallEvent *callEvent = new CallEvent();
 	callEvent->setCallMsg( new _CallDisConnect() );
	callEvent->setCallId( slave->getCallId() );
	callEvent->setEndPoint( slave);
	sendSchedulerEvent(callEvent);

	/* notify master */
	Sptr <ConsoleEndPoint> master = getMasterEndPoint( event);
	
	CallEvent *cEvent = new CallEvent();
 	cEvent->setCallMsg( new _CallDisConnect() );
	cEvent->setCallId( master->getCallId() );
	cEvent->setEndPoint( master);
#if 0
	PbxAgent::instance()->getGateway()->reportEvent(cEvent);
#else
	sendSchedulerEvent(cEvent);

	master->reset();
	master->setState( master->findState(CONSOLE_EP_STATE_IDLE) );
#endif

	slave->reset();
	return slave->findState( CONSOLE_EP_STATE_IDLE );
}

