/*
 * $Id: CallState.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */
#include "global.h"
#include "CallState.hxx"
#include "CallId.hxx"

#include "PbxEvent.hxx"
#include "EndPoint.hxx"

using namespace Assist;

#if 0
// Create a unique instance for comparison.
Sptr<CallState> Assist::CALL_PROXY_DONE_WITH_EVENT = new CallState;
// Continue is just 0
Sptr<CallState> Assist::CALL_PROXY_CONTINUE = 0;
#endif

CallState::CallState(const string& name, int state)
	:PbxState(name, state )
{
//	initState();
}

CallState::~CallState()
{
	myOperators.clear();
}

Sptr <PbxStateObject>  CallState::getMyStateObject(const Sptr <PbxEvent> event)
{
	assert(event!=0);
	return event->getStateObject();
}

void  CallState::setMyStateObject(const Sptr <PbxEvent> event, Sptr <PbxStateObject> obj) 
{
	PbxState::setMyStateObject( event,  obj);
	
	event->setStateObject(obj);
	return;
}

