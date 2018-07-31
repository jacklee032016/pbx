/*
 * $Id: EpState.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */
#include "global.h"
#include "EpState.hxx"
#include "CallId.hxx"

#include "PbxEvent.hxx"
#include "EndPoint.hxx"

using namespace Assist;

#if 0
/* endpoint header file   */ 
static char *state_name[] =
{ 
	"EPOINT_STATE_IDLE", 
	"EPOINT_STATE_IN_SETUP", 
	"EPOINT_STATE_OUT_SETUP", 
	"EPOINT_STATE_IN_OVERLAP", 
	"EPOINT_STATE_OUT_OVERLAP", 
	"EPOINT_STATE_IN_PROCEEDING", 
	"EPOINT_STATE_OUT_PROCEEDING", 
	"EPOINT_STATE_IN_ALERTING", 
	"EPOINT_STATE_OUT_ALERTING", 
	"EPOINT_STATE_CONNECT", 
	"EPOINT_STATE_IN_DISCONNECT", 
	"EPOINT_STATE_OUT_DISCONNECT", 
}; 

int state_name_num = sizeof(state_name) / sizeof(char *);

#endif

EpState::EpState(const string& name, int state)
	:PbxState(name, state)
{
//	initState();
}

EpState::~EpState()
{
//	PbxState::~PbxState();
	myOperators.clear();
}

Sptr <PbxStateObject>  EpState::getMyStateObject(const Sptr <PbxEvent> event)
{
	assert(event!=0);
	return event->getEndPoint();
}

void  EpState::setMyStateObject(const Sptr <PbxEvent> event, Sptr <PbxStateObject> obj ) 
{
	PbxState::setMyStateObject( event,  obj);

	Sptr <EndPoint> ep = NULL;
	ep.dynamicCast(obj);
	assert(ep!= 0);
	
	event->setEndPoint( ep);
	return;
}

