/*
* $Id: VThreadEvent.cxx,v 1.1.1.1 2006/11/30 16:27:10 lizhijie Exp $
*/

#include "global.h"
#include "VThreadEvent.hxx"

VThreadEvent::VThreadEvent(void)
{
    pthread_cond_init(&event, NULL);
    pthread_mutex_init(&dummy_mutex, NULL);
}
VThreadEvent::~VThreadEvent (void)
{
    pthread_cond_destroy(&event);
    pthread_mutex_destroy(&dummy_mutex);
}

void
VThreadEvent::Signal(void)
{
    pthread_cond_broadcast(&event);

}

void
VThreadEvent::Wait(void)
{
    pthread_mutex_lock(&dummy_mutex);
    pthread_cond_wait(&event, &dummy_mutex);
    pthread_mutex_unlock(&dummy_mutex);
}
