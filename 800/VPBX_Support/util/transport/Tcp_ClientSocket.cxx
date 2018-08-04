/*
* $Id: Tcp_ClientSocket.cxx,v 1.1.1.1 2006/11/30 16:27:10 lizhijie Exp $
*/

#ifndef __vxworks

#include "global.h"
#include <errno.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <sys/socket.h>
#ifndef WIN32
#include <strings.h>
#else
#include <string.h>
#endif

#include "VNetworkException.hxx"
#include "Tcp_ClientSocket.hxx"
#include "VEnvVar.hxx"
#include "NetworkAddress.h"
#include "NetworkConfig.hxx"
#include "cpLog.h"

TcpClientSocket::TcpClientSocket(const string& hostName,
                                 bool closeCon, bool blocking)
        : _conn(blocking),
        _hostName(hostName),
        _serverPort( -1),
        _closeCon(closeCon),
        _blocking(blocking)
{
    initalize();
}

TcpClientSocket::TcpClientSocket(const string& hostName, int servPort,
                                 bool closeCon, bool blocking)
        : _conn(blocking),
        _hostName(hostName),
        _serverPort(servPort),
        _closeCon(closeCon),
        _blocking(blocking)
{
    initalize();
}

TcpClientSocket::TcpClientSocket(const NetworkAddress& server,
                                 bool closeCon, bool blocking)
        : _conn(blocking),
        _closeCon(closeCon),
        _blocking(blocking)
{
    _hostName = server.getHostName();
    _serverPort = server.getPort();
    cpLog(LOG_DEBUG_STACK, "%s %d", _hostName.c_str(), _serverPort);
    if (_serverPort == -1 )
    {
        _serverPort = VEnvVar::VPS_PORT;
    }
    initalize();
}

TcpClientSocket::TcpClientSocket(const TcpClientSocket& other)
        : _conn(other._blocking)
{
    _conn = other._conn;
}

TcpClientSocket&
TcpClientSocket::operator=(TcpClientSocket& other)
{
    if (this != &other)
    {
        _conn = other._conn;
        _hostName = other._hostName;
        _serverPort = other._serverPort;
        _closeCon = other._closeCon;
        _blocking = other._blocking;
    }
    return *this;
}


void
TcpClientSocket::initalize()
{
}

TcpClientSocket::~TcpClientSocket()
{
    close();
}


void
TcpClientSocket::connect() throw (VNetworkException&)
{
    struct addrinfo hints, *res, *tSave;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = NetworkConfig::instance().getAddrFamily();
    hints.ai_socktype = SOCK_STREAM;
    int err=0;
    char pBuf[56];
    sprintf(pBuf, "%d", _serverPort);
    if((err = getaddrinfo(_hostName.c_str(), pBuf, &hints, &res)) != 0)
    {
        char buf[256];
        sprintf(buf, "Failed to getaddrinfo for server %s:%d, reason %s",
            _hostName.c_str(), 
            _serverPort,
            gai_strerror(errno));
        cpLog(LOG_ERR, buf);
        throw VNetworkException(buf, __FILE__, __LINE__, errno);
    }
    tSave = res;
    do
    {
        _conn._connId = ::socket(res->ai_family, res->ai_socktype, res->ai_protocol);
        if (_conn._connId < 0)
        {
            char buf[256];
            sprintf(buf, "Failed to create socket: reason %s",
                strerror(errno));
            cpLog(LOG_DEBUG, buf);
            continue;
        }
        if (::connect(_conn._connId,
                  res->ai_addr,
                  res->ai_addrlen) == 0)
        {
            ///Success
            char descBuf[256];
            cpLog(LOG_DEBUG, "Connected to %s", connectionDesc(res, descBuf, 256));
            delete []_conn._connAddr;
            _conn._connAddr = (struct sockaddr*) new char[res->ai_addrlen];
            memcpy((_conn._connAddr), (res->ai_addr), res->ai_addrlen);
            cpLog(LOG_DEBUG, "SIze:%d, size2:%d", sizeof(*_conn._connAddr), res->ai_addrlen);
            _conn._connAddrLen = res->ai_addrlen;
            break;
        }
        _conn.close();
    } while ((res = res->ai_next) != 0);

    if(res == 0)
    {
        char buf[256];
        sprintf(buf, "Failed to connect to server %s:%d, reason %s",
            _hostName.c_str(), 
            _serverPort,
            gai_strerror(errno));
        cpLog(LOG_ERR, buf);
        _conn.close();
        throw VNetworkException(buf, __FILE__, __LINE__, errno);
    }
    freeaddrinfo(tSave);
    _conn.setState();
}

void
TcpClientSocket::close()
{
    if (_closeCon && _conn.getConnId() > 2)
        _conn.close();
}

const char*
TcpClientSocket::connectionDesc(struct addrinfo* laddr, char* descBuf, int bufLen) const
{
    assert(laddr);
    char hName[256];
    char portBuf[56];
    hName[0] = '\0';
    portBuf[0] = '\0';
    switch(laddr->ai_family)
    {
        case AF_INET:
        {
            cpLog(LOG_DEBUG, "IPv4");
            struct sockaddr_in* sin = (struct sockaddr_in*)(laddr->ai_addr);
            if(inet_ntop(laddr->ai_family, &sin->sin_addr, hName, 256) == 0)
            {
                return 0;
            };
            if(ntohs(sin->sin_port) != 0)
            {
               sprintf(portBuf, "%d", ntohs(sin->sin_port)); 
            }
            snprintf(descBuf, bufLen, "%s:%s", hName, portBuf);
        }
        break;
        case AF_INET6:
        {
            cpLog(LOG_DEBUG, "IPv6");
            struct sockaddr_in6* sin = (struct sockaddr_in6*)(laddr->ai_addr);
            if(inet_ntop(laddr->ai_family, &sin->sin6_addr, hName, 256) == 0)
            {
                return 0;
            };
            if(ntohs(sin->sin6_port) != 0)
            {
               sprintf(portBuf, "%d", ntohs(sin->sin6_port)); 
            }
            snprintf(descBuf, bufLen, "%s:%s", hName, portBuf);
        }
        break;
        default:
            snprintf(descBuf, bufLen, "Unknown");
        break;
    }
    return descBuf;
}

#endif
