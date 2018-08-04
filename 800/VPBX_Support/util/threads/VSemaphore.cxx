/*
* $Id: VSemaphore.cxx,v 1.1.1.1 2006/11/30 16:27:10 lizhijie Exp $
*/

//#include "global.h"
#include "VSemaphore.hxx"

#if defined(__svr4__)|| defined (__SUNPRO_CC)

VSemaphore::VSemaphore(void)
{

    sema_init(&semaphore, 1, USYNC_THREAD, 0);

}

VSemaphore::VSemaphore(int available)
{

    sema_init(&semaphore, available, USYNC_THREAD, 0);
}

VSemaphore:: ~VSemaphore(void)
{
    sema_destroy(&semaphore);
}

void
VSemaphore::Post(void)
{
    sema_post(&semaphore);
}

void
VSemaphore::Post(int how_many)
{
    while (how_many-- > 0)
        sema_post(&semaphore);
}

void
VSemaphore::Wait(void)
{
    sema_wait(&semaphore);
}
#else

VSemaphore::VSemaphore(void)
{

    sem_init(&semaphore, 0, 1);

}

VSemaphore::VSemaphore(int available)
{
    sem_init(&semaphore, 0, available);
}

VSemaphore:: ~VSemaphore(void)
{
    sem_destroy(&semaphore);
}

void
VSemaphore::Post(void)
{
    sem_post(&semaphore);
}

void
VSemaphore::Post(int how_many)
{
    while (how_many-- > 0)
        sem_post(&semaphore);
}

void
VSemaphore::Wait(void)
{
    sem_wait(&semaphore);
}

#endif
