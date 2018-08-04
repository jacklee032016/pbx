/*
* $Id: FifoTest03.cxx,v 1.2 2007/03/02 07:29:49 lizhijie Exp $
*/
#include "Fifo.h"
#include "VThread.hxx"
#include "VTime.hxx"
#include <stdio.h>
#include <assert.h>
#include <unistd.h>

using namespace Vocal;

struct Foo
{
    bool isTimed;
    struct timeval start;
    Foo()
    {}


    Foo(const Foo& f) : isTimed(f.isTimed)
    {
        start.tv_sec = f.start.tv_sec;
        start.tv_usec = f.start.tv_usec;
    }
};

Fifo < Foo > shared;

int delay = 999;
int udelay = 10000;

void* run1(void*)
{
    while (1)
    {
        if (udelay)
            usleep(udelay);
        Foo x;
        x.isTimed = true;
        gettimeofday(&(x.start), 0);
        shared.addDelayMs(x, delay);
    }
    return 0;
}


void* run2(void*)
{
    int count = 0;
    int bad = 0;

    while (1)
    {
        Foo x = shared.getNext();
        if (x.isTimed)
        {
            // check to see if this is over 1 second
            struct timeval end;
            gettimeofday(&end, 0);

            long us =
                ( end.tv_sec - x.start.tv_sec) * 1000000 +
                ( end.tv_usec - x.start.tv_usec);

            if (us < (delay * 1000) )
            {
                bad++;
            }
        }
        count++;
        if ((count % 100) == 0)
        {
            printf("%d   %d\n", count, bad);
        }
    }
    return 0;
}



int main(int argc, char* argv[])
{

    if (argc > 1)
    {
        delay = atoi(argv[1]);
    }

    if (argc > 2)
    {
        udelay = atoi(argv[2]);
    }

    VThread thread1;
    VThread thread2;

    thread1.spawn(&run1, 0);
    thread2.spawn(&run2, 0);

    thread1.join();
    thread2.join();
    return 0;
}
