/*
* $Id: cpLogConcurrencyTest.cxx,v 1.2 2007/03/02 07:29:49 lizhijie Exp $
*/
#include "cpLog.h"
#include <unistd.h>

// simple test suite for cplog

void* thread1(void*)
{
    //    cpLogSetPriorityThread(VThread::selfId(), LOG_DEBUG);
    //    cpLogSetLabelThread(VThread::selfId(), "thread 1");

    for ( ; ; )
    {
        cpDebug(LOG_DEBUG, "thread 1 logging");
        //	cpLogSetLabel("adfhic");
        //	usleep(1000);
    }

    return 0;
}


void* thread2(void*)
{
    //    cpLogSetPriorityThread(VThread::selfId(), LOG_DEBUG);
    //    cpLogSetLabelThread(VThread::selfId(), "thread 2");

    for ( ; ; )
    {
        cpDebug(LOG_DEBUG, "thread 2 logging");
        //	cpLogSetLabel("adf");
    }

    return 0;
}


void* thread3(void*)
{
    for ( ; ; )
    {
        cpDebug(LOG_DEBUG, "thread 3 logging");
    }
    return 0;
}


void* thread4(void*)
{
    for ( ; ; )
    {
        cpDebug(LOG_DEBUG, "thread 4 logging");
    }
    return 0;
}


int main()
{
    cpLogSetPriority(LOG_DEBUG);
    //    cpLogSetLabel("");

    VThread my_thread1;
    VThread my_thread2;
    VThread my_thread3;
    VThread my_thread4;

    my_thread1.spawn(thread1, 0);
    my_thread2.spawn(thread2, 0);
       my_thread3.spawn(thread3, 0);
       my_thread4.spawn(thread4, 0);

    my_thread1.join();
    my_thread2.join();
       my_thread3.join();
       my_thread4.join();

    return 0;
}
