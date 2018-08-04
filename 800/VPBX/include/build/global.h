#ifndef GLOBAL_H_
#define GLOBAL_H_

/*
* $Id: global.h,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#ifdef USE_DMALLOC
#include <dmalloc.h>
#endif

#ifdef __sgi
#include <pthread.h>
#include <string>
#endif

#ifdef __cplusplus
namespace std 
{
};

namespace __gnu_cxx
{
};

using namespace std;
using namespace __gnu_cxx;

#endif

/* this is used to turn of unused warnings in GCC */

#ifdef __GNUC__
#define UNUSED_VARIABLE __attribute__((__unused__))
#else
#define UNUSED_VARIABLE 
#endif

// Add: Mac OS X Support
#ifdef __APPLE__
#define in_addr_t u_int

#endif

#if defined(WIN32)

#if _MSC_VER > 1000
#pragma once
#endif /* _MSC_VER > 1000 */

#pragma warning(disable : 4786)

#define WIN32_LEAN_AND_MEAN	

#include <winsock2.h>
#include <direct.h>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <process.h>

#define srandom         srand
#define random          rand

#define strcasecmp      stricmp
#define strncasecmp     strnicmp

#define EWOULDBLOCK             WSAEWOULDBLOCK
#define EINPROGRESS             WSAEINPROGRESS
#define EALREADY                WSAEALREADY
#define ENOTSOCK                WSAENOTSOCK
#define EDESTADDRREQ            WSAEDESTADDRREQ
#define EMSGSIZE                WSAEMSGSIZE
#define EPROTOTYPE              WSAEPROTOTYPE
#define ENOPROTOOPT             WSAENOPROTOOPT
#define EPROTONOSUPPORT         WSAEPROTONOSUPPORT
#define ESOCKTNOSUPPORT         WSAESOCKTNOSUPPORT
#define EOPNOTSUPP              WSAEOPNOTSUPP
#define EPFNOSUPPORT            WSAEPFNOSUPPORT
#define EAFNOSUPPORT            WSAEAFNOSUPPORT
#define EADDRINUSE              WSAEADDRINUSE
#define EADDRNOTAVAIL           WSAEADDRNOTAVAIL
#define ENETDOWN                WSAENETDOWN
#define ENETUNREACH             WSAENETUNREACH
#define ENETRESET               WSAENETRESET
#define ECONNABORTED            WSAECONNABORTED
#define ECONNRESET              WSAECONNRESET
#define ENOBUFS                 WSAENOBUFS
#define EISCONN                 WSAEISCONN
#define ENOTCONN                WSAENOTCONN
#define ESHUTDOWN               WSAESHUTDOWN
#define ETOOMANYREFS            WSAETOOMANYREFS
#define ETIMEDOUT               WSAETIMEDOUT
#define ECONNREFUSED            WSAECONNREFUSED
#define ELOOP                   WSAELOOP
#define EHOSTDOWN               WSAEHOSTDOWN
#define EHOSTUNREACH            WSAEHOSTUNREACH
#define EPROCLIM                WSAEPROCLIM
#define EUSERS                  WSAEUSERS
#define EDQUOT                  WSAEDQUOT
#define ESTALE                  WSAESTALE
#define EREMOTE                 WSAEREMOTE

#if !defined(MAXHOSTNAMELEN)
#define MAXHOSTNAMELEN  256
#endif /* !defined(MAXHOSTNAMELEN) */

typedef int	            sigset_t;
typedef int                 siginfo_t;
typedef int                 pid_t;
typedef unsigned long int   in_addr_t;
typedef long                off_t;

struct pollfd
{
    int fd;
    short int events;
    short int revents;
};

#define POLLIN		0x001
#define POLLPRI		0x002
#define POLLOUT		0x004
#define POLLERR		0x008
#define POLLHUP		0x010
#define POLLNVAL	0x020

#define getcwd          _getcwd

#endif /* defined(WIN32) */

#endif /* GLOBAL_H_ */
