/*
* $Id: StreamServerSocket.cxx,v 1.1.1.1 2006/11/30 16:27:10 lizhijie Exp $
*/

#include "global.h"
#include "StreamServerSocket.hxx"
#include "TransportCommon.hxx"
#include "SystemException.hxx"
#include "VLog.hxx"
#include <cerrno>


using Assist::Transport::StreamServerSocket;
using Assist::Transport::Socket;
using Assist::Transport::AddressFamily;
using Assist::Transport::TransportAddress;
using Assist::Transport::SocketType;
using Assist::Logging::VLog;


StreamServerSocket::StreamServerSocket(
    const AddressFamily     	&   addressFamily,
    int     	    	    	    backlog,
    const char      	    	*   name
)
throw ( Assist::SystemException )
    : 	Socket( addressFamily, 
    	    	SocketType(SOCK_STREAM), 
		(name ? name : "StreamServer"))
{
    listen(backlog);
}


StreamServerSocket::StreamServerSocket(
    const TransportAddress  	&   localAddr,
    int     	    	    	    backlog,
    const char      	    	*   name
)
throw ( Assist::SystemException )
    : 	Socket( localAddr,
    	    	SocketType(SOCK_STREAM),
		(name ? name : "StreamServer"))
{
    listen(backlog);
}


StreamServerSocket::~StreamServerSocket()
{
}


void
StreamServerSocket::listen(int backlog)
throw ( Assist::SystemException )
{
    const string    fn("StreamServerSocket::listen");
    VLog    	    log(fn);

    if ( ::listen(fd_, backlog) < SUCCESS )
    {
    	throw Assist::SystemException(fn + ": " + strerror(errno), 
	    	    	    	__FILE__, __LINE__, errno);
    }
    
    VDEBUG(log) << fn << ": " << *this
    	    	<< ": now listening." << VDEBUG_END(log);
}
