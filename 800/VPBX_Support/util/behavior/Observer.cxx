/*
* $Id: Observer.cxx,v 1.1.1.1 2006/11/30 16:27:10 lizhijie Exp $
*/

#include "Observer.hxx"


using Assist::Behavioral::Observer;
using Assist::Behavioral::Subject;


Observer::Observer()
{
}


Observer::~Observer()
{
}


void	           
Observer::update(Subject &)
{
}


bool	    	    
Observer::operator==(const Observer & src) const
{
    // Since each observer is unique, we can compare addresses.
    //
    return ( this == &src );
}


bool	    	    
Observer::operator<(const Observer & src) const
{
    return ( this < &src );
}
