/*
* $Id: Lockable.cxx,v 1.1.1.1 2006/11/30 16:27:10 lizhijie Exp $
*/

#include "global.h"
#include "Lockable.hxx"    


using Assist::Threads::Lockable;


Lockable::Lockable()
{
}


Lockable::~Lockable()
{
}


void	
Lockable::readlock()
{
    lock();
}
    
    
void	
Lockable::writelock()
{
    lock();
}
	

