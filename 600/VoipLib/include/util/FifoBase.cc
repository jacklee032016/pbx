/*
 * $Id: FifoBase.cc,v 1.2 2007/03/01 20:04:29 lizhijie Exp $
 */

template <class Msg>
FifoBase<Msg>::FifoBase()
   : myFifoSize(0),
     myTimerSize(0),
     myShutdown(false)
{
}



template <class Msg>
FifoBase<Msg>::~FifoBase()
{
}



template <class Msg>
void
FifoBase<Msg>::add(const Msg & msg)
{
    Lock lock(myMutex); (void)lock;
    if ( myShutdown )
    {
        return;
    }

    myFifo.push_back(new Msg(msg));
    myFifoSize++;
    
    try
    {
        wakeup();
    }
    catch ( VException & )
    {
        assert(0);
    }
}


template <class Msg>
typename FifoBase<Msg>::EventId
FifoBase<Msg>::addDelay(
    const Msg	    &   msg,
    const TimeVal   &   relativeTimeout
)
{
    Lock lock(myMutex); (void)lock;

    if ( myShutdown )
    {
        return ( 0 );
    }

    EventId newId = myTimer.add(new Msg(msg), relativeTimeout);
    myTimerSize++;
    
    postAddTimed(newId);

    return ( newId );
}


template <class Msg>
typename FifoBase<Msg>::EventId
FifoBase<Msg>::addUntil(
    const Msg	    &   msg,
    const TimeVal   &   when
)
{
    Lock lock(myMutex); (void)lock;

    if ( myShutdown )
    {
        return ( 0 );
    }

    TimerEntryId newId = myTimer.add(new Msg(msg), when, false);
    myTimerSize++;
    
    postAddTimed(newId);

    return ( newId );
}


template <class Msg>
typename FifoBase<Msg>::EventId
FifoBase<Msg>::addDelayMs(
    const Msg	            &   msg,
    const milliseconds_t        pRelativeTimeout
)
{
    Lock lock(myMutex); (void)lock;

    if ( myShutdown )
    {
        return ( 0 );
    }

    milliseconds_t relativeTimeout = pRelativeTimeout;

    if ( relativeTimeout < 0 )
    {
        relativeTimeout = 0;
    }

    TimerEntryId newId = myTimer.add(new Msg(msg), relativeTimeout);
    myTimerSize++;
    
    postAddTimed(newId);

    return ( newId );
}


template <class Msg>
typename FifoBase<Msg> ::EventId
FifoBase<Msg> ::addUntilMs(
    const Msg	            &   msg,
    const milliseconds_t        when
)
{
    Lock lock(myMutex); (void)lock;

    if ( myShutdown )
    {
        return ( 0 );
    }

    EventId newId = myTimer.add(new Msg(msg), when, false);
    myTimerSize++;

    postAddTimed(newId);

    return ( newId );
}


template <class Msg>
void
FifoBase<Msg>::cancel(EventId id)
{
    if ( id == 0 )
    {
        return ;
    }

    Lock lock(myMutex); (void)lock;

    if ( !myTimer.cancel(id) )
    {
        // If the timer didn't hold the message to cancel, walk through
        // the queue and see if we have it there.
        //
        for (	typename MessageContainer::iterator it = myFifo.begin();
                it != myFifo.end();
                ++it
            )
        {
            // Assertion: The id for a timed event is just the
            // address of the memory for the message. If this isn't
            // true in TimerEntry, then we have problems.
            //
            if ( id == (EventId)((*it).operator->()) )
            {
                myFifo.erase(it);
                myFifoSize--;
                return ;
            }
        }
    }
    else
    {
       myTimerSize--;
    }
}


template <class Msg>
int
FifoBase<Msg>::block(milliseconds_t relativeTimeout)
throw ( VException )
{
    Lock lock(myMutex); (void)lock;

    if ( messageAvailableNoLock() )
    {
        return ( 1 );
    }

    return ( blockNoLock(relativeTimeout) );
}


template <class Msg>
Msg
FifoBase<Msg> ::getNext()
throw ( VException )
{
    Lock lock(myMutex); (void)lock;

    // Wait while there are no messages available.
    //
    while ( !messageAvailableNoLock() )
    {
        blockNoLock();
    }

    // Move expired timers into fifo.
    //
    while ( myTimer.messageAvailable() )
    {
        try
        {
            myFifo.push_back(myTimer.getMessage());
            myFifoSize++;
            myTimerSize--;
        }
        catch ( ... )
        {
            assert(0);
        }
    }

    // Return the first message on the fifo.
    //
    assert (myFifoSize > 0);
    if(myFifo.empty())
    {
       cerr << "Fifo: " << this << " list is empty, but fifo size is " << myFifoSize << endl;
    }
    assert (!myFifo.empty());
    Msg firstMessage = *(myFifo.front());

    myFifo.pop_front();
    myFifoSize--;
    
    return ( firstMessage );
}


template <class Msg>
unsigned int
FifoBase<Msg> ::size() const
{
    Lock lock(myMutex); (void)lock;

    return ( myFifoSize + myTimerSize );
}


template <class Msg>
unsigned int 
FifoBase<Msg>::sizeAvailable() const
{
    Lock lock(myMutex); (void)lock;
    
    return ( myFifoSize );
}


template <class Msg>
unsigned int 
FifoBase<Msg>::sizePending() const
{
    Lock lock(myMutex); (void)lock;
    
    return ( myTimerSize );
}


template <class Msg>
bool
FifoBase<Msg>::messageAvailable()
{
    Lock lock(myMutex); (void)lock;
    
    return ( messageAvailableNoLock() );
}


template <class Msg>
void
FifoBase<Msg>::shutdown()
{
    Lock lock(myMutex); (void)lock;
    
    myShutdown = true;
}


template<class Msg>
bool
FifoBase<Msg>::operator==(const FifoBase & src) const
{
    // Since each oberver is unique, we can compare addresses.
    //
    return ( this == &src );
}


template <class Msg>
bool
FifoBase<Msg>::operator!=(const FifoBase & src) const
{
    return ( this != &src );
}


template <class Msg>
bool
FifoBase<Msg>::operator<(const FifoBase & src) const
{
    return ( this < &src );
}

template <class Msg>
bool
FifoBase<Msg>::operator<=(const FifoBase & src) const
{
    return ( this <= &src );
}


template <class Msg>
bool
FifoBase<Msg>::operator>(const FifoBase & src) const
{
    return ( this > &src );
}

template <class Msg>
bool
FifoBase<Msg>::operator>=(const FifoBase & src) const
{
    return ( this >= &src );
}


template <class Msg>
bool
FifoBase<Msg>::messageAvailableNoLock()
{
   // can this just call myTimerSize? !jf!
   return ( myFifoSize > 0 || myTimer.messageAvailable() ); 
}

template <class Msg>
int
FifoBase<Msg>::blockNoLock(milliseconds_t relativeTimeout)
throw ( VException )
{
    // Use the shortest timeout value between the given relativeTimout
    // and the timer container's timeout, remembering that infinite timeout
    // is specified by a negative number.
    //
    milliseconds_t timerTimeout = myTimer.getTimeout(),
                                  timeout;

    // If timerTimeout is infinite, relativeTimeout can only be the
    // same or shorter.
    //
    if ( timerTimeout < 0 )
    {
        timeout = relativeTimeout;
    }

    // If relativeTimeout is infinite, timerTimeout can only be the
    // same or shorter.
    //
    else if ( relativeTimeout < 0 )
    {
        timeout = timerTimeout;
    }

    // Both are positive timeout values. Take the shortest in duration.
    else
    {
        timeout = relativeTimeout < timerTimeout
                  ? relativeTimeout
                  : timerTimeout;
    }

    // Wait for an event. A timer expiry or signal will return a 0 here.
    //
    int numberActive = wait(timeout);

    if ( numberActive > 0 )
    {
        return ( numberActive );
    }

    // See if a timer has expired.  If it has expired, return 1 to indicate
    // a message is available from the timer.
    //
    if ( messageAvailableNoLock() )
    {
        return ( 1 );
    }

    // To get here, either a signal woke us up, or the we used the
    // relativeTimeout value, meaning that a message isn't available from
    // the timer container.
    //
    return ( 0 );
}


template <class Msg>
void
FifoBase<Msg>::postAddTimed(const EventId & newId)
{ 
    TimerEntryId firstId = myTimer.getFirstTimerEntryId();

    // If we insert the new message at the front of the timer list,
    // we need to wakeup wait() since we have a timed message that
    // expires sooner than the current, or if we didn't have a timer
    // to begin with.
    //
    if ( firstId == newId )
    {
        try
        {
            wakeup();
        }
        catch ( VException & )
        {
            assert(0);
        }
    }
}
