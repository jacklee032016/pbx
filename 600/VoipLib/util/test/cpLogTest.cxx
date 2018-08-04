/*
* $Id: cpLogTest.cxx,v 1.2 2007/03/02 07:29:49 lizhijie Exp $
*/
#include "cpLog.h"
#include <unistd.h>
#include <cassert>

// simple test suite for cplog

void logFn()
{
    cpLogShow();
    int i;
    cpDebug(LOG_INFO, "LOG_INFO %d", i);
    cpLog(LOG_ERR, "LOG_ERR");
    cpDebug(LOG_DEBUG, "LOG_DEBUG");
}


void* thread1(void*)
{
    cpLogSetPriorityThread(VThread::selfId(), LOG_DEBUG);
    cpLogSetLabelThread(VThread::selfId(), "thread 1");
    logFn();
    sleep(1);
    logFn();
    return 0;
}


void* thread2(void*)
{
    cpLogSetPriorityThread(VThread::selfId(), LOG_ERR);
    cpLogSetLabelThread(VThread::selfId(), "thread 2");
    logFn();
    sleep(1);
    logFn();
    return 0;
}


void* thread3(void*)
{
    cpLogSetPriorityThread(VThread::selfId(), LOG_INFO);
    //    cpLogSetLabelThread(VThread::selfId(), "thread 3");
    logFn();
    sleep(1);
    logFn();
    return 0;
}


bool cpLogTest()
{
    // do something here
    if (cpLogStrToPriority("LOG_DEBUG") != LOG_DEBUG)
        return false;
    if (cpLogStrToPriority("DEBUG_STACK") != LOG_DEBUG_STACK)
        return false;
    if (cpLogStrToPriority("FOO") != -1)
        return false;
    if (cpLogStrToPriority("LOGFOO") != -1)
        return false;
    if (cpLogStrToPriority("LOGDEBUG") != -1)
        return false;
    if (cpLogStrToPriority("LOG_DEBUG_") != -1)
        return false;
    return true;
}


int main()
{
    cpLogSetLabel("cpLogTest");

    cpLogShow();
    VThread my_thread1;
    VThread my_thread2;
    VThread my_thread3;

    my_thread1.spawn(thread1, 0);
    my_thread2.spawn(thread2, 0);
    my_thread3.spawn(thread3, 0);

    my_thread1.join();
    my_thread2.join();
    my_thread3.join();


    if (!cpLogTest())
        assert(0);
    else
        cpLog(LOG_ERR, "test passed!");

    return 0;
}
