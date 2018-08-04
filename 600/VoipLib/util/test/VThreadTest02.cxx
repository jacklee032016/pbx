/*
* $Id: VThreadTest02.cxx,v 1.2 2007/03/02 07:29:49 lizhijie Exp $
*/

#include "VThread.hxx"
#include "VMutex.h"
#include "VTest.hxx"

struct TestStruct
{
    vthread_t* idPtr;
    bool retVal;
    VMutex* mutex;
};

void* testfn(void* structPtr)
{
    TestStruct* s;
    vthread_t myId;

    s = static_cast < TestStruct* > (structPtr);

    s->mutex->lock();
    s->mutex->unlock();
    myId = VThread::selfId();

    if (myId == *(s->idPtr))
    {
        s->retVal = true;
    }
    else
    {
        s->retVal = false;
    }
    return 0;
}

bool test_00()
{
    VThread myThread;
    VMutex mutex;
    TestStruct s;

    s.idPtr = 0;
    s.retVal = false;
    s.mutex = &mutex;

    mutex.lock();

    vthread_t id;
    myThread.spawn(testfn, &s);
    id = *(myThread.getId());
    s.idPtr = &id;
    mutex.unlock();
    myThread.join();
    return s.retVal;
}

int main()
{
    VTest myTest(__FILE__);

    myTest.test(0, test_00(), "VThread test 0");

    return myTest.exitCode();
}
