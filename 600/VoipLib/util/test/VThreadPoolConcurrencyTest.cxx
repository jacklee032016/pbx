/*
* $Id: VThreadPoolConcurrencyTest.cxx,v 1.2 2007/03/02 07:29:49 lizhijie Exp $
*/

#include "VThreadPool.cxx"
#include "Condition.hxx"
#include "cpLog.h"

void doWork(void* msg)
{
    assert(msg == 0);
    cpLog(LOG_ERR, "good!");
    vusleep(1);
}


int main()
{
    VThreadPool pool(10);

    for(;;)
    {
	VFunctor func(doWork, 0);
	pool.addFunctor(func);
    }
}
