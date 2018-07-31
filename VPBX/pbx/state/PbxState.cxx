/*
 * $Id: PbxState.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */
#include "global.h"
#include "cpLog.h"
#include "PbxEvent.hxx"
#include "CallInfo.hxx"

#include "PbxState.hxx"

using namespace Assist;

#if 1
/* break from operations chain of current state and no state changed */
Sptr<PbxState> Assist::PBX_STATE_DONE_WITH_EVENT = new PbxState("DOWN_STATE", PBX_STATE_DONE_ID);

Sptr<PbxState> Assist::PBX_STATE_CONTINUE = 0;
#endif

PbxState::PbxState(const string& name, int state)
	:myName(name),
	myState(state)
{
//	initState();
}

PbxState::~PbxState()
{
	myOperators.clear();
}

Sptr < PbxState > PbxState::process(const Sptr <PbxEvent> event)
{
	if ( event == 0 )
	{
		assert ( event != 0 );
		return ( 0 );
	}
	Sptr <PbxStateObject> obj = getMyStateObject(event);
	assert(obj != 0);

	//  Get the next state from the event's call info. This will be the
	//  next state if a state isn't returned from one of the operators.
	Sptr <PbxState> currentState =  obj->getState(); //event->getStateObject()->getState();
	assert(currentState != 0);

	// To process this state, the currentState must either be PROXY_CONTINUE
	// or this. If it is PROXY_CONTINUE, that means that this is really
	// a stateless proxy and they didn't bother to set the currentState.
	if  (   currentState != PBX_STATE_CONTINUE  &&  currentState != this )
	{
		cpLog(LOG_DEBUG, "******************************CONTINUE");
		return ( PBX_STATE_CONTINUE );
	}

	Sptr <PbxState> nextState = PBX_STATE_CONTINUE;


	// Iterating through all operators of the given state.
	for (   OperatorIter iter = myOperators.begin(); iter != myOperators.end(); iter++)
	{
		
//		cpLog(LOG_DEBUG, "Processing operator (%s)", (*iter)->name());
		Sptr <PbxState> newState = (*iter)->process(event);

		/*following code should be modified in order to implement :
		* Ops is added into state by order;
		* when Ops change the state, then other Ops following this Ops is not executed 
		*/
		if( newState == PBX_STATE_DONE_WITH_EVENT )
		{
			cpLog(LOG_DEBUG, "***** Done with Event ***");
			break;
		}
		else if( newState != PBX_STATE_CONTINUE )
		{
			assert( nextState == PBX_STATE_CONTINUE );
			nextState = newState;
		}
	}	

	if( nextState != PBX_STATE_CONTINUE && nextState != PBX_STATE_DONE_WITH_EVENT )
	{
		processExit(event);

		obj->setState(nextState);

		nextState->processEntry(event);
	}

	cpLog(LOG_DEBUG, "Done with state (%s)" , name());
	return ( nextState );
}


void PbxState::addOperator(const Sptr <PbxOperator> op)
{
	helperAdd(myOperators, op, "Insert %s operator");
}

void PbxState::addEntryOperator(const Sptr <PbxOperator> op)
{
	helperAdd(myEntryOperators, op, "Insert %s entry operator");
}

void PbxState::addExitOperator(const Sptr <PbxOperator> op)
{
	helperAdd(myExitOperators, op, "Insert %s exit operator");
}

void PbxState::clearAll( )
{
	while ( myEntryOperators.empty() == false )
	{
		myEntryOperators.pop_back();
	}

	while ( myExitOperators.empty() == false )
	{
		myExitOperators.pop_back();
	}

	while ( myOperators.empty() == false )
	{
		myOperators.pop_back();
	}
}

void PbxState::processEntry(const Sptr <PbxEvent> event)
{
	for (OperatorIter iter = myEntryOperators.begin();iter != myEntryOperators.end(); iter++)
	{
		(*iter)->process(event);
	}
}

void PbxState::processExit(const Sptr <PbxEvent> event)
{
	for (OperatorIter iter = myExitOperators.begin(); iter != myExitOperators.end(); iter++)
	{
		(*iter)->process(event);
	}
}

void PbxState::helperAdd(OperatorContainer &container, const Sptr <PbxOperator> &op, const char* const cpLogStr )
{
	assert( op != 0 );
	cpLog(LOG_DEBUG, cpLogStr , op->name());
	container.push_back(op);

	assert( container.begin() != container.end() );
}

void PbxState::doCpLog() const
{
	cpLog( LOG_DEBUG, "PbxState: %s", this->name() );
}

Sptr <PbxStateObject>  PbxState::getMyStateObject(const Sptr <PbxEvent> event) 
{
	assert(0);
	return 0;
}

void  PbxState::setMyStateObject(const Sptr <PbxEvent> event, Sptr <PbxStateObject> obj) 
{
	assert(event!=0);
	assert(obj!=0);

	return ;
}
		
