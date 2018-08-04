#ifndef SIP_THREAD_HXX
#define SIP_THREAD_HXX
/*
 * $Id: SipThread.hxx,v 1.3 2007/05/06 09:48:54 lizhijie Exp $
 */

#include "Fifo.h"
#include "Sptr.hxx"
#include "ThreadIf.hxx"

//#include "SipTransceiver.hxx"
//#include "SipProxyEvent.hxx"
//#include "SipMsg.hxx"

namespace Vocal
{

class SipTransceiver;
class SipProxyEvent;
class SipMsg;

/** Object SipThread
<pre>
<br> Usage of this Class </br>

    SipThread is derived from ThreadIf.
    The class acts as a message dispatcher between SIP stack
    and application. It dispatches messages received from the SIP stack to
    the application after translating into a suitable event.
    SipThread blocks on sipstack receive.  On receiving a SipMsg it creates a
    SipEvent. The SipEvent then gets posted to fifo which application gets
    and processes.
</pre>
 */
class SipThread: public ThreadIf
{
    public:

        /** Create the sip thread given the sip transceiver and the fifo.
         *  The incoming messages from the sip transceiver will be queued on 
         *  the given fifo.
         *  For Marshal and RS proxies, no CallLegHistory is required
         */
        SipThread( const Sptr < SipTransceiver > sipStack,
		   const Sptr < Fifo < Sptr < SipProxyEvent > > > outputFifo ,
                   bool callLegHistory=true);


        /** Virtual destructor.
         */
        virtual ~SipThread();


        /** Returns true if a loop is detected in the via list.
         */
        bool discardMessage(Sptr < SipMsg > & sipMsg) const;


    protected:


        /** Overload's ThreadIf::thread() and handles the bulk of the 
         *  processing. The incoming messages from the sip transceiver 
         *  will be queued on the given fifo.
         */
        virtual void thread();


        /** Source of incoming sip events.
         */
        Sptr <SipTransceiver> mySipStack;


        /** Sink for incoming sip events.
         */
        Sptr < Fifo < Sptr < SipProxyEvent > > > myOutputFifo;


    private:

    
        /** Suppress copying
         */
        SipThread(const SipThread &);
        

        /** Suppress copying
         */
        const SipThread * operator=(const SipThread &);

        bool myCallLegHistory;
};
 
}

#endif // SIP_THREAD_HXX
