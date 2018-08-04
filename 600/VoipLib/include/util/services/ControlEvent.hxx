#if !defined(VOCAL_CONTROL_EVENT_HXX)
#define VOCAL_CONTROL_EVENT_HXX

/*
 * $Id: ControlEvent.hxx,v 1.2 2007/03/01 20:04:30 lizhijie Exp $
 */


#include "Event.hxx"
#include "Sptr.hxx"
#include "FifoBase.h"
#include "Mutex.hxx"


/** Infrastructure common to VOCAL.
 */
namespace Vocal 
{


/** Infrastructure common to VOCAL to provide event based services.<br><br>
 */
namespace Services
{


using Vocal::Threads::Mutex;


/** ControlEvent is used to manage the starting, stopping and shutting down of
 *  a thread that waits on Fifo<Event>'s.<br><br>
 *
 *  It is best to only use these within an Sptr.
 *
 *  @see    Vocal::Services::Event
 *  @see    FifoBase
 */
class ControlEvent : public Vocal::Services::Event
{
    public:


        /** The type of control event.
         */
        enum Type
        {
            START = 0,
            STOP,
            SHUTDOWN,
            START_RESPONSE,
            STOP_RESPONSE,
            SHUTDOWN_RESPONSE,

            TOTAL_TYPES,
            
            MAX_REQUEST = SHUTDOWN,
            MIN_RESPONSE = START_RESPONSE,
            MAX_RESPONSE = SHUTDOWN_RESPONSE
    	};


        /** Create a control event with a type and an optional reply
         *  fifo. If the reply fifo is set, the control event can be
         *  used as a request (rather than a command), and the response
         *  can be communicated via the returncode.
         */
        ControlEvent(Type, FifoBase< Sptr<Event> > * event = 0);


        /**  Create a control event with a type and a reply
         *  fifo. The control event is used as a request (rather than a 
	 *  command), and the response can be communicated via the returncode.
         */
        ControlEvent(Type, FifoBase< Sptr<Event> > &);


    	/** Virtual destructor.
	 */
        virtual ~ControlEvent();


    	/** Name of the this class of event, being "ControlEvent".
	 */
        virtual const char*  name() const;


    	/** Return the type of ControlEvent.
	 */
        Type	getType() const;


    	/** Set the return code for a returned response.
	 */
        void	setReturnCode(int);


    	/** Get the return code for a returned response.
	 */
        int     getReturnCode() const;


        /** Is a request event.
         */
        bool    isRequest() const;
        

        /** Is a response event.
         */
        bool    isResponse() const;


        /** Convert the request to a response
         */
        void    response();
        
        
    	/** Set the reply fifo for a returned response.
	 */
        void	setReplyFifo(
            FifoBase< Sptr<Event> > &
        );


    	/** Get the return code for a returned response.
	 */
        FifoBase< Sptr<Event> > * getReplyFifo() const;


        /** To uniquely identify a request. Useful if this control event is
         *  a request. The fifo waiting for the response can the uniquely
         *  identify the response, in the case they sent out multiple control
         *  events simultaneously.
         */
        int     getControlEventId() const;


        /** Write the event to a stream.
         */
        virtual ostream & writeTo(ostream &) const;


    private:

        // nextId is a globally shared resource, so it needs to be protected.
        //
        static	Mutex	                myNextIdMutex;
        static	int                     myNextId;

        Type	                        myType;
        int                             myReturnCode;
        FifoBase < Sptr < Event > > *	myFifo;
        int                             myId;
};


} // namespace Services
} // namespace Vocal


#endif // !defined(VOCAL_CONTROL_EVENT_HXX)
