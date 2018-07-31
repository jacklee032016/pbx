/*
 * $Id: PbxOperator.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */

#include "global.h"

#include "PbxStateObject.hxx"
#include "EndPoint.hxx"
#include "CallInfo.hxx"

#include "PbxEvent.hxx"
#include "CallEvent.hxx"
#include "PbxTimerEvent.hxx"
#include "CommonMsg.hxx"

#include "PbxOperator.hxx"

#include "PbxAgent.hxx"

using namespace Assist;

PbxOperator::PbxOperator(const string& name)
{
	myName = name;
}

PbxOperator::~PbxOperator()
{}

const char* const PbxOperator::name() const
{
	return myName.c_str();
}

void PbxOperator::doCpLog(const Sptr <PbxEvent> event ) const
{
	cpLog( LOG_DEBUG, "Operator: %s on Event %s", name(), event->name() );
}

#if 0
bool PbxOperator::setTimer( const Sptr <PbxEvent> event, unsigned int timeout )
{
	if ( timeout <= 0 ) 
		return false;

	Sptr <PbxStateObject> obj = NULL;
	Sptr <PbxState> state = NULL;
	Sptr <CallEvent> callEvent = NULL;
	callEvent.dynamicCast( event );

	if(callEvent != 0)
	{/* event for a CallSchduler */
	 	obj = event->getStateObject() ;
	}
	else
	{/* event for DeviceMgr */
		obj = event->getEndPoint();
	}
	assert( obj != 0 );

	state = obj->getState();
	assert(state != 0);
	
	Sptr < Fifo < Sptr <PbxEvent> > >  myQ =event->getQueue();
	assert(myQ!=0);

	Sptr <PbxTimerEvent>  timerEvent = new PbxTimerEvent( myQ );

	/* send this StateObject into TimerEvent, where callInfo is assigned to myStateObjec, EndPoint is assigned to myEndPoint  */
	state->setMyStateObject( timerEvent,  obj);

	cpLog(LOG_DEBUG, "Start TIMER for %d s.", timeout );

	// Save the timer event in callInfo or EndPoint
	obj->setTimer ( timerEvent );
//	timerEvent->setCallInfo( call, event->getCallContainer() );

	timerEvent->startTimer(timerEvent, timeout*1000 );

	return true;
}

bool PbxOperator::cancelTimer( const Sptr <PbxEvent> event )
{
	Sptr <PbxStateObject> obj = NULL;
	Sptr <PbxState> state = NULL;
	
	Sptr <CallEvent> callEvent = NULL;
	callEvent.dynamicCast( event );

	if(callEvent != NULL)
	{/* event for a CallSchduler */
	 	obj = event->getStateObject() ;
	}
	else
	{/* event for DeviceMgr */
		obj = event->getEndPoint();
	}
	assert( obj != 0 );

	if ( obj->getTimer() != 0 )
	{
		cpLog( LOG_DEBUG, "Cancelling timer." );
		obj->getTimer()->cancelTimer();
		return true;
	}
	
	cpLog( LOG_ERR, "No timer was found." );

	return false;
}

bool PbxOperator::isTimeout( const Sptr <PbxEvent> event )
{
	Sptr <PbxTimerEvent> 	timerEvent;
	timerEvent.dynamicCast( event );

	if ( timerEvent != 0 )
	{
		return true;
	}
	else
	{
		return false;
	}

	return true;
}
#endif

Sptr <CallEvent> PbxOperator::isCallEvent( const Sptr <PbxEvent> event)
{
	Sptr <CallEvent> callEvent;
	
	assert( event!= 0);
	doCpLog(event );
	
	callEvent.dynamicCast( event );
	if ( callEvent == 0 )
	{
		return NULL;
	}
	return callEvent;

}

Sptr <CommonMsg> PbxOperator::isCallMsg( const Sptr <PbxEvent> event)
{
	Sptr <CallEvent> callEvent = isCallEvent(event);
	
	if ( callEvent == 0 )
	{
		return NULL;
	}

	Sptr <CommonMsg> commMsg = callEvent->getCallMsg();
	assert( commMsg != 0 );
	cpLog(LOG_DEBUG, "'%s' is handled in %s", commMsg->_Describer(), name() );

	return commMsg;
}

call_method_t PbxOperator::checkCallMsg( const Sptr <PbxEvent> event)
{
	Sptr <CommonMsg> msg = isCallMsg(event);
	if( msg != 0 )
		return msg->getMethod();

	return CALL_UNKNOWN;
}

void PbxOperator::sendGatewayEvent(Sptr <PbxEvent> event)
{
	PbxAgent::instance()->getGateway()->reportEvent( event);
}

