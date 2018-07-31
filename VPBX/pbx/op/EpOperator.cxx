/*
 * $Id: EpOperator.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */

#include "global.h"

#include "EpOperator.hxx"
#include "PbxDeviceEvent.hxx"
#include "CallId.hxx"
#include "PbxConfiguration.hxx"

#include "PbxTimerEvent.hxx"
#include "DigitEvent.hxx"
#include "CallEvent.hxx"
#include "CallMsgBuilder.hxx"

#include "PbxAgent.hxx"

using namespace Assist;

EpOperator::EpOperator(const string& name)
	:PbxOperator(name)
{}

EpOperator::~EpOperator()
{}

/* only for EndPoint*/
bool EpOperator::setTimer( const Sptr <PbxEvent> event, unsigned int ms )
{
//	cpLog(LOG_DEBUG, "Start TIMER for %d ms......", ms );
	if ( ms <= 0 ) 
		return false;

	Sptr <EndPoint> ep = NULL;
	ep = event->getEndPoint();
	assert( ep != 0 );

	Sptr <PbxTimerEvent>  timerEvent = new PbxTimerEvent( PbxAgent::instance()->getGatewayQueue() );
	timerEvent->setEndPoint( ep);

	cpLog(LOG_DEBUG, "Start TIMER for %d ms.", ms );

	/* Save the timer event in EndPoint*/
	ep->setTimer ( timerEvent );

	timerEvent->startTimer(timerEvent, ms );
	return true;
}

bool EpOperator::cancelTimer( const Sptr <PbxEvent> event )
{
	Sptr <EndPoint> ep = NULL;
	ep = event->getEndPoint();
	assert(ep != 0);
	
	if ( ep->getTimer() != 0 )
	{
		cpLog( LOG_DEBUG, "Cancelling timer." );
		ep->getTimer()->cancelTimer();
		ep->removeTimer();
		return true;
	}
	
	cpLog( LOG_ERR, "No timer was found." );
	return false;
}

Sptr <PbxTimerEvent> EpOperator::isTimeout( const Sptr <PbxEvent> event )
{
	Sptr <PbxTimerEvent> 	timerEvent;
	assert(event!=0);
	doCpLog(event);
	
	timerEvent.dynamicCast( event );

	return timerEvent;
#if 0
	if ( timerEvent != 0 )
	{
		return true;
	}
	else
	{
		return false;
	}

	return true;
#endif	
}

Sptr <DigitEvent> EpOperator::isDigit( const Sptr <PbxEvent> event )
{
	assert(event!=0);
	doCpLog(event);

	Sptr <DigitEvent> digitEvent;
	digitEvent.dynamicCast( event );
	return digitEvent;
}

void EpOperator::sendSchedulerEvent(Sptr <PbxEvent> event)
{
	PbxAgent::instance()->getScheduler()->reportEvent( event);
}

void EpOperator::sendCallSignal(Sptr <PbxEvent> event, call_method_t cmd)
{
	Sptr <EndPoint> ep = event->getEndPoint();
	assert(ep != 0);
	
	Sptr <CallEvent> callEvent = new CallEvent();
	Sptr <CommonMsg> callMsg;

	switch(cmd)
	{
		case CALL_SETUP_ACK:
		{
			callMsg = new _CallSetupAck;
			break;
		}

		case CALL_INFORMATION:
		{
			callMsg = new _CallInformation;
			break;
		}
		case CALL_PROCEEDING:
		{
			callMsg = new _CallProceeding;
			break;
		}
		case CALL_ALERTING:
		{
			callMsg = new _CallAlerting;
			break;
		}
		case CALL_CONNECT:
		{
			callMsg = new _CallConnect;
			break;
		}
		case CALL_CONNECT_ACK:
		{
			callMsg = new _CallConnectAck;
			break;
		}
		case CALL_DISCONNECT:
		{
			callMsg = new _CallDisConnect;
			break;
		}
		case CALL_RELEASE:
		{
			Sptr <_CallRelease> rel = new _CallRelease;
			rel->setCause(CALL_CAUSE_NO_ROUTE);
			callMsg = rel;
			break;
		}
		case CALL_RELEASE_COMPLETE:
		{
			callMsg = new _CallReleaseComplete;
			break;
		}
		case CALL_SETUP:
		{
			cpLog(LOG_WARNING, "CallSetup can not be send by this method");
		}
		default:
		{
			return;
		}

	}

	callEvent->setCallMsg(callMsg);
	callEvent->setEndPoint(ep);

	/* must set CallId into CallEvent, so we can clear Callid of EndPoint freely */
	callEvent->setCallId( ep->getCallId() );

	sendSchedulerEvent(callEvent);
}

void EpOperator::sendDeviceCommand(Sptr <PbxEvent> event)
{
	PbxAgent::instance()->getGateway()->sendHwCommand( event);
}

Sptr <PbxState> EpOperator::lookupEPState(Sptr <PbxEvent> event,  int stateType)
{
	Sptr <EndPoint> ep = event->getEndPoint();
	assert(ep != 0);

	return ep->findState(stateType);
}

