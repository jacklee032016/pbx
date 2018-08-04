/*
* $Id: TCPClientSocket.cxx,v 1.1.1.1 2006/11/30 16:27:10 lizhijie Exp $
*/

#include "global.h"
#include "TCPClientSocket.hxx"


using Assist::Transport::TCPClientSocket;
using Assist::Transport::StreamClientSocket;
using Assist::Transport::AddressFamily;
using Assist::IO::file_descriptor_t;
using Assist::Transport::IPAddress;


TCPClientSocket::TCPClientSocket(
    const char      	    * 	name
)
    throw ( Assist::SystemException )
    :	StreamClientSocket( 
    	    AddressFamily(AF_INET), 
    	    (name ? name : "TCPClient"))
{
}


TCPClientSocket::TCPClientSocket(
    const IPAddress 	    & 	localAddr,
    const char      	    *	name
)
    throw ( Assist::SystemException )
    :	StreamClientSocket(localAddr, (name ? name : "TCPClient") )
{
}



TCPClientSocket::TCPClientSocket(
    const IPAddress  	&   localAddr,
    const IPAddress  	&   remoteAddr,
    const char	    	*   name
)
    throw ( Assist::SystemException, Assist::SystemStatus )
    :	StreamClientSocket(localAddr, remoteAddr, (name ? name : "TCPClient") )
{
}


TCPClientSocket::TCPClientSocket(
    file_descriptor_t	    fd,
    const IPAddress 	&   remoteAddr,
    const char      	*   name
)
throw ( Assist::SystemException, Assist::SystemStatus )
    :	StreamClientSocket(fd, remoteAddr, (name ? name : "TCPClient"))
{
}


TCPClientSocket::~TCPClientSocket()
{
}


Sptr<IPAddress>     
TCPClientSocket::getRemoteIPAddress() const
{
    Sptr<IPAddress> remoteIPAddr;

    return ( remoteIPAddr.dynamicCast(getRemoteAddress()) );
}    
