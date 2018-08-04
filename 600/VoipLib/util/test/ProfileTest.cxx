/*
* $Id: ProfileTest.cxx,v 1.2 2007/03/02 07:29:49 lizhijie Exp $
*/
#include <assert.h>
#include "VThread.hxx"
#include <unistd.h>
#include <stdio.h>

void profile0()
{
    for (int i = 0; i < 50; i++);
}

void profile1()
{
    for (int i = 0; i < 100; i++);
}

#define THREAD_PROFILE 1

void* runner(void* proportion)
{
    /*
    int props = *((int *) proportion) | 0x7f;
    int runme = *((int *) proportion) & 0x80;
*/
    int props = 2;
    int runme = 2;

#if THREAD_PROFILE
    u_short buf[1000000];
    size_t addr = ((size_t) & profile0) - 100000;
    if (runme)
    {
        memset(buf, 0, 1000000);

        profil(buf, 1000000, addr, 65536);
    }
#endif

    int count0 = 0;
    int count1 = 0;

    for (int i = 0; i < 1000000; i++)
    {
        if (i % props)
        {
            profile0();
            count0++;
        }
        else
        {
            profile1();
            count1++;
        }
    }

#if THREAD_PROFILE
    if (runme)
    {
        printf ("%d\n", (int)addr);
        profil(0, 1000000, addr, 65536);
        for (int j = 0; j < 1000000; j++)
        {
            if (buf[j] > 0)
            {
                printf ("%d: %d: %d\n",
                        (int)VThread::selfId(),
                       (int)( addr + j), buf[j]);
            }
        }
    }
#endif
    return 0;
}

int main()
{
#if 1
#if !THREAD_PROFILE
    size_t addr = ((size_t) & profile0) - 100000;
    u_short buf[1000000];

    memset(buf, 0, 1000000);

    profil(buf, 1000000, addr, 65536);
#endif

    VThread thread1;
    VThread thread2;
    VThread thread3;
    VThread thread4;
    VThread thread5;
    static int	    vProportion1=2 | 0x80;
    static int	    vProportion2=2;
    
    thread1.spawn(&runner, (void*) &vProportion1);
    /*
    thread2.spawn(&runner, (void*) &vProportion2);
    thread3.spawn(&runner, (void*) &vProportion2);
    thread4.spawn(&runner, (void*) &vProportion2);
    thread5.spawn(&runner, (void*) &vProportion2);
*/
    thread1.join();
    /*
    thread2.join();
    thread3.join();
    thread4.join();
    thread5.join();
*/
#if !THREAD_PROFILE
    profil(0, 1000000, addr, 65536);
    for (int j = 0; j < 1000000; j++)
    {
        if (buf[j] > 0)
        {
            printf ("%d: %d: %d\n",
                    VThread::selfId(),
                    addr + j, buf[j]);
        }
    }
#endif

#else
runner((void*) 2);
#endif

    return 0;
}
