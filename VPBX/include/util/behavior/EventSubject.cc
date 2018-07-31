/*
* $Id: EventSubject.cc,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
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
