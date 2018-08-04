/*
 * $Id: MACAddress.cxx,v 1.2 2007/03/01 20:04:33 lizhijie Exp $
 */


#include "global.h"
#include "MACAddress.hxx"
#include "Socket.hxx"
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>
#include <unistd.h>
#include <string.h>


using Vocal::Transport::MACAddress;
using Vocal::Transport::Socket;


MACAddress::MACAddress()
    :	high_(0),
    	low_(0)
{
    getMACAddress(socket(AF_INET, SOCK_DGRAM, IPPROTO_IP));
}


MACAddress::MACAddress(const Socket & sock)
{
    getMACAddress(sock.getFD());
}


MACAddress::~MACAddress()
{
}


u_int32_t   	    	    
MACAddress::high()
{
    return ( high_ );
}   


u_int32_t   	    	    
MACAddress::low()
{
    return ( low_ );
}



void	    	    	    
MACAddress::getMACAddress(int sock)
{
    #if !defined(SIOCGIFHWADDR) && !defined(SIOCGENADDR)

    return;

    #else

    if ( sock < 0 ) 
    {
    	return;
    }

    ifconf 	ifc;

    char    	buf[1024];

    memset(buf, 0, sizeof(buf));
    ifc.ifc_len = sizeof(buf);
    ifc.ifc_buf = buf;

    if ( ioctl(sock, SIOCGIFCONF, (char *)&ifc) < 0) 
    {
	close(sock);
    	return;
    }
    
    ifreq 	ifr, *ifrp;

    int size = ifc.ifc_len;
    u_int8_t    *   a = 0;

    for (int i = 0; i < size; i += sizeof(ifreq) ) 
    {
    	ifrp = (ifreq *)((char *)ifc.ifc_buf + i);
	
    	strncpy(ifr.ifr_name, ifrp->ifr_name, IFNAMSIZ);

    	#if defined(SIOCGIFHWADDR)
	if ( ioctl(sock, SIOCGIFHWADDR, &ifr) < 0 )
	{
    	    continue;
	}
	a = (unsigned char *)(&ifr.ifr_hwaddr.sa_data);

    	#elif defined(SIOCGENADDR)
	if ( ioctl(sock, SIOCGENADDR, &ifr) < 0 )
	{
    	    continue;
	}
	a = (unsigned char *)ifr.ifr_enaddr;
    	#endif

    	if ( !a[0] && !a[1] && !a[2] && !a[3] && !a[4] && !a[5] )
	{   	
	    continue;
	}

    	high_ 	=   ( a[0] << 8 )   + a[1];
    	low_  	=   ( a[2] << 24 )  + ( a[3] << 16 )
	      	+   ( a[4] << 8 )   + a[5];
	
    }

    close(sock);

    #endif // !defined(SIOCGIFHWADDR) || !defined(SIOCGENADDR)
}
