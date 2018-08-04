/*
 * $Id: AssertSingleThreaded.cxx,v 1.2 2007/03/01 20:04:33 lizhijie Exp $
 */

#include "global.h"
#include "AssertSingleThreaded.hxx"


using Vocal::Threads::SingleThreadAssertable;


SingleThreadAssertable::SingleThreadAssertable()
    :
    myWriteBool(false)
{
}


SingleThreadAssertable::~SingleThreadAssertable()
{
    assert(myWriteBool == false);
    assert(myReadCount == 0);
}


void
SingleThreadAssertable::readlock()
{
    assert(myWriteBool == false);
    myReadCount.increment();
}


void
SingleThreadAssertable::writelock()
{
    assert(myWriteBool == false);
    assert(myReadCount == 0);
    myWriteBool = true;
}


void
SingleThreadAssertable::lock()
{
    writelock();
}


void
SingleThreadAssertable::unlock()
{
    assert((myWriteBool == false) || (myReadCount == 0));
    if(myWriteBool)
    {
	myWriteBool = false;
    }
    else
    {
	myReadCount.decrement();
    }
}
