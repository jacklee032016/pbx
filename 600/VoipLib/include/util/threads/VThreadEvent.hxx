#ifndef  VThreadEvent_hxx
#define VThreadEvent_hxx
/*
 * $Id: VThreadEvent.hxx,v 1.2 2007/03/01 20:04:30 lizhijie Exp $
 */

#include <pthread.h>

class VThreadEvent
{
    private:
        pthread_cond_t event;

        // in order to wait on a condition variable you must have a locked
        // mutex, which we don't really need.
        pthread_mutex_t dummy_mutex;

    public:
        VThreadEvent(void);

        ~VThreadEvent(void);

        void Signal(void);

        void Wait(void);
};
#endif
