/*
* $Id: Context.cxx,v 1.1.1.1 2006/11/30 16:27:10 lizhijie Exp $
*/

#include "global.h"
#include "Context.hxx"
#include "StateInterface.hxx"


using Assist::Behavioral::Context;
using Assist::Behavioral::StateInterface;
using Assist::Behavioral::Stimulus;


Context::Context()
    :	myState(0),
    	myStartState(0),
	myStopState(0)
{
}


Context::~Context()
{
}


void	    	
Context::reset() throw()
{
    if ( myState )
    {
    	myState->exit();
    }
    
    myState = myStartState;
    myState->enter();
}


void	    	
Context::reset(StateInterface & state) throw()
{
    if ( myState )
    {
    	myState->exit();
    }
    
    myState = &state;
    myState->enter();
}

void	    	
Context::startState(StateInterface & state)
{
    myStartState = &state;
    
    if ( myState == 0 )
    {
    	myState = myStartState;
    	myStartState->enter();
    }
}


bool	    	
Context::inStartState() const
{
    return ( myState == myStartState );
}


void	    	
Context::stopState(StateInterface & state)
{
    myStopState = &state;
}


bool	    	
Context::inStopState() const
{
    return ( myState == myStopState );
}


void        	
Context::stimulate(const Stimulus & stimulus) throw ()
{
    if ( myState )
    {
    	myState = &(myState->stimulate(stimulus));
    }
}


ostream &       
Context::writeTo(ostream & out) const
{
    return ( out << name() );
}


bool
Context::verifyState(const StateInterface & state)
{
    return ( &state == myState );
}
