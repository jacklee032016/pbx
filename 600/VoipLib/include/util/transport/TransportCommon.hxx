#if !defined(VOCAL_TRANSPORTCOMMON_HXX)
#define VOCAL_TRANSPORTCOMMON_HXX
/*
 * $Id: TransportCommon.hxx,v 1.2 2007/03/01 20:04:31 lizhijie Exp $
 */

#include "VocalCommon.hxx"


#if !defined(__linux__)
#define     	MSG_NOSIGNAL	0
#endif // !defined(__linux__)


#if defined(__FreeBSD__) || defined(__APPLE__)
typedef unsigned int in_addr_t;
#endif // defined(__FreeBSD__)


#if defined(VOCAL_USE_SOCKLEN_T)
typedef VOCAL_USE_SOCKLEN_T socklen_t;
#endif // defined(VOCAL_USE_SOCKLEN_T)


#if defined(WIN32)
typedef char sockbuf_t;
#elif defined(VOCAL_USE_SOCKET_BUFFER)
typedef VOCAL_USE_SOCKET_BUFFER sockbuf_t;
#else
typedef void sockbuf_t;
#endif // defined(VOCAL_USE_SOCKET_BUFFER)


#if defined(__APPLE__)
typedef int socklen_t;
#endif
#endif // !defined(VOCAL_TRANSPORTCOMMON_HXX)
