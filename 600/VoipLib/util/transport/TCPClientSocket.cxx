/*
 * $Id: TCPClientSocket.cxx,v 1.3 2007/03/03 18:38:56 lizhijie Exp $
 */


#include "global.h"
#include "TCPClientSocket.hxx"


using Vocal::Transport::TCPClientSocket;
using Vocal::Transport::StreamClientSocket;
using Vocal::Transport::AddressFamily;
using Vocal::IO::file_descriptor_t;
using Vocal::Transport::IPAddress;


TCPClientSocket::TCPClientSocket(
    const char      	    * 	name
)
    throw ( Vocal::SystemException )
    :	StreamClientSocket( 
    	    AddressFamily(AF_INET), 
    	    (name ? name : "TCPClient"))
{
}


TCPClientSocket::TCPClientSocket(
    const IPAddress 	    & 	localAddr,
    const char      	    *	name
)
    throw ( Vocal::SystemException )
    :	StreamClientSocket(localAddr, (name ? name : "TCPClient") )
{
}



TCPClientSocket::TCPClientSocket(
    const IPAddress  	&   localAddr,
    const IPAddress  	&   remoteAddr,
    const char	    	*   name
)
    throw ( Vocal::SystemException, Vocal::SystemStatus )
    :	StreamClientSocket(localAddr, remoteAddr, (name ? name : "TCPClient") )
{
}


TCPClientSocket::TCPClientSocket(
    file_descriptor_t	    fd,
    const IPAddress 	&   remoteAddr,
    const char      	*   name
)
throw ( Vocal::SystemException, Vocal::SystemStatus )
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


