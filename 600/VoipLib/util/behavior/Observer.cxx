/*
 * $Id: Observer.cxx,v 1.2 2007/03/01 20:04:32 lizhijie Exp $
 */

#include "Observer.hxx"


using Vocal::Behavioral::Observer;
using Vocal::Behavioral::Subject;


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
