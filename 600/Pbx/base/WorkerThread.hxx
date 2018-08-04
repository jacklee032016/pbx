#ifndef WORKER_THREAD_HXX
#define WORKER_THREAD_HXX
/*
 * $Id: WorkerThread.hxx,v 1.2 2007/03/02 04:19:27 lizhijie Exp $
 */

#include "Fifo.h"
#include "Sptr.hxx"
#include "ThreadIf.hxx"
#include "Builder.hxx"

namespace Vocal
{

class SipProxyEvent;


/** Onject WorkerThread
<pre>
<br> Usage of this Class </br>

    it is a ThreadIf. It has a input queue on which it is blocked.
    The input queue contains sip proxy events.

</pre>
*/
class WorkerThread: public ThreadIf
{
    public:


        /** Create the worker thread given the incoming sip event fifo and
         *  the builder. The incoming messages from the fifo will be 
         *  distributed to the builder.
         */
        WorkerThread( const Sptr < Fifo < Sptr < SipProxyEvent > > > inputFifo,
		      const Sptr < Builder > builder);


        /** Virtual destructor
         */
        virtual ~WorkerThread();


        /** Indicates to the thread that it needs to cease processing.
         */
        virtual void shutdown();


    protected:
    
    
        /** The method that does the work of distributing the sip events
         *  to the builder.
         */
        virtual void thread();


    private:


        /** Incoming sip events will come through this fifo. They will then
         *  be distributed to the builder.
         */
        Sptr < Fifo < Sptr < SipProxyEvent > > > myFifo;


        /** Responsible for handling sip events.
         */
        Sptr < Builder > myBuilder;


        /** Suppress copying
         */
        WorkerThread(const WorkerThread &);
        
        
        /** Suppress copying
         */
        const WorkerThread & operator=(const WorkerThread &);
};
 
}

#endif // WORKER_THREAD_HXX
