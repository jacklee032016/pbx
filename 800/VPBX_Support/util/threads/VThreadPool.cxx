/*
* $Id: VThreadPool.cxx,v 1.1.1.1 2006/11/30 16:27:10 lizhijie Exp $
*/

#ifndef __vxworks


#include "global.h"
#include <cerrno>
#include <cstdio>

#include "VThreadPool.hxx"
#include "cpLog.h"

extern "C" 
{

static void* workerProc(void* args)
{
    return VThreadPool::workerProc(args);
}

}


VThreadPool::VThreadPool(int numThreads)
    :
    numThreads_(numThreads),
    queueClosed_(0),
    shutdown_(0),
    currQueueSize_(0)

{
    init();
}


void
VThreadPool::init() throw (VThreadInitException&)
{
    int ret;
    char buf[256];
    if ((ret = pthread_mutex_init(&queueLock_, 0)) != 0)
    {
        sprintf(buf, "pthread_mutext_init %s", strerror(errno));
        cpLog(LOG_ALERT, buf);
        throw VThreadInitException(buf, __FILE__, __LINE__, errno);
    }

    if ((ret = pthread_cond_init(&queueNotEmpty_, 0)) != 0)
    {
        sprintf(buf, "pthread_cond_init %s", strerror(errno));
        cpLog(LOG_ALERT, buf);
        throw VThreadInitException(buf, __FILE__, __LINE__, errno);
    }

    if ((ret = pthread_cond_init(&queueNotFull_, 0)) != 0)
    {
        sprintf(buf, "pthread_cond_init %s", strerror(errno));
        cpLog(LOG_ALERT, buf);
        throw VThreadInitException(buf, __FILE__, __LINE__, errno);
    }

    if ((ret = pthread_cond_init(&queueEmpty_, 0)) != 0)
    {
        sprintf(buf, "pthread_cond_init %s", strerror(errno));
        cpLog(LOG_ALERT, buf);
        throw VThreadInitException(buf, __FILE__, __LINE__, errno);
    }

    ///create threads
    for (int i = 0 ; i < numThreads_; i++)
    {
        VThread* thread = new VThread();
        workerThreads_.push_back(thread);
        ret = thread->spawn(::workerProc, this);
        if (ret != 0)
        {
            sprintf(buf, "pthread_create %s", strerror(errno));
            cpLog(LOG_ALERT, buf);
            throw VThreadInitException(buf, __FILE__, __LINE__, errno);
        }
    }
}


void
VThreadPool::addFunctor(const VFunctor& functor) throw (VThreadInitException&)
{
    pthread_mutex_lock(&queueLock_);
    if (shutdown_ || queueClosed_)
    {
        pthread_mutex_unlock(&queueLock_);
        cpLog(LOG_DEBUG, "Pool shutting down..");
        throw VThreadInitException("Pool shutting down..",
                                   __FILE__, __LINE__, 0);
    }

    if (currQueueSize_ == 0)
    {
        queue_.push_back(functor);
        pthread_cond_signal(&queueNotEmpty_);
    }
    else
    {
        queue_.push_back(functor);
        pthread_cond_signal(&queueNotEmpty_);
    }
    currQueueSize_++;
    pthread_mutex_unlock(&queueLock_);
}


void
VThreadPool::shutdown(bool finish)
{
    int ret;

    if ((ret = pthread_mutex_lock(&queueLock_)) != 0)
    {
        cpLog(LOG_DEBUG, "Failed to get the lock, still shutting down..");
	assert(0);
	return;
    }
    if (queueClosed_ || shutdown_)
    {
        pthread_mutex_unlock(&queueLock_);
        return ;
    }
    queueClosed_ = 1;
    if (finish)
    {
        // Wait till the entire queue is finished
        while (currQueueSize_ != 0)
        {
            pthread_cond_wait(&queueEmpty_, &queueLock_);
        }
    }
    shutdown_ = 1;

    pthread_mutex_unlock(&queueLock_);

    // Wakeup all worker threads so that they can recheck shutdown flag
    pthread_cond_broadcast(&queueNotEmpty_);
    pthread_cond_broadcast(&queueNotFull_);
    // wait for workers to exit
    for (ListOfThreads::iterator itr = workerThreads_.begin();
            itr != workerThreads_.end(); itr++)
    {
        Sptr < VThread > vth = (*itr);
        if ((ret = vth->join()) != 0)
        {
            cpLog(LOG_ALERT, "Failed to join thread (%d)", vth->getId());
        }
    }
}


void*
VThreadPool::workerProc(void* args)
{
    VThreadPool& self = *(static_cast < VThreadPool* > (args));
    cpLog(LOG_DEBUG, "Creating worker thread (%d)", pthread_self());
    for (;;)
    {
        pthread_mutex_lock(&(self.queueLock_));
        while ((self.currQueueSize_ == 0) && !(self.shutdown_))
        {
            pthread_cond_wait(&(self.queueNotEmpty_), &(self.queueLock_));
        }

        if (self.shutdown_)
        {
            pthread_mutex_unlock(&(self.queueLock_));
            cpLog(LOG_DEBUG, "Thread (%d) exiting.." , pthread_self());
            pthread_exit(0);
        }
	VFunctor work = (self.queue_.front());
        self.queue_.pop_front();
        self.currQueueSize_--;

        if (self.currQueueSize_ == 0)
        {
            pthread_cond_signal(&(self.queueEmpty_));
        }

        pthread_mutex_unlock(&(self.queueLock_));
        try
        {
//            if (work)
//            {
                cpLog(LOG_DEBUG, "Thread (%d) Doing work." , pthread_self());
                work.doWork();
//            }
        }
        catch (VException& e)
        {
            cpLog(LOG_ALERT, "Work threw an exception: %s", e.getDescription().c_str());
        }
    }
   return 0;
}


VThreadPool::~VThreadPool()
{
    cpLog(LOG_DEBUG, "VThreadPool::~VThreadPool");
    shutdown();
}

#endif
