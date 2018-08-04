/*
 * $Id: Lockable.cxx,v 1.2 2007/03/01 20:04:33 lizhijie Exp $
 */

#include "global.h"
#include "Lockable.hxx"    


using Vocal::Threads::Lockable;


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
	

