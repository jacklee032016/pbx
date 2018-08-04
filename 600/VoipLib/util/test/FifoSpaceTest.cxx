/*
* $Id: FifoSpaceTest.cxx,v 1.2 2007/03/02 07:29:49 lizhijie Exp $
*/
#include "Fifo.h"
#include "VThread.hxx"
#include <stdio.h>
#include <assert.h>
#include <unistd.h>

using namespace Vocal;

struct big_struct
{
    int number;
    char space[500];
};

typedef big_struct big;


Fifo < big > shared;

void* run1(void*)
{
    long x = 0;
    while (1)
    {
        //      usleep(10000);
        big item;
        item.number = x++;
        shared.addDelayMs(item, 1000 * (random() % 10));
        if ((x % 5000) == 0)
        {
            printf("add: %ld\n", x);
        }
    }
    return 0;
}


void* run2(void*)
{
    long old = 0;
    while (1)
    {
        big item;
        item = shared.getNext();
        //      assert(old == x);
        //      printf("%d\n", x);
        old++;
        if ((old % 5000) == 0)
        {
            printf("get: %ld\n", old);
        }
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
