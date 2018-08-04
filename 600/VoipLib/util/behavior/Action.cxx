/*
 * $Id: Action.cxx,v 1.2 2007/03/01 20:04:32 lizhijie Exp $
 */


#include "global.h"
#include "Action.hxx"


using namespace Vocal;
using Behavioral::Action;
using Behavioral::Stimulus;


Action::Action()
    :	myNextAction(0)
{
}


Action::~Action()
{
}


ReturnCode
Action::action() throw ()
{
    if ( onAction() == SUCCESS )
    {
	if ( myNextAction != 0 )
	{
    	    return( myNextAction->action() );
	}
    }
    return ( !SUCCESS );
}


ReturnCode
Action::action(const Stimulus & stim) throw ()
{
    ReturnCode	rc = onAction(&stim);

    if ( rc == SUCCESS )
    {
	if ( myNextAction != 0 )
	{
    	    return( myNextAction->action(stim) );
	}
    }
    return ( rc );
}


void	    	
Action::chain(Action & action)
{
    myNextAction = &action;
}


ostream &       
Action::writeTo(ostream & out) const
{
    for ( const Action * action = this; action != 0; action = action->myNextAction )
    {
    	if ( action != this )
	{
	    out << "->";
	}
	out << action->name();
    }
    
    return ( out );
}


const char *	
Action::name() const
{
    return ( "NoAction" );
}


ReturnCode
Action::onAction(const Stimulus *) throw()
{
    return ( SUCCESS );
}
