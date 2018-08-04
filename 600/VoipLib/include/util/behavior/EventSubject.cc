/*
 * $Id: EventSubject.cc,v 1.2 2007/03/01 20:04:29 lizhijie Exp $
 */

template <class Event>
EventSubject<Event>::EventSubject()
{
}


template <class Event>
EventSubject<Event>::~EventSubject()
{
}


template <class Event>
void	    	    
EventSubject<Event>::setEvent(Event event)
{
    myEvent = event;
    notify();
}


template <class Event>
Event	    	    
EventSubject<Event>::getEvent()
{
    return ( myEvent );
}
