#ifndef  VThreadEvent_hxx
#define VThreadEvent_hxx
/*
* $Id: VThreadEvent.hxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
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
