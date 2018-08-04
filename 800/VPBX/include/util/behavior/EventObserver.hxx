#if !defined(ASSIST_EVENT_OBSERVER_HXX)
#define ASSIST_EVENT_OBSERVER_HXX
/*
* $Id: EventObserver.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "global.h"
#include "Observer.hxx"
#include "EventSubject.hxx"


/** Infrastructure common to ASSIST.
 */
namespace Assist 
{


/** Infrastructure in ASSIST related to "algorithms and the assignment of
 *  responsibilities between objects".<br><br>
 *
 *  See Design Patterns, ISBN 0-201-63361-2, Chapter 5.
 */
namespace Behavioral
{


template <class> class EventSubject;


/** Specialized observer to handle events.<br><br>
 *
 *  @see    Assist::Behavioral::Observer
 *  @see    Assist::Behavioral::EventSubject
 */
template<class Event>
class EventObserver : public Assist::Behavioral::Observer
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


#endif // !defined(ASSIST_EVENT_OBSERVER_HXX)
