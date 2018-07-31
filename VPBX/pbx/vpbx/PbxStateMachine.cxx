/*
 * $Id: PbxStateMachine.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */

#include "global.h"
#include "EndPoint.hxx"
#include "PbxStateMachine.hxx"
#include "PbxManager.hxx"
#include "PbxEvent.hxx"


using namespace Assist;

PbxStateMachine::PbxStateMachine(PbxManager *_parent)
{
	myParent = _parent;
	cpLog( LOG_DEBUG, "PBX State Machine" );
}

PbxStateMachine::~PbxStateMachine()
{
	myStates.clear();
}

void PbxStateMachine::process( const Sptr <PbxEvent> event )
{
	assert( event != 0 );

	Sptr <PbxStateObject> obj = getMyStateObject(event);
	assert( obj != 0 );
	
	cpLog( LOG_DEBUG, "%s Current PbxState is %s", obj->getName(), obj->getState()->name() );
	Sptr <PbxState> nextState = obj->getState()->process( event );
	
	if( nextState != PBX_STATE_CONTINUE )
	{
		obj->setState( nextState );
		cpLog( LOG_DEBUG, "'%s' Next EpState is '%s'",obj->getName(), nextState->name() );
	}
	else
		cpLog( LOG_DEBUG, "'%s' continue current '%s'" , obj->getName(), obj->getState()->name() );
}

Sptr < PbxState > PbxStateMachine::findState( const string stateName )
{
	StateIter iter = myStates.begin();
	while ( iter != myStates.end() )
	{
		if ( stateName == (*iter)->name() )
		{
			return *iter;
		}
		iter++;
	}
	return 0;
}

Sptr < PbxState > PbxStateMachine::findState( int stateType )
{
	Sptr <PbxState> state;
	StateIter iter = myStates.begin();
	while ( iter != myStates.end() )
	{
		state = *iter;
		if ( stateType == state->getStateType() )
		{
			return state;
		}
		iter++;
	}
	return 0;
}

void PbxStateMachine::addState(const Sptr <PbxState> state)
{
	assert( state != 0);

	cpLog( LOG_DEBUG, "Insert %s state", state->name() );
	myStates.push_back(state);
	assert( myStates.begin() != myStates.end() );
}

Sptr <PbxManager> PbxStateMachine::getParent()
{
	return myParent;
}

Sptr <PbxStateObject> PbxStateMachine::getMyStateObject(Sptr <PbxEvent> event)
{
	Sptr <PbxStateObject> obj = event->getStateObject();
//	if(obj == NULL)
//		return event->getEndPoint();

	return obj;
}

