/*
 * $Id: State.cxx,v 1.4 2007/03/30 09:13:18 lizhijie Exp $
 */

#include "global.h"
#include "State.hxx"
#include "CallInfo.hxx"

using namespace Vocal;

Sptr<State> Vocal::PROXY_DONE_WITH_EVENT = new State;

// Continue is just 0
Sptr<State> Vocal::PROXY_CONTINUE = 0;

State::State()
{
}

State::~State()
{
	myOperators.clear();
}

Sptr <State> State::process(const Sptr <SipProxyEvent> event)
{
	if ( event == 0 )
	{
		assert ( event != 0 );
		return ( 0 );
	}

    //  Get the next state from the event's call info. This will be the
    //  next state if a state isn't returned from one of the operators.
    //
	Sptr <State> currentState = event->getCallInfo()->getState();
    
    // To process this state, the currentState must either be PROXY_CONTINUE
    // or this. If it is PROXY_CONTINUE, that means that this is really
    // a stateless proxy and they didn't bother to set the currentState.
    //
	if ( currentState != PROXY_CONTINUE &&  currentState != this )
	{
		cpDebug(LOG_DEBUG, "******************************PROXY CONTINUE");
		return ( PROXY_CONTINUE );
	}

	Sptr <State> nextState = PROXY_CONTINUE;

    // Iterating through all operators of the given state.
    //
	cpDebug(LOG_DEBUG, "Doing with state (%s)" , name());
	for(OperatorIter iter = myOperators.begin();iter != myOperators.end();iter++ )
	{
		cpDebug(LOG_DEBUG, "Processing operator (%s)", (*iter)->name());
		Sptr <State> newState = (*iter)->process(event);

		if( newState == PROXY_DONE_WITH_EVENT )
		{
			cpDebug(LOG_DEBUG, "***** Done with Event ***");
			break;
		}
		else if( newState != PROXY_CONTINUE )
		{
			cpDebug(LOG_DEBUG, "new state %s" , newState->name());
			// We have to assert, because two operators returning new
			// states is a major bug.
			assert( nextState == PROXY_CONTINUE );
			if (nextState != PROXY_CONTINUE)
			{
//				cpLog(LOG_DEBUG, "new state %s, nextState : %s" , newState->name(), nextState->name() );
				cpLog(LOG_EMERG, "nextState != PROXY_CONTINUE");
				exit(0);
			}
			nextState = newState;
		}
	}

	if( nextState != PROXY_CONTINUE )
	{                
		// Leaving this state, so invoke the exit operators.
		processExit(event);

		// Update the nextState.
		event->getCallInfo()->setState(nextState);

		// Entering the new state, so invoke the entry operator.
		nextState->processEntry(event);
	}
	
	cpDebug(LOG_DEBUG, "Done with state (%s)" , name());
	return ( nextState );
}


void State::addOperator(const Sptr < Operator > op)
{
	helperAdd(myOperators, op, "Insert %s operator");
}

void State::addEntryOperator(const Sptr < Operator > op)
{
	helperAdd(myEntryOperators, op, "Insert %s entry operator");
}

void State::addExitOperator(const Sptr < Operator > op)
{
	helperAdd(myExitOperators, op, "Insert %s exit operator");
}

void State::clearAll( )
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


void State::processEntry(const Sptr < SipProxyEvent > event)
{
	for (OperatorIter iter = myEntryOperators.begin(); iter != myEntryOperators.end(); iter++ )
	{
		(*iter)->process(event);
	}
}

void State::processExit(const Sptr < SipProxyEvent > event)
{
	for ( OperatorIter iter = myExitOperators.begin(); iter != myExitOperators.end(); iter++ )
	{
		(*iter)->process(event);
	}
}


void State::helperAdd( OperatorContainer &container, const Sptr < Operator > &op, const char* const cpLogStr )
{
	assert( op != 0 );

	cpDebug(LOG_DEBUG, cpLogStr , op->name());

	container.push_back(op);

	assert( container.begin() != container.end() );
}

