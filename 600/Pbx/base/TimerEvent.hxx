#ifndef TIMER_EVENT_HXX
#define TIMER_EVENT_HXX
/*
 * $Id: TimerEvent.hxx,v 1.2 2007/03/02 04:19:27 lizhijie Exp $
 */

#include "SipProxyEvent.hxx"
#include "SipTransceiver.hxx"

namespace Vocal
{


/**  Object TimerEvent
<pre>
<br> Usage of this Class </br>

    TimerEvent is a SipProxyEvent. This event is posted when a timer goes off.

</pre>
*/
class TimerEvent : public SipProxyEvent
{
    public:


        /** Create a timer event given the associate fifo.
         */
        TimerEvent(Sptr < Fifo < Sptr < SipProxyEvent > > > outputFifo);


        /** Virtual destructor
         */
        virtual ~TimerEvent();


        /** Post the given event to the associated fifo to expire in
         *  timerMs milliseconds.
         */
        void startTimer(Sptr < SipProxyEvent > newEvent, const int timerMs);


        /** Cancel this previously started timer.
         */
        void cancelTimer();


        /** Remove the call associated with the call info from the
         *  call container.
         */
        void removeCall();


        /** Return the timer id of this previously started timer.
         */
        FifoEventId getTimerId();


        /** Name of this class.
         */
        virtual const char* const name() const
        {
            return ( "TimerEvent" );
        }


        /** Associate a sip stack with this event.
         */
        void setSipStack(const Sptr < SipTransceiver > sipStack);


        /** Accessor to the associated sip stack. 
         *  May be 0 if a sip stack hasn't been associate with this event.
         */
        Sptr < SipTransceiver > getSipStack() const;


    private:


        /** Sip transceiver associate with this event.
         */
        Sptr < SipTransceiver > mySipStack;
        

        /** Suppress copying
         */
        TimerEvent(const TimerEvent &);
        

        /** Suppress copying
         */
        const TimerEvent & operator=(const TimerEvent &);
};
 
}


#endif // TIMER_EVENT_HXX
