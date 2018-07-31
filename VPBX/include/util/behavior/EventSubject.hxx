#if !defined(ASSIST_EVENT_SUBJECT_HXX)
#define ASSIST_EVENT_SUBJECT_HXX
/*
* $Id: EventSubject.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "global.h"
#include "Subject.hxx"


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


/** Specialized subject to handle events.<br><br>
 *
 *  @see    Assist::Behavioral::Subject
 *  @see    Assist::Behavioral::EventObserver
 */
template <class Event>
class EventSubject : public Assist::Behavioral::Subject
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
} // namespace Assist


#endif // !defined(ASSIST_EVENT_SUBJECT_HXX)
