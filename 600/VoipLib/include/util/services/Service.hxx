#if !defined(VOCAL_SERVICE_HXX)
#define VOCAL_SERVICE_HXX
/*
 * $Id: Service.hxx,v 1.2 2007/03/01 20:04:30 lizhijie Exp $
 */


#include "Runnable.hxx"
#include "Writer.hxx"
#include "NonCopyable.hxx"
#include "FifoBase.h"
#include "EventObserver.hxx"
#include "EventSubject.hxx"
#include "Event.hxx"
#include "Sptr.hxx"
#include "ControlEvent.hxx"
#include "VocalCommon.hxx"
#include <string>

class VException;

namespace Vocal {
    namespace Signals {
        class SignalAction;
    }
}
    


/** Infrastructure common to VOCAL.
 */
namespace Vocal 
{


/** Infrastructure common to VOCAL to provide events based services.<br><br>
 */
namespace Services
{


using namespace Vocal::Behavioral;
using Vocal::Signals::SignalAction;


/** A Service is a Runnable that can be started, stopped, and shutdown.<br><br>
 *
 *  It has a Fifo where it blocks, and is an EventSubject, so that
 *  it can distribute Events as they arrive. Usually a Service is
 *  started in it's own Thread.
 *
 *  @see    Vocal::Runnable
 *  @see    Vocal::Behavioral::EventObserver
 *  @see    Vocal::Behavioral::EventSubject
 *  @see    Vocal::Services::Event
 *  @see    Vocal::ReturnCode
 *  @see    Vocal::Services::ControlEvent
 *  @see    Vocal::Threads::Thread
 *  @see    FifoBase
 *  @see    Sptr
 */
class Service : public Vocal::Runnable, public Vocal::IO::Writer, 
                public Vocal::NonCopyable
{

    protected:

    	/** Create a Service given an optional name.
    	 */    
    	Service(
	    const char              *   name = 0
	);


    public:
    

    	/** Virtual destructor
    	 */    
	virtual ~Service();


    	/** Accessor for Service Manager's Fifo.
	 */
	FifoBase< Sptr<Event> >     	&   getServiceManagerFifo();


    	/** Accessor for Service Manager's Fifo.
	 */
	const FifoBase< Sptr<Event> >	&   getServiceManagerFifo() const;


	/** Set the service manager fifo.
	 */
	void	setServiceManagerFifo(FifoBase< Sptr<Event> > *);


    	/** Accessors for Fifo.
	 */
	virtual	FifoBase< Sptr<Event> >     	&   getFifo() = 0;


    	/** Accessors for Fifo.
	 */
	virtual	const FifoBase< Sptr<Event> >	&   getFifo() const = 0;


    	/** Accessor for EventSubject.
	 */
	EventSubject< Sptr<Event> > 	    &	getEventSubject();


    	/** Accessor for EventSubject.
	 */
	const EventSubject< Sptr<Event> >   &	getEventSubject() const;


        /** Set the signal action. 
         */
        void                    setSignalAction(SignalAction * signalAction);
        
        
    	/** Runnable entry point.
	 */
    	virtual ReturnCode  	run();


    	/** Subscribe the message observer for incoming events.
	 */	
	void	    	    	subscribe(EventObserver< Sptr<Event> > &);


    	/** Unsubscribe the message observer for incoming events.
	 */	
	void	    	    	unsubscribe(EventObserver< Sptr<Event> > &);


	/** For logging.
	 */
	ostream &           	writeTo(ostream &) const;


        /** Returns the name of the service.
         */
        const string &          name() const;

    protected:
    

	/** User callback called when a valid start request has been issued. 
         *  A start request may be issued at the start of the service's
         *  lifetime, or after a successful stop request.
	 */
	virtual	ReturnCode      onStart();


	/** User callback called when a valid stop request has been issued. 
         *  The service manager may issue a start after a successful stop.
	 */
	virtual	ReturnCode      onStop();


	/** User callback called when a valid shutdown command has been 
         *  issued. When issued the service will be going away immediately,
         *  so you need to cleanup. The shutdown command does not require 
         *  a stop request to be made first.
	 */
	virtual	ReturnCode	onShutdown();
		

    	/** Handle an exception thrown by the fifo. The default behavior is 
    	 *  to ignore the exception. 
	 */
	virtual	void	    	onVException(VException &);


    	/** Handle an unknown exception thrown by the fifo. The default 
    	 *  behavior is to do a hardShutdown.
	 */
	virtual	void	    	onException();

        
        /** Handle the signal. Given that a signal action exists, a signal 
         *  has been caught. The default behavior is to do a hardShutdown.
         */
        virtual void            onSignalCaught();
        

    	/** Gets the message, detects if it is a control event. If
    	 *  so, it calls the appropriate callback. Otherwise the
    	 *  event is send to the subscribed observers.
	 */
    	virtual bool		processMessages(int, ReturnCode &);



    	/** A hard shutdown stops the service by calling onStop with 
    	 *  a zero stopEvent, and sends a STOP ControlEvent to the 
    	 *  service manager's fifo. It then shuts down the service by 
    	 *  calling onShutdown with a zero shutdownEvent and sends 
    	 *  the SHUTDOWN ControlEvent to the service manager's fifo.
	 */
    	void	    	    	hardShutdown(ReturnCode errorCode);


    private:


	/** Handle a start request. If the start request comes from
         *  the service manager, the start request is processed, setting 
	 *  the return code to SUCCESS. This calls the onStart callback.
	 */
	void	                start(Sptr<ControlEvent> startEvent);


	/** Handle a stop request. The default action is to return the 
	 *  stop request, setting the return code to SUCCESS. This calls
	 *  the onStop callback.
	 */
	virtual	void	    	stop(Sptr<ControlEvent> stopEvent);


	/** Handle a shutdown command. The shutdownEvent may be
    	 *  zero, indicating that the service is shutting down itself,
    	 *  usually on error. The default action is to do nothing.
	 */
	virtual	ReturnCode	shutdown(Sptr<ControlEvent> shutdownEvent);
		

	void	    	        blockSignals();
	void	    	        unblockSignals();
	bool  	                signalCaught();
	
    	int  	    	        block();


	FifoBase< Sptr<Event> >     *	myServiceManagerFifo;
    	EventSubject< Sptr<Event> > 	mySubject;
	string	    	    	    	myName;
        SignalAction                *   mySignalAction;
};


} // namespace Services
} // namespace Vocal


#endif // !defined(VOCAL_SERVICE_HXX)
