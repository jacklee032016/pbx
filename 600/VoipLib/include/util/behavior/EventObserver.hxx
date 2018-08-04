#if !defined(VOCAL_EVENT_OBSERVER_HXX)
#define VOCAL_EVENT_OBSERVER_HXX
/*
 * $Id: EventObserver.hxx,v 1.2 2007/03/01 20:04:29 lizhijie Exp $
 */

#include "global.h"
#include "Observer.hxx"
#include "EventSubject.hxx"


/** Infrastructure common to VOCAL.
 */
namespace Vocal 
{


/** Infrastructure in VOCAL related to "algorithms and the assignment of
 *  responsibilities between objects".<br><br>
 *
 *  See Design Patterns, ISBN 0-201-63361-2, Chapter 5.
 */
namespace Behavioral
{


template <class> class EventSubject;


/** Specialized observer to handle events.<br><br>
 *
 *  @see    Vocal::Behavioral::Observer
 *  @see    Vocal::Behavioral::EventSubject
 */
template<class Event>
class EventObserver : public Vocal::Behavioral::Observer
{
    protected:

    	/** This object will only respond to events from the given
	 *  subject.
	 */
    	EventObserver(EventSubject<Event> &);

    public:

    	/** Virtual destructor.
    	 */
	virtual ~EventObserver();
	

    	/** When update() is called by the given subject, the callback
	 *  interestedIn() is called, and on success, onEvent() is then
	 *  called, allowing the extending class to process the event.
    	 */
    	void	    	update(Subject &);

    protected:
    
    	/** This callback must return true to indicate that the extending
	 *  wishes to continue processing this event.
    	 */
    	virtual bool	interestedIn(const Event &) = 0;


    	/** This callback allows the extending class to process the incoming 
	 *  event.
    	 */
    	virtual void	onEvent(Event) = 0;
	
    private:
    
    	EventSubject<Event>   	&   myEventSubject;
};


#include "EventObserver.cc"


} // namespace Behavioral
} // namespace Vovida


#endif // !defined(VOCAL_EVENT_OBSERVER_HXX)
