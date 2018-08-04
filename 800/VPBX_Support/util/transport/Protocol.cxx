/*
* $Id: Protocol.cxx,v 1.1.1.1 2006/11/30 16:27:10 lizhijie Exp $
*/

#include "global.h"
#include "Protocol.hxx"
#include "FileDescriptor.hxx"
#include "Poll.hxx"


using Assist::Transport::Protocol;
using Assist::Transport::ProtocolException;
using Assist::Transport::ConnectionBrokenException;
using Assist::Transport::Poll;
using Assist::SystemException;


Protocol::Protocol(const char * pName)
    :	myIncoming(true),
    	myOutgoing(false),
    	myPriority(false),
	myName(pName ? pName : ""),
        myPoll(0)
{
}


Protocol::Protocol(Poll & poll, const char * pName)
    :   myIncoming(true),
        myOutgoing(false),
        myPriority(false),
        myName(pName ? pName : ""),
        myPoll(&poll)
{
}


Protocol::~Protocol()
{
    if ( myPoll )
    {
        myPoll->unregisterProtocol(*this);
    }
}


void	    	    	    
Protocol::onIncomingAvailable()
{
}


void	    	    	    
Protocol::onOutgoingAvailable()
{
}


void	    	    	    
Protocol::onPriority()
{
}


void	    	    	    
Protocol::onDisconnect(
    ConnectionBrokenException *
)
throw ( Assist::Transport::ProtocolException )
{
    throw Assist::Transport::ProtocolException(
    	    	    	    "Protocol::onDisconnect", 
		    	    ProtocolException::HANGUP_EXCEPTION,
			    *this);
}


void	    	    	    
Protocol::onError(
    SystemException *
)
throw ( Assist::Transport::ProtocolException )
{
    throw Assist::Transport::ProtocolException(
    	    	    	    "Protocol::onError", 
		    	    ProtocolException::ERROR_EXCEPTION,
			    *this);
}


void	    	    	    
Protocol::handleUnknownException()
{
    throw;
}

void	    	    	    
Protocol::awaitIncoming(bool incoming)
{
    myIncoming = incoming;

    if ( myPoll )
    {
        myPoll->updateProtocol(*this);
    }
}


bool	    	    	    
Protocol::awaitIncoming() const
{
    return ( myIncoming );
}


void	    	    	    
Protocol::awaitOutgoing(bool outgoing)
{
    myOutgoing = outgoing;

    if ( myPoll )
    {
        myPoll->updateProtocol(*this);
    }
}


bool	    	    	    
Protocol::awaitOutgoing() const
{
    return ( myOutgoing );
}


void	    	    	    
Protocol::awaitPriority(bool priority)
{
    myPriority = priority;

    if ( myPoll )
    {
        myPoll->updateProtocol(*this);
    }
}


bool	    	    	    
Protocol::awaitPriority() const
{
    return ( myPriority );
}


const string &
Protocol::name() const
{
    return ( myName );
}


ostream &           	    
Protocol::writeTo(ostream & out) const
{
    return  (	out << myName << ", " << getFileDescriptor()
    	    	    << ( myIncoming ? ", awaiting incoming" : "")
	    	    << ( myOutgoing ? ", awaiting outgoing" : "")
	    	    << ( myPriority ? ", awaiting priority" : "")
	    );
}


bool	    	    	    
Protocol::operator==(const Protocol & src)
{
    return ( this == &src );
}


bool	    	    	    
Protocol::operator<(const Protocol & src)
{
    return ( this < &src );
}
