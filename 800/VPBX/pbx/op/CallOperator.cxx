/*
 * $Id: CallOperator.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */

#include "global.h"

#include "CallOperator.hxx"
#include "PbxEvent.hxx"
#include "CallEvent.hxx"
#include "PbxDeviceEvent.hxx"
#include "CallId.hxx"
#include "CallMsgBuilder.hxx"

#include "PbxAgent.hxx"

using namespace Assist;

CallOperator::CallOperator(const string& name)
	:PbxOperator(name)
{
}

CallOperator::~CallOperator()
{}

Sptr <CallInfo> CallOperator::getCallInfo(Sptr <PbxEvent> event)
{
	Sptr <CallInfo> call =NULL;
	/* StateObject, eg. CallInfo is set in CallScheduler */
	Sptr <PbxStateObject> obj = event->getStateObject();
	if(obj==NULL)
	{
		cpLog(LOG_ERR, "No CallInfo object has be assigned in this CallEvent");
		return 0;
	}
	
	call.dynamicCast(obj);
	assert(call != 0);
	return call;
}

void CallOperator::forwardCallEvent(Sptr <EndPoint> fwdEp, Sptr <PbxEvent> callEvent)
{
	assert(fwdEp != 0);
	assert(callEvent != 0);
	callEvent->setEndPoint( fwdEp );
	sendGatewayEvent( callEvent);
}

void CallOperator::sendCallSignalToMgr(Sptr <EndPoint> destEp, call_method_t  method)
{
	Sptr <CallEvent> callEvent = new CallEvent();
	Sptr <CommonMsg> callMsg;

	switch(method)
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

	forwardCallEvent(destEp, callEvent);
}

