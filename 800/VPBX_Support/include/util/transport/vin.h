/*
* $Id: vin.h,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

// shadow header for netinet/in.h

#if defined(__FreeBSD__)

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#else
#if defined(__linux__) || defined(__svr4__)

#include <netinet/in.h>

#endif

#ifdef WIN32
typedef long ssize_t;
typedef int socklen_t;
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#endif // WIN32

#ifdef __APPLE__
typedef int socklen_t;
#endif
#endif
