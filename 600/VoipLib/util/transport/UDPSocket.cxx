/*
 * $Id: UDPSocket.cxx,v 1.2 2007/03/01 20:04:33 lizhijie Exp $
 */


#include "global.h"
#include "UDPSocket.hxx"
#include "IPAddress.hxx"


using Vocal::Transport::UDPSocket;
using Vocal::Transport::DatagramSocket;
using Vocal::Transport::IPAddress;
using Vocal::Transport::AddressFamily;


UDPSocket::UDPSocket(const char * name)
    throw ( Vocal::SystemException )
    :	DatagramSocket(AddressFamily(AF_INET), (name ? name : "UDP"))
{
}


UDPSocket::UDPSocket(IPAddress & localAddr, const char * name)
    throw ( Vocal::SystemException )
    :	DatagramSocket(localAddr, (name ? name : "UDP"))
{
}


UDPSocket::UDPSocket(
    IPAddress 	    	&   localAddr, 
    IPAddress 	    	&   remoteAddr, 
    const char      	*   name
)
    throw ( Vocal::SystemException )
    :	DatagramSocket(localAddr, remoteAddr, (name ? name : "UDP"))
{
}


UDPSocket::~UDPSocket()
{
}


Sptr<IPAddress>     
UDPSocket::getRemoteIPAddress() const
{
    Sptr<IPAddress> remoteIPAddr;

    return ( remoteIPAddr.dynamicCast(getRemoteAddress()) );
}    
