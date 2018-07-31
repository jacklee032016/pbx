#ifndef VSOCK_H
#define VSOCK_H
/*
* $Id: vsock.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#ifdef __vxworks
#include <resolvLib.h>
#include "sockLib.h"
#include "hostLib.h"
#include "netinet/ppp/random.h"
#include <taskLib.h>
#include <sys/times.h>
#include <u_Lib.h>
#include <in.h>
#include <sysLib.h>
#include <bootLib.h>
#include <ctype.h>

struct ip_mreqn
{
    struct in_addr imr_multiaddr;        /* IP multicast address of group */
    struct in_addr imr_address;          /* local IP address of interface */
    int imr_ifindex;                     /* Interface index */
};

typedef int socklen_t;

#define h_errno 0

extern struct hostent* gethostbyname( const char* name );
extern struct hostent* gethostbyaddr( const char* addr, int len, int type );
extern unsigned int getpid( void );
extern int strcasecmp(const char* s1, const char* s2);
extern int strncasecmp(const char* s1, const char* s2, size_t n);

#else

#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <sys/time.h>
#include <pwd.h>
#include <unistd.h>
#include <string.h>


#endif  // !__vxworks

extern char* getUser( void );

// VSOCK_H
#endif
