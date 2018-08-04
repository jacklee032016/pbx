/*
* $Id: VThreadGate.cxx,v 1.1.1.1 2006/11/30 16:27:10 lizhijie Exp $
*/

#include "global.h"
#include "VThreadGate.hxx"


VThreadGate::VThreadGate()
{
    pthread_mutex_init(&structure_lock, NULL);
    pthread_cond_init(&wakeup, NULL);
    closed = 1;
}


VThreadGate::~VThreadGate()
{
    pthread_mutex_destroy(&structure_lock);
    pthread_cond_destroy(&wakeup);
}


void
VThreadGate::Open()
{
    pthread_mutex_lock(&structure_lock);
    closed = 0;
    pthread_cond_broadcast(&wakeup);
    pthread_mutex_unlock(&structure_lock);
}


void
VThreadGate::Close()
{
    pthread_mutex_lock(&structure_lock);
    closed = 1;
    pthread_mutex_unlock(&structure_lock);
}


// temporarily open the gate, allowing any waiting threads to
// proceed
void
VThreadGate::Release()
{
    pthread_cond_broadcast(&wakeup);
}


void
VThreadGate::Wait()
{
    pthread_mutex_lock(&structure_lock);
    if (closed)
    {
        pthread_cond_wait(&wakeup, &structure_lock);
    }
    pthread_mutex_unlock(&structure_lock);
}
