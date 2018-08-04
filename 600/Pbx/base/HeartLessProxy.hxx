#ifndef HEART_LESS_PROXY_HXX
#define HEART_LESS_PROXY_HXX
/*
 * $Id: HeartLessProxy.hxx,v 1.3 2007/06/18 11:11:17 lizhijie Exp $
 */

#include "Fifo.h"
#include "CallContainer.hxx"
#include "SipTransceiver.hxx"

namespace Vocal
{

class Builder;
class WorkerThread;
class SipThread;


/** Object  HeartLessProxy
<pre>
<br> Usage of this Class </br>

    This class creates a  call container, callprocessing fifo, sipstack,
    worker and sip threads. HeartLessProxy::run method starts the worker
    and sip threads. A builder object and SIP port number is given on
    instantiation.

    Create the proxy given the builder, the sip port (which defaults
    to 5060), the application name (which defaults to unknown).
    The boolean filteron indicates whether to createg a filtering sip
    transceiver or not (defaults to true). The boolean nat indicates
    whether the sip transceiver should be created to handle network
    address translation (defaults to false).

    HeartLessProxy(const Sptr < Builder >   builder,
                        unsigned short          defaultSipPort = 5060,
                        Data                    applName = "unknown",
                        bool                    filteron = true,
                        bool                    nat = false,
                        SipAppContext           aContext=APP_CONTEXT_GENERIC);

</pre>
*/
class HeartLessProxy
{
    public:


      /// Create one with default values Explained in Usage of Class section
        HeartLessProxy(const Sptr <Builder>   builder,
                        unsigned short          defaultSipPort = 5060,
                        Data                    applName = "unknown", 
                        bool                    filteron = true, 
                        bool                    nat = false,
                        SipAppContext           aContext=APP_CONTEXT_GENERIC);


        /** Virtual destructor
         */
        virtual ~HeartLessProxy();


        /** Runs the underlying sip thread and worker thread.
         */
        virtual void run();


        /** Shutdown the underlying sip thread and worker thread.
         */
        virtual void shutdown();


        /** Joins the underlying sip thread and worker thread.
         */
        virtual void join();


    protected:


        /** Call container associated with the builder.
         */
        Sptr < CallContainer >  myCallContainer;


        /** Builder supplied on construction.
         */
        Sptr < Builder >        myBuilder;


        /** Shared call processing queue between the sip thread and the
         *  worker thread. The sip thread writes to the queue and the
         *  sip thread reads from the queue.
         */
        Sptr < Fifo < Sptr < SipProxyEvent > > > myCallProcessingQueue;


        /** Sip transceiver that receives the incoming sip message, and
         *  that is used to send outgoing sip messages. The sip thread
         *  uses the sip transceiver to read incoming sip messages.
         */
        Sptr < SipTransceiver > mySipStack;


        /** The worker thread reads incoming sip events and processes them,
         *  via the supplied builder.
         */
        Sptr < WorkerThread > myWorkerThread;


        /** Ther sip thread reads the incoming sip messages from the 
         *  sip transceiver and posts them to the worker thread.
         */
        Sptr < SipThread > mySipThread;


    private:


        /** Suppress copying
         */
        HeartLessProxy(const HeartLessProxy &);
        

        /** Suppress copying
         */
        const HeartLessProxy & operator=(const HeartLessProxy &);
};
 
}

#endif // HEART_LESS_PROXY_HXX
