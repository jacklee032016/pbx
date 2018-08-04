/*
 * $Id: Context.cxx,v 1.2 2007/03/01 20:04:32 lizhijie Exp $
 */

#include "global.h"
#include "Context.hxx"
#include "StateInterface.hxx"


using Vocal::Behavioral::Context;
using Vocal::Behavioral::StateInterface;
using Vocal::Behavioral::Stimulus;


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
