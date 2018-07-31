/*
 * $Id: ConsoleOpOutCallRelease.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */

#include "ConsoleOpBuilder.hxx"

using namespace Assist;

const Sptr <PbxState> ConsoleOpOutCallRelease::process( const Sptr <PbxEvent> event )
{
	if(checkCallMsg(event) != CALL_RELEASE )
	{
		return PBX_STATE_CONTINUE;
	}

	Sptr <EndPoint> slave = getSlaveEndPoint( event);

	/* 1 : send release to peer */

#if 0
	Sptr <CallEvent> callEvent= NULL;
	callEvent.dynamicCast(event);
	assert(callEvent != 0);
	/* forward this msg to Master by Gateway */
	callEvent->setCallId( ctrl->getMasterEndPoint()->getCallId() );
	callEvent->setEndPoint( ctrl->getMasterEndPoint() );
	PbxAgent::instance()->getGateway()->reportEvent( callEvent);
#else
	/* begin Master operation directly */
	Sptr <ConsoleEndPoint> master = getMasterEndPoint(event);
	
	CallEvent *cEvent = new CallEvent();
 	cEvent->setCallMsg( new _CallDisConnect() );
	cEvent->setCallId( master->getCallId() );
	cEvent->setEndPoint( master);
	sendSchedulerEvent(cEvent);
	
	master->reset();
	master->setState( master->findState(CONSOLE_EP_STATE_IDLE) );

#endif

	slave->reset();
	/* return to IDLE */
	return slave->findState( CONSOLE_EP_STATE_IDLE );
}

