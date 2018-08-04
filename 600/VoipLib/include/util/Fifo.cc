/*
 * $Id: Fifo.cc,v 1.2 2007/03/01 20:04:29 lizhijie Exp $
 */

template <class Msg>
Fifo<Msg>::Fifo() 
{
}



template <class Msg>
Fifo<Msg>::~Fifo()
{
}



template <class Msg>
void
Fifo<Msg>::wakeup()
throw ( VException )
{
    // Tell any waiting threads the FIFO is no longer empty
    //
    int error = myCondition.signal();
    assert( !error );
}


template <class Msg>
int
Fifo<Msg>::wait(milliseconds_t relativeTimeout)
throw ( VException )
{
    // This will unlock the object's mutex, wait for a condition's signal,
    // and relock.
    //
    int error = myCondition.wait(&myMutex, relativeTimeout);

    if ( error != 0 )
    {
        // If we have been awaken due to a unix signal, i.e. SIGTERM,
        // or a timeout return 0 to indicate no activity.
        //
        if ( error == EINTR || error == ETIMEDOUT )
        {
            return ( 0 );
        }

        // Defensive, shouldn't ever get here.
        //
        else
        {
            assert( 0 );
        }
    }

    return ( myFifoSize );
}
