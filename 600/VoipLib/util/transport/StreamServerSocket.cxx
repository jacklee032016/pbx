/*
 * $Id: StreamServerSocket.cxx,v 1.2 2007/03/01 20:04:33 lizhijie Exp $
 */

#include "global.h"
#include "StreamServerSocket.hxx"
#include "TransportCommon.hxx"
#include "SystemException.hxx"
#include "VLog.hxx"
#include <cerrno>


using Vocal::Transport::StreamServerSocket;
using Vocal::Transport::Socket;
using Vocal::Transport::AddressFamily;
using Vocal::Transport::TransportAddress;
using Vocal::Transport::SocketType;
using Vocal::Logging::VLog;


StreamServerSocket::StreamServerSocket(
    const AddressFamily     	&   addressFamily,
    int     	    	    	    backlog,
    const char      	    	*   name
)
throw ( Vocal::SystemException )
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
throw ( Vocal::SystemException )
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
throw ( Vocal::SystemException )
{
    const string    fn("StreamServerSocket::listen");
    VLog    	    log(fn);

    if ( ::listen(fd_, backlog) < SUCCESS )
    {
    	throw Vocal::SystemException(fn + ": " + strerror(errno), 
	    	    	    	__FILE__, __LINE__, errno);
    }
    
    VDEBUG(log) << fn << ": " << *this
    	    	<< ": now listening." << VDEBUG_END(log);
}
