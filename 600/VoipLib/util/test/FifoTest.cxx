/*
* $Id: FifoTest.cxx,v 1.2 2007/03/02 07:29:49 lizhijie Exp $
*/
#include "Fifo.h"
#include "VThread.hxx"
#include <stdio.h>
#include <assert.h>
#include <unistd.h>

using namespace Vocal;

Fifo < int > shared;

void* run1(void*)
{
    int x = 0;
    while (1)
    {
        usleep(10000);
        shared.addDelayMs(x++, 1000);
    }
    return 0;
}


void* run2(void*)
{
    int old = 0;
    while (1)
    {
        int x = shared.getNext();
        assert(old == x);
        printf("%d\n", x);
        old++;
    }
    return 0;
}



int main()
{

    VThread thread1;
    VThread thread2;

    thread1.spawn(&run1, 0);
    thread2.spawn(&run2, 0);

    thread1.join();
    thread2.join();
}
