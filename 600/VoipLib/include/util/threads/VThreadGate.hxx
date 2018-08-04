#ifndef  VTHREADGATE_HXX_
#define  VTHREADGATE_HXX_
/*
 * $Id: VThreadGate.hxx,v 1.2 2007/03/01 20:04:30 lizhijie Exp $
 */

#include <pthread.h>

/** implements a class which allows a user to wait() or release() the
 * wait after done.  Another user can block all waiters, or allow
 * waiters to go through.
 */

class VThreadGate
{
    public:
	/// constructor
        VThreadGate();
	/// destructor
        virtual ~VThreadGate();
	/// open the thread gate
        void Open();
	/// close the thread gate
        void Close();
	/// release this thread gate
        void Release();
	/// wait on this thread gate
        void Wait();
    private:
	/// suppress copy constructor
	VThreadGate(const VThreadGate&);
	/// suppress assignment operator
	VThreadGate& operator=(const VThreadGate&);

        pthread_mutex_t structure_lock;
        pthread_cond_t wakeup;
        int closed;
};


#endif
