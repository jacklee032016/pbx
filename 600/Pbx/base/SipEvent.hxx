#ifndef SIP_EVENT_HXX
#define SIP_EVENT_HXX
/*
 * $Id: SipEvent.hxx,v 1.2 2007/03/02 04:19:27 lizhijie Exp $
 */

#include <vector>

#include "SipMsgQueue.hxx"
#include "SipCallLeg.hxx"
#include "SipProxyEvent.hxx"


namespace Vocal
{

class SipCommand;
class InviteMsg;
class SipMsg;


/** Object SipEvent
<pre>
<br> Usage of this Class </br>

    SipEvent is a SipProxyEvent. SipThread object creates a SipEvent
    on receiving a SipMsg. SipEvent gets posted to an output fifo.

</pre>
*/
class SipEvent : public SipProxyEvent
{
    public:

        /** Convenience typedef.
         */
        typedef vector < Sptr < SipMsg > > CallLegVector;


        /** Create the sip event given the associated fifo.
         */
        SipEvent( const Sptr < Fifo < Sptr < SipProxyEvent > > > outputFifo );


        /** Virtual destructor.
         */
        virtual ~SipEvent();


        /** The name of the class.
         */
        virtual const char* const name() const
        {
            return "SipEvent";
        }


        /** Set the sip message associated with this event. Updates the 
         *  call leg.
         */
        void setSipMsg(const Sptr < SipMsg > sipMsg);

        ///
        void setCallLeg();


        /** Returns the associated sip message.
         */
        const Sptr < SipMsg > getSipMsg() const;


        /** Set the sip message queue associated with this event.
         *  It also updates the sip message.
         */
        void setSipMsgQueue( const Sptr < SipMsgQueue > sipRcv );


        /** Get the sip message queue associated with this event.
         */
        const Sptr < SipMsgQueue > getSipMsgQueue() const;


        /** Calls setSipMsgQueue(). For backwards compatibility.
         */
        inline void setSipReceive( const Sptr < SipMsgQueue > sipRcv );
        

        /** Calls getSipMsgQueue(). For backwards compatibility.
         */
        const Sptr < SipMsgQueue > getSipReceive() const;


        /** Returns the INVITE associated with the event, or 0 if none exists
         */
        const Sptr < InviteMsg > getInvite() const;

        /** Returns the SIP request associated with the event, or 0 if none exists
         */
        const Sptr < SipCommand > getCommand() const;

        /** Takes a Command and returns the corresponding command which is to 
         *  be cancelled, or 0 if none exists
         */
        const Sptr < SipCommand > 
        getPendingCommand( Sptr < SipCommand > SipCommand ) const;

        ///
        const Sptr < SipCallLeg > getSipCallLeg() const;


        /**
         */
        void removeCall();

    private:


        /** Sip message associated with this event.
         */
        Sptr < SipMsg > mySipMsg;


        /** Sip message queue associate with this event. May be updated
         *  during the lifetime of an event.
         */
        Sptr < SipMsgQueue > mySipMsgQueue;


        /** Sip call leg associated with this event. May be updated during
         *  this lifetime of an event.
         */
        Sptr < SipCallLeg > mySipCallLeg;

        
        /** Suppress copying
         */
        SipEvent(const SipEvent &);
        

        /** Suppress copying
         */
        const SipEvent & operator=(const SipEvent &);
};
 
}

#endif // SIP_EVENT_HXX
