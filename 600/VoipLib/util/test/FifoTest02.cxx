/*
* $Id: FifoTest02.cxx,v 1.2 2007/03/02 07:29:49 lizhijie Exp $
*/
#include "Fifo.h"
#include "VThread.hxx"
#include <stdio.h>
#include <assert.h>
#include <unistd.h>

using namespace Vocal;

int main()
{
    Fifo < int > x;

    for (int i = 0; i < 10; i++)
    {
        x.add(i);
    }
    assert(10 == x.size());
    for (int i = 0; i < 10; i++)
    {
        int j = x.getNext();
    }
    assert(0 == x.size());

    for (int i = 0; i < 10; i++)
    {
        x.addDelayMs(i, 1);
    }

    sleep(1);
    //    assert(10 == x.size());



    return 0;
}
