/*
 * $Id: TransportAddress.cxx,v 1.2 2007/03/01 20:04:33 lizhijie Exp $
 */


#include "global.h"
#include "TransportAddress.hxx"
#include "IPAddress.hxx"
#include "IP6Address.hxx"


using Vocal::Transport::TransportAddress;
using Vocal::Transport::Socket;
using Vocal::Transport::IPAddress;
#if defined(__linux__)
using Vocal::Transport::IP6Address;
#endif
using Vocal::Transport::AddressFamily;


TransportAddress::TransportAddress()
{
}


TransportAddress::~TransportAddress()
{
}


TransportAddress *
TransportAddress::create(int addressFamily)
{
    TransportAddress *	newAddress = 0;
    
    switch ( addressFamily )
    {
    	case AF_INET:
	{
	    newAddress = new IPAddress();
	    break;
	}

    	#if defined(__linux__)
	case AF_INET6:
	{
	    newAddress = new IP6Address();
	    break;
	}
    	#endif // defined(__linux__)

	default:
	{
	    break;
	}
    }
    return ( newAddress );
}


void	    	    	
TransportAddress::setAddressFamily(int addrFamily)
{
    sockaddr	*   addr = getAddress();
    
    if ( addr )
    {
    	addr->sa_family = addrFamily;
    }
}


AddressFamily	    	
TransportAddress::getAddressFamily() const
{
    sockaddr	*   addr = getAddress();
    
    AddressFamily   af(addr ? addr->sa_family : AF_UNSPEC);
    return ( af );
}
