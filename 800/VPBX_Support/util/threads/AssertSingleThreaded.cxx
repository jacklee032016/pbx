/*
* $Id: AssertSingleThreaded.cxx,v 1.1.1.1 2006/11/30 16:27:10 lizhijie Exp $
*/

#include "global.h"
#include "AssertSingleThreaded.hxx"


using Assist::Threads::SingleThreadAssertable;


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
