/*
* $Id: UDPSocket.cxx,v 1.1.1.1 2006/11/30 16:27:10 lizhijie Exp $
*/

#include "global.h"
#include "UDPSocket.hxx"
#include "IPAddress.hxx"


using Assist::Transport::UDPSocket;
using Assist::Transport::DatagramSocket;
using Assist::Transport::IPAddress;
using Assist::Transport::AddressFamily;


UDPSocket::UDPSocket(const char * name)
    throw ( Assist::SystemException )
    :	DatagramSocket(AddressFamily(AF_INET), (name ? name : "UDP"))
{
}


UDPSocket::UDPSocket(IPAddress & localAddr, const char * name)
    throw ( Assist::SystemException )
    :	DatagramSocket(localAddr, (name ? name : "UDP"))
{
}


UDPSocket::UDPSocket(
    IPAddress 	    	&   localAddr, 
    IPAddress 	    	&   remoteAddr, 
    const char      	*   name
)
    throw ( Assist::SystemException )
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
