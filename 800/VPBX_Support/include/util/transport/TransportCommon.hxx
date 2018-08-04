#if !defined(ASSIST_TRANSPORTCOMMON_HXX)
#define ASSIST_TRANSPORTCOMMON_HXX
/*
* $Id: TransportCommon.hxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#include "AssistCommon.hxx"


#if !defined(__linux__)
#define     	MSG_NOSIGNAL	0
#endif // !defined(__linux__)


#if defined(__FreeBSD__) || defined(__APPLE__)
typedef unsigned int in_addr_t;
#endif // defined(__FreeBSD__)


#if defined(ASSIST_USE_SOCKLEN_T)
typedef ASSIST_USE_SOCKLEN_T socklen_t;
#endif // defined(ASSIST_USE_SOCKLEN_T)


#if defined(WIN32)
typedef char sockbuf_t;
#elif defined(ASSIST_USE_SOCKET_BUFFER)
typedef ASSIST_USE_SOCKET_BUFFER sockbuf_t;
#else
typedef void sockbuf_t;
#endif // defined(ASSIST_USE_SOCKET_BUFFER)


#if defined(__APPLE__)
typedef int socklen_t;
#endif
#endif // !defined(ASSIST_TRANSPORTCOMMON_HXX)
