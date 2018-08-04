/*
* $Id: Tcp_ServerSocket.cxx,v 1.1.1.1 2006/11/30 16:27:10 lizhijie Exp $
*/

#ifndef __vxworks


#include "global.h"
#include <errno.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#ifndef WIN32
#include <strings.h>
#else
#include <string.h>
#endif

#include "VNetworkException.hxx"
#include "TransportCommon.hxx"
#include "Tcp_ServerSocket.hxx"
#include "VEnvVar.hxx"
#include "cpLog.h"
#include "NetworkAddress.h"
#include "NetworkConfig.hxx"

#define LISTENQ   15
TcpServerSocket::TcpServerSocket(int servPort) throw (VNetworkException&)
{
    listenOn(servPort);
}

void
TcpServerSocket::listenOn(int servPort) throw (VNetworkException&)
{
    struct addrinfo hints, *res, *sRes;
    memset(&hints, 0, sizeof(hints));
    hints.ai_flags = AI_PASSIVE;
    hints.ai_family = NetworkConfig::instance().getAddrFamily();
    hints.ai_socktype = SOCK_STREAM;
    char serv[56], err;
    sprintf(serv, "%u", servPort);
    if((err= getaddrinfo(NULL, serv, &hints, &res)) != 0)
    {
        char buf[256];
        sprintf(buf, "getaddrinfo failed, reason:%s", gai_strerror(errno));
        cpLog(LOG_ERR, buf);
        throw VNetworkException(buf, __FILE__, __LINE__, errno);
    }

    sRes = res;
    do
    {
        _serverConn._connId = ::socket(res->ai_family, res->ai_socktype, res->ai_protocol);
         if (_serverConn._connId < 0)
         {
             continue;
         }
         int on = 1;
#ifndef WIN32
         if ( setsockopt ( _serverConn._connId, SOL_SOCKET, SO_REUSEADDR,
                      reinterpret_cast<sockbuf_t *>(&on), sizeof ( on )) )
         {
             // this is an error -- can't set it
             char buf[256];
             sprintf(buf, "setsockopt failed, reason: %s", strerror(errno));
             cpLog(LOG_ALERT, "%s", buf);
         }
#endif
        if (::bind(_serverConn._connId, res->ai_addr, res->ai_addrlen) == 0)
        {
            //Success
            delete []_serverConn._connAddr;
            _serverConn._connAddr = (struct sockaddr*) new char[res->ai_addrlen];
            memcpy(_serverConn._connAddr, res->ai_addr, res->ai_addrlen);
            cpLog(LOG_INFO, "(%s) TCP server bound to port %d", (res->ai_family == PF_INET) ? "IPv4" : "IPv6", servPort);
            break;
        }
        ::close(_serverConn._connId);
    } while((res = res->ai_next) != NULL);

    freeaddrinfo(sRes);

    if(res == NULL)
    {
        char buf[256];
        sprintf(buf, "Failed to initialize TCP server on port %d, reason: %s", servPort, strerror(errno));
        cpLog(LOG_ALERT, "%s", buf);
        throw VNetworkException(buf, __FILE__, __LINE__, errno);
    }

    if (::listen(_serverConn._connId, LISTENQ))
    {
        char buf[256];
        sprintf(buf, "listen failed, reason:%s", strerror(errno));
        cpLog(LOG_ALERT, "%s", buf);
        throw VNetworkException(buf, __FILE__, __LINE__, errno);
    }
}

TcpServerSocket::TcpServerSocket(const TcpServerSocket& other)
{
    _serverConn = other._serverConn;
    _clientConn = other._clientConn;
}

TcpServerSocket&
TcpServerSocket::operator=(TcpServerSocket& other)
{

    if (this != &other)
    {
        _serverConn = other._serverConn;
        _clientConn = other._clientConn;
    }
    return *this;
}

TcpServerSocket::TcpServerSocket() throw (VNetworkException&)
{
    int port = VEnvVar::VPS_PORT; 
    listenOn(port);
}

TcpServerSocket::~TcpServerSocket()
{
    close();
}


int
TcpServerSocket::accept(Connection& con) throw (VNetworkException&)
{
    if ((con._connId = ::accept(_serverConn._connId, (SA*) con._connAddr, &con._connAddrLen)) < 0)
    {
        char buf[256];
        sprintf(buf, "Failed to accept the connection, reason:%s", strerror(errno));
        cpLog(LOG_DEBUG, buf);
        throw VNetworkException(buf, __FILE__, __LINE__, errno);
    }
    cpLog(LOG_DEBUG_STACK, "Connection from %s", con.getDescription().c_str());
    con._live = true;
    con.setState();
    return con._connId;
}

void
TcpServerSocket::close()
{
    #if !defined(WIN32)
    ::close(_serverConn.getConnId());
    #else
    ::closesocket(_serverConn.getConnId());
    #endif
}

#endif
