/*
 * $Id: Subject.cxx,v 1.2 2007/03/01 20:04:32 lizhijie Exp $
 */

#include "global.h"
#include "Subject.hxx"
#include "Observer.hxx"


using namespace Vocal;
using Behavioral::Subject;
using Behavioral::Observer;


Subject::Subject()
{
}


Subject::~Subject()
{
}


void	    	    
Subject::attach(Observer & src)
{
    // Push it on the back of the list. The user can enter the same
    // observer more than once if they wish.
    //
    myObservers.push_back(&src);
}


void	    	    
Subject::detach(Observer & src)
{
    // Remove the first observer. If not found, do nothing.
    //
    for (   vector<Observer *>::iterator it = myObservers.begin();
    	    it != myObservers.end();
	    it++
	)
    {
    	if ( *(*it) == src )
	{
	    myObservers.erase(it);
	    break;
	}
    }
}


void	    	    
Subject::notify()
{
    for (   vector<Observer *>::iterator it = myObservers.begin();
    	    it != myObservers.end();
	    it++
	)
    {
    	(*it)->update(*this);
    }
}


bool	    	    
Subject::operator==(const Subject & src) const
{
    return ( this == &src );
}


ostream &           
Subject::writeTo(ostream & out) const
{
    out << "observers (size: " << myObservers.size() << ") = { ";
    
    bool first = true;
    
    for (   vector<Observer *>::const_iterator it = myObservers.begin();
    	    it != myObservers.end();
	    it++
	)
    {
    	if ( first )
	{
	    first = false;
	}
	else
	{
	    out << ", ";
	}
	out << *(*it);
    }

    out << " }";
    
    return ( out );
}
