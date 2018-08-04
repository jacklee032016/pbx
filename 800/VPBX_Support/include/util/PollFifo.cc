/*
* $Id: PollFifo.cc,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

using Assist::Logging::VLog;


template <class Msg>
PollFifo<Msg>::PollFifo()
{
}


template <class Msg>
PollFifo<Msg>::~PollFifo()
{
}


template <class Msg>
void	    	    
PollFifo<Msg>::registerProtocol(Protocol & protocol)
{
    myPoll.registerProtocol(protocol);
}


template <class Msg>
void	    	    
PollFifo<Msg>::updateProtocol(Protocol & protocol)
{
    myPoll.updateProtocol(protocol);
}


template <class Msg>
void	    	    
PollFifo<Msg>::unregisterProtocol(Protocol & protocol)
{
    myPoll.unregisterProtocol(protocol);
}


template <class Msg>
void 	    	    
PollFifo<Msg>::processProtocols(int numberFdsActive)
throw ( Assist::Transport::ProtocolException )
{
    myPoll.processProtocols(numberFdsActive);
}


template <class Msg>
Poll &      
PollFifo<Msg>::poll()
{
    return ( myPoll );
}
        

template <class Msg>
const Poll &    
PollFifo<Msg>::poll() const
{
    return ( myPoll );
}


template <class Msg>
void	    
PollFifo<Msg>::wakeup() 
throw ( VException )
{
    myPoll.interrupt();    
}


template <class Msg>
int	    	    
PollFifo<Msg>::wait(milliseconds_t relativeTimeout)
throw ( VException )
{
    const string    fn("PollFifo::wait");
    VLog    	    log(fn);

    VDEBUG(log) << fn << ": poll, timeout = " << relativeTimeout
	    	<< VDEBUG_END(log);

    // The mutex is locked coming into this method. Unlock it while we
    // block so others can use the Fifo.
    //
    myMutex.unlock();
    
    // Block, waiting for io activity. This may throw a transport
    // exception. If so, let it pass through.
    //
    // If poll receives a UNIX signal, i.e. SIGTERM, it will return 0.
    //
    int numberFdsActive = myPoll.poll(relativeTimeout);

    // Lock the Fifo back up.
    //
    myMutex.lock();

    VDEBUG(log) << fn << ": poll returned. fds active = " << numberFdsActive
	    	<< VDEBUG_END(log);

    // Return the number of file descriptors active.
    //
    return ( numberFdsActive );
}
