/*
 * $Id: EventObserver.cc,v 1.2 2007/03/01 20:04:29 lizhijie Exp $
 */


template<class Event>
EventObserver<Event>::EventObserver(EventSubject<Event> & eventSubject)
    :	myEventSubject(eventSubject)
{
}


template<class Event>
EventObserver<Event>::~EventObserver()
{
}


template<class Event>
void	            
EventObserver<Event>::update(Subject & subject)
{
    if	( subject == myEventSubject )
    {
    	Event event = myEventSubject.getEvent();
	
    	if ( interestedIn(event) )
	{
    	    onEvent(event);
	}
    }
}
