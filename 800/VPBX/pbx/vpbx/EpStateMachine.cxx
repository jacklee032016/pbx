/*
 * $Id: EpStateMachine.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */

#include "global.h"
#include "EpStateMachine.hxx"

#include "EndPoint.hxx"
#include "PbxEvent.hxx"

using namespace Assist;

/*
PbxStateMachine *EpStateMachine::stateMachineInstance = 0;

PbxStateMachine* EpStateMachine::instance()
{
	if( stateMachineInstance == 0 );
		stateMachineInstance = new EpStateMachine();
	return stateMachineInstance;
}

const char* EpStateMachine::getName() const
{
	return "PBX EndPoint State Machine";
} 

*/

EpStateMachine::EpStateMachine(PbxManager *_parent)
	:PbxStateMachine( _parent)
{
}

EpStateMachine::~EpStateMachine()
{
	cpLog(LOG_DEBUG, "Destorying.....");
	myStates.clear();
}


Sptr <PbxStateObject> EpStateMachine::getMyStateObject(Sptr <PbxEvent> event)
{
	return event->getEndPoint();
}

