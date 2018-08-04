#if !defined(VOCAL_EVENT_SUBJECT_HXX)
#define VOCAL_EVENT_SUBJECT_HXX
/*
 * $Id: EventSubject.hxx,v 1.2 2007/03/01 20:04:29 lizhijie Exp $
 */

#include "global.h"
#include "Subject.hxx"


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


/** Specialized subject to handle events.<br><br>
 *
 *  @see    Vocal::Behavioral::Subject
 *  @see    Vocal::Behavioral::EventObserver
 */
template <class Event>
class EventSubject : public Vocal::Behavioral::Subject
{
    public:
    

	/** Default constructor
	 */
    	EventSubject();


	/** Virtual destructor
	 */
	virtual ~EventSubject();


	/** Causes all EventObservers to be notified of the given Event.
	 */
    	void	    	    setEvent(Event);


	/** Event accessor.
	 */
    	Event	    	    getEvent();

    private:
        
        Event 	    	    myEvent;
};


#include "EventSubject.cc"


} // namespace Behavioral
} // namespace Vocal


#endif // !defined(VOCAL_EVENT_SUBJECT_HXX)
