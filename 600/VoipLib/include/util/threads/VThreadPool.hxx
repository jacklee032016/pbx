#ifndef VTHREADPOOL_HXX_
#define VTHREADPOOL_HXX_
/*
 * $Id: VThreadPool.hxx,v 1.2 2007/03/01 20:04:30 lizhijie Exp $
 */


#include <pthread.h>
#include <deque>
#include <list>

#include "VThread.hxx"
#include "VFunctor.hxx"
#include "VThreadInitException.hxx"
#include "Sptr.hxx"

/** this class creates an abstraction for doing work in multiple
 * threads.  VFunctors may be added the work queue.  The waiting
 * threads of the ThreadPool do the work specified by each VFunctor in
 * the work queue.  When the work is done, the thread returns to the
 * thread pool, doing more work if there are any jobs in the work
 * queue or, waiting until there is more work to be done if there are not.
 */

class VThreadPool
{
    public:
        typedef list < Sptr < VThread > > ListOfThreads;

        /// constructor
        VThreadPool(int numThreads = 1);

	/// destructor
        virtual ~VThreadPool();

        /// add a functor to the thread pool to be worked on
        void addFunctor(const VFunctor& functor) throw(VThreadInitException&);

        /** shutdown the threadpool.  if the finish flag is true, wait
	 * till the entire work queue is done
	 */
        void shutdown(bool finish = true);
	
	/// get a list of the worker threads
        const ListOfThreads& getWorkerThreads() const
        {
            return workerThreads_;
        };

        /// call from the thread construction function
        static void* workerProc(void* arg);

    private:
        /// initialize the code
        void init() throw (VThreadInitException&);

        deque < VFunctor > queue_;
        pthread_mutex_t queueLock_;
        pthread_cond_t queueNotEmpty_;
        pthread_cond_t queueNotFull_;
        pthread_cond_t queueEmpty_;
        int numThreads_;
        int queueClosed_;
        int shutdown_;
        int currQueueSize_;

        ListOfThreads workerThreads_;
};

#endif
