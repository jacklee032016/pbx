/*
 * $Id: TCPServerSocket.cxx,v 1.2 2007/03/01 20:04:33 lizhijie Exp $
 */


#include "global.h"
#include "TCPServerSocket.hxx"
#include "SystemException.hxx"
#include "SystemStatus.hxx"
#include "IPAddress.hxx"
#include "TCPClientSocket.hxx"
#include "VLog.hxx"
#include <cerrno>


using Vocal::Transport::TCPServerSocket;
using Vocal::Transport::StreamServerSocket;
using Vocal::Transport::TCPClientSocket;
using Vocal::Transport::IPAddress;
using Vocal::IO::file_descriptor_t;
using Vocal::Logging::VLog;


TCPServerSocket::TCPServerSocket(
    int     	    	    backlog,
    const char      	*   name,
    const char      	*   acceptedName
)
throw ( Vocal::SystemException )
    : 	StreamServerSocket(
    	    AddressFamily(AF_INET), 
	    backlog, 
	    (name ? name : "TCPServer")),
	acceptedName_(acceptedName ? acceptedName : "")
{
}


TCPServerSocket::TCPServerSocket(
    const IPAddress 	&   localAddr,
    int     	    	    backlog,
    const char      	*   name,
    const char      	*   acceptedName
)
throw ( Vocal::SystemException )
    :	StreamServerSocket(
    	    localAddr, 
	    backlog, 
	    (name ? name : "TCPServer")),
	acceptedName_(acceptedName ? acceptedName : "")
{
}


TCPServerSocket::~TCPServerSocket()
{
}


Sptr<TCPClientSocket>
TCPServerSocket::accept()
throw ( Vocal::SystemException, Vocal::SystemStatus )
{
    const string    fn("TCPServerSocket::accept");
    VLog    	    log(fn);

    file_descriptor_t	newFd = INVALID;

    IPAddress	    	remoteAddr;
    socklen_t	    	length = remoteAddr.getAddressLength();
        
    if	(   (newFd = ::accept(fd_, 
    	    	    	remoteAddr.getAddress(), 
			&length)) == INVALID
	)
    {
    	int error = errno;
	
    	// If the socket is set nonblocking, we can get an EAGAIN
	// here, without receiving the packet. If the socket is set 
	// blocking we can get an EINTR here, without receiving the packet.
	// We'll throw a SystemStatus.
	// 
	if ( error == EAGAIN || error == EINTR )
	{
            throw Vocal::SystemStatus(fn + " on accept(): " + strerror(error), 
	    	    	    	__FILE__, __LINE__, error);
    	}	

    	throw Vocal::SystemException(fn + ": " + strerror(error), 
	    	    	    	__FILE__, __LINE__, error);
    }

    Sptr<TCPClientSocket>	newClientSocket 
    	= new TCPClientSocket(newFd, remoteAddr, acceptedName_.c_str());
	
    return ( newClientSocket );
}
