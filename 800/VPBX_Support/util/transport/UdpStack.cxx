/*
* $Id: UdpStack.cxx,v 1.1.1.1 2006/11/30 16:27:10 lizhijie Exp $
*/

/* TODO List
 * - add sendTo function to allow you to specifiy different destinations
 * - add recvFrom function that tell you who the packet came from 
 * - add non blockinge version fo send and receive
 * - derive "ReliableUDP" stack that takes care of retransmissions
 * - look into using MSG_ERRQUEUE to check for ICMP errors
 * - check portability 
 * - support IP6 
 * - stress test 
 */

#include "global.h"
#include <fstream>
#include <assert.h>
#include <string.h> // for memset
#include <errno.h>
#include <iostream>
#include <stdio.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h> // for struct sockaddr_in
#include <stdlib.h>
#include <strstream>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "VTime.hxx"
#include <unistd.h>
#include <sys/uio.h>

#ifndef __vxworks
#include <fcntl.h>
#endif

#include <endian.h>


#include "UdpStack.hxx"
#include "cpLog.h"
#include "vsock.hxx"
#include "LockHelper.hxx"
#include "InitTransport.hxx"
#include "NetworkConfig.hxx"

// This is a file that contains multicast definitions for window support
// contact nismail@cisco.com
#ifdef WIN32
#include "W32McastCfg.hxx"
#endif

static VMutex G_lock;

static const char separator[7] = "\n****\n";

UdpStack::UdpStack ( const NetworkAddress* desHost
                     /* null if this is the server */ ,
                     int minPort,
                     int maxPort,
                     UdpMode udpMode,
                     bool log_flag,
                     bool isMulticast)
        :
        packetLossProbability( float(0.0) ),
        numBytesReceived (0),
        numPacketsReceived (0),
        numBytesTransmitted (0),
        numPacketsTransmitted (0),
        mode (sendrecv),
        logFlag (log_flag),
	blockingFlg(true)
{
	initTransport();
    data = new UdpStackPrivateData();
    assert(data);

    mode = udpMode;

    data->socketFd = socket(NetworkConfig::instance().getAddrFamily(), SOCK_DGRAM, IPPROTO_UDP);
    cpLog (LOG_DEBUG_STACK, "UdpStack socketFd = %d", data->socketFd);
    if ( data->socketFd < 0 )
    {
#if !defined(WIN32)
        int err = errno;
        strstream errMsg;
        errMsg << "UdpStack::::UdpStack error during socket creation:";
        errMsg << "Reason " << strerror(err) << ends;

        cpLog(LOG_ERR,  errMsg.str());
        throw UdpStackException(errMsg.str());
#else
		int err = WSAGetLastError();
		cpLog(LOG_ERR, "UdpStack: socket failed: %d", err);
		assert(0);
#endif
    }

    int buf1 = 1;
    int len1 = sizeof(buf1);


    int rcvbuf = 0;
    int rcvbufnew = 240 * 1024;
    int rcvbufnewlen = sizeof(rcvbufnew);
    int sndbuf = 0;
    unsigned int rcvbuflen = 1;
    unsigned int sndbuflen = 1;

#ifdef __linux__
    struct protoent * protoent;
    protoent = getprotobyname("icmp");

    if (!protoent)
    {
        fprintf(stderr, "Cannot get icmp protocol\n");
    }
    else
    {
        if (setsockopt(data->socketFd, protoent->p_proto, SO_BSDCOMPAT,
                       (char*)&buf1, len1)
                == -1)
        {
            fprintf(stderr, "setsockopt error SO_BSDCOMPAT :%s\n",
                    strerror(errno));
        }
#if 1
        if (setsockopt(data->socketFd, SOL_SOCKET, SO_RCVBUF,
                       (int *)&rcvbufnew, rcvbufnewlen)
                == -1)
        {
            fprintf(stderr, "setsockopt error SO_RCVBUF :%s\n",
                    strerror(errno));
        }
#endif
        if (getsockopt(data->socketFd, SOL_SOCKET, SO_RCVBUF, 
                       (int*)&rcvbuf, &rcvbuflen) == -1)
        {
            fprintf(stderr, "getsockopt error SO_RCVBUF :%s\n",
                    strerror(errno));
        }
        else
        {
            cpLog(LOG_DEBUG, "SO_RCVBUF = %d, rcvbuflen  =%d" , rcvbuf , rcvbuflen);
        }
        if (getsockopt(data->socketFd, SOL_SOCKET, SO_SNDBUF, 
                       (int*)&sndbuf, &sndbuflen) == -1)
        {
            fprintf(stderr, "getsockopt error SO_SNDBUF :%s\n",
                    strerror(errno));
        }
        else
        {
            cpLog(LOG_DEBUG, "SO_SNDBUF = %d, sndbuflen = %d" , sndbuf , sndbuflen);
        }
    }

#endif

    if (isMulticast)
    {
        // set option to get rid of the "Address already in use" error
        if (setsockopt(data->socketFd, SOL_SOCKET, SO_REUSEADDR,
                       (char*)&buf1, len1)
                == -1)
        {
            fprintf(stderr, "setsockopt error SO_REUSEADDR :%s",
                    strerror(errno));
        }

#if defined(__FreeBSD__) || defined(__APPLE__)

        if (setsockopt(data->socketFd, SOL_SOCKET, SO_REUSEPORT,
                       (char*)&buf1, len1)
                == -1)
        {
            fprintf(stderr, "setsockopt error SO_REUSEPORT :%s",
                    strerror(errno));
        }

#endif

    }

    switch (mode)
    {
        case inactive :
        {
            cpLog(LOG_INFO, "Udp stack is inactive");
            cpLog(LOG_ERR, "desHost is saved for future use.");
            doClient(desHost);
        }
        break;
        case sendonly :
        {
            if ( desHost )
            {
                // set the remote address
                doClient(desHost);
            }
            else
            {
                cpLog(LOG_DEBUG_STACK, "sendonly Udp stack, desHost is needed by using setDestination()");
            }
        }
        break;
        case recvonly :
        {
            if ( desHost )
            {
                cpLog(LOG_ERR,
                      "recvonly Udp stack, desHost is saved for future use.");
                doClient(desHost);
            }
            else
            {
                // only receive, do bind socket to local port
                doServer(minPort, maxPort);
            }
        }
        break;
        case sendrecv :
        {
            if ( desHost )
            {
                // receive and send,
                // bind the socket to local port and set the remote address
                doServer(minPort, maxPort);
                doClient(desHost);
            }
            else
            {
                // only receive, do bind socket to local port
                doServer(minPort, maxPort);
                cpLog(LOG_DEBUG_STACK, "sendrecv Udp stack, desHost is needed by using setDestination()");
            }
        }
        break;
        default :
        cpLog(LOG_ERR, "undefined mode for udp stack");
        break;
    }

    //    logFlag = true;

    strstream logFileNameRcv;
    strstream logFileNameSnd;

    if (logFlag)
    {
        in_log = new ofstream(logFileNameRcv.str(), ios::app);
        if (!in_log)
        {
            cerr << "Cannot open file "
            << logFileNameRcv.str() << endl;
            logFileNameRcv.freeze(false);
            logFlag = false;
        }
        else
        {
            in_log->write ("UdpRcv\n", 7);
            strstream lcPort;
            lcPort << "localPort: " << getTxPort() << "\n" << char(0);
            in_log->write(lcPort.str(), strlen(lcPort.str()));
            lcPort.freeze(false);
            logFileNameRcv.freeze(false);
            rcvCount = 0;
        }

        out_log = new ofstream(logFileNameSnd.str(), ios::app);
        if (!out_log)
        {
            cerr << "Cannot open file "
            << logFileNameSnd.str() << endl;
            logFileNameSnd.freeze(false);
            logFlag = false;
        }
        else
        {
            if (! logFlag)
                logFlag = true;
            out_log->write ("UdpSnd\n", 7);
            logFileNameSnd.freeze(false);
            sndCount = 0;
        }
    }

}

void
UdpStack::doServer ( int minPort,
                     int maxPort )  // these are lcoal ports
{
    /*
        cpLog (LOG_DEBUG_STACK, "UdpStack::doServer");
        cpLog (LOG_DEBUG_STACK, "minPort = %d, maxPort = %d", minPort, maxPort);
    */
    LockHelper helper(_lock);

    // this is a server
    if ( (minPort == -1) && (maxPort == -1) )
    {
        minPort = 1024;
        maxPort = 65534;
    }
    if ( maxPort == -1 )
    {
        maxPort = minPort;
    }

    // reset name now that the port range is defined
    strstream aName;
    aName << "-receiver-" << ":"
    << "[" << minPort
    << "-" << maxPort << "]"
    << char(0);
    setLclName( aName.str() );
    aName.freeze(false);

    // find a port to use
    int portOk = false;
    int err = 0;
    int bError = false;


    struct addrinfo hints;
    struct addrinfo *sa;
    memset(&hints, 0, sizeof(hints));
    hints.ai_flags = AI_PASSIVE;
    hints.ai_family = NetworkConfig::instance().getAddrFamily();
    hints.ai_socktype = SOCK_DGRAM;

    // here it assigns the port, the local port
    // & bind the addr(INADDR_ANY+port) to the socket
    for (int localPort = minPort; localPort <= maxPort; localPort++ )
    {
	char currport[6];
	sprintf(currport, "%u", localPort);
        cpLog(LOG_DEBUG_STACK, "getaddrinfo()");
	int error = getaddrinfo(NULL, currport, &hints, &sa);
	if (error) {
		perror(gai_strerror(error));
                continue;
	}


        cpLog(LOG_DEBUG, "Udp bind() fd =%d, port=%s", data->socketFd, currport); //port = %d(h_order), %d(n_order)", data->socketFd, localPort, (data->localAddr).sin6_port);


        if (bind(data->socketFd, sa->ai_addr, sa->ai_addrlen))
        {
            err = errno;

        #if !defined(WIN32)
            if ( err == EADDRINUSE )
            {
        #else
            /* Fix suggested by Anandprasanna Gaitonde, prasanna@controlnet.co.in */
        //    err = WSAGetLastError();
         //   if ( err == WSAEADDRINUSE )
          //  
        #endif
                freeaddrinfo(sa);
                continue;  // this port is in use - try the next one
            }

            int err = errno;
            strstream errMsg;
            errMsg << "UdpStack<" << getLclName() << ">::UdpStack error during socket bind: ";
            errMsg << strerror(err);
            errMsg << char(0);
		bError = true;
            cpLog(LOG_ERR, "%s",  errMsg.str());
        }
        else
        {
            portOk = true;
            memcpy(data->localAddr, sa->ai_addr, sa->ai_addrlen);
	    bError = false;
            if(sa->ai_family == AF_INET6)
            {
                cpLog(LOG_DEBUG, "(IPv6) Udp bound to fd = %d, port = %d",data->socketFd, localPort);
                //Set the sockoption so that we get get source IP
                //when running on the same host
                int on=1;
                setsockopt(data->socketFd, IPPROTO_IPV6, IPV6_PKTINFO, &on, sizeof(on));
            }
            else
            {
                cpLog(LOG_DEBUG, "(IPv4) Udp bound to fd = %d, port = %d",data->socketFd, localPort);
            }
        }
        freeaddrinfo(sa);
	if (portOk) break;
    }

    if (bError) throw UdpStackException("fecked");//errMsg.str());


    // deal with errors
    if (!portOk)
    {
        // all ports are in use
        //localPort = -1;
        strstream errMsg;
        errMsg << "UdpStack<" << getLclName()
        << ">::UdpStack all ports in range "
        << minPort << " to " << maxPort
        << " are in use.";
        errMsg << char(0);
        cpLog(LOG_ERR, errMsg.str());
        throw UdpStackExceptionPortsInUse(errMsg.str());
        errMsg.freeze(false);
        assert(0);
    }

    // reset name now that the port is defined
    strstream aName2;
    aName2 << "-receiver-" << ":" << minPort << char(0);
    setLclName( aName2.str() );
    aName2.freeze(false);
}

void
UdpStack::doClient ( const NetworkAddress* desHost) // This is the destination port
{
    
    //cpLog (LOG_DEBUG_STACK, "UdpStack::doClient");
    //cpLog (LOG_DEBUG_STACK, "desHost = %s, desPort = %d", 
    //           desHost->getIpName().c_str(), desHost->getPort());

    // this is a client
    assert (desHost);
    desHost->getSockAddr(data->remoteAddr);
}

void
UdpStack::connectPorts()
{
    if ((mode == recvonly) || (mode == inactive))
    {
        cpLog(LOG_ERR, "The UdpStack is recvonly or inactive.");
        return ;
    }

    int result;

    // conenct to server
    if ((result = connect(data->socketFd,
                          (struct sockaddr*) & (data->localAddr),
                          sizeof(*data->localAddr))) != 0)
    {
        int err = errno;
        strstream errMsg;
        errMsg << "UdpStack<" << getLclName() << " " << getRmtName()
        << ">::UdpStack error during socket connect: ";
        errMsg << strerror(err);
        errMsg << char(0);

        cpLog(LOG_ERR,  errMsg.str());
        throw UdpStackException(errMsg.str());
        errMsg.freeze(false);
        assert(0);
    }
}


// After testing, this method currently is not working on Linux and Sun4
// possibily because the system are not supporting it.
// Now the work around is to call connect twice.

void
UdpStack::disconnectPorts()
{
    if ((mode == recvonly) || (mode == inactive))
    {
        cpLog(LOG_ERR, "The UdpStack is recvonly or inactive.");
        return ;
    }

    struct sockaddr dummyAddr;

    memset((char*) &dummyAddr, 0, sizeof(dummyAddr));
    dummyAddr.sa_family = NetworkConfig::instance().getAddrFamily();

    int result;

    if ((result = connect(data->socketFd,
                          (struct sockaddr*) & dummyAddr,
                          sizeof(dummyAddr))) != 0)
    {
        int err = errno;
        strstream errMsg;
        errMsg << "UdpStack<" << getLclName() << " " << getRmtName()
        << ">::UdpStack error during socket connect: ";
        errMsg << strerror(err);
        errMsg << char(0);

        cpLog(LOG_ERR,  errMsg.str());
        errMsg.freeze(false);
#if 0
        throw UdpStackException(errMsg.str());
        assert(0);
#endif
    }

    dummyAddr.sa_family = AF_UNSPEC;
    if ((result = connect(data->socketFd,
                          (struct sockaddr*) & dummyAddr,
                          sizeof(dummyAddr))) != 0)
    {
        int err = errno;
        strstream errMsg;
        errMsg << "UdpStack<" << getLclName() << " " << getRmtName()
        << ">::UdpStack error during socket connect: ";
        errMsg << strerror(err);
        errMsg << char(0);

        cpLog(LOG_ERR, errMsg.str());
        errMsg.freeze(false);
#if 0
        throw UdpStackException(errMsg.str());
        assert(0);
#endif
    }
}

/// set the local ports
/// The first time change from inactive to recvonly/sendrecv or
/// change the local ports, this method needs to be called.
void
UdpStack::setLocal (const int minPort, int maxPort )
{
    cpLog (LOG_DEBUG_STACK, "UdpStack::setLocal");
    cpLog (LOG_DEBUG_STACK, "minPort = %d, maxPort = %d", minPort, maxPort);

    if ((mode == sendonly) || (mode == inactive))
    {
        cpLog(LOG_ERR, "The UdpStack is sendonly or inactive.");
        return ;
    }
    // To reopen a new socket, since after sendto(), bind() will fail
    //    if (localPort != -1)
    {
        int newFd;
        newFd = socket(NetworkConfig::instance().getAddrFamily(), 
                       SOCK_DGRAM, IPPROTO_UDP);
        if(NetworkConfig::instance().getAddrFamily() == AF_INET6)
        {
            //Set the sockoption so that we get get source IP
            //when running on the same host
            int on=1;
            setsockopt(data->socketFd, IPPROTO_IPV6, IPV6_PKTINFO, &on, sizeof(on));
        }
#ifndef WIN32
        if ( close (data->socketFd) != 0 )
#else
	if ( closesocket(data->socketFd) )
#endif
	{
            cpLog(LOG_ERR, "close socketFd error!");
	}
        data->socketFd = newFd;
        if ( data->socketFd < 0 )
        {
            int err = errno;
            strstream errMsg;
            errMsg << "UdpStack<" /* << getLclName() */
            << ">::UdpStack error during socket creation: ";
            errMsg << strerror(err);
            errMsg << char(0);

            cpLog(LOG_ERR,  errMsg.str());
            throw UdpStackException(errMsg.str());
            errMsg.freeze(false);
            assert(0);
        }
#ifdef __linux__
        int buf1 = 1;
        int len1 = sizeof(buf1);
        struct protoent * protoent;
        protoent = getprotobyname("icmp");

        if (!protoent)
        {
            fprintf(stderr, "Cannot get icmp protocol\n");
        }
        else
        {
            if (setsockopt(data->socketFd, protoent->p_proto, SO_BSDCOMPAT,
                           (char*)&buf1, len1)
                    == -1)
            {
                fprintf(stderr, "setsockopt error SO_BSDCOMPAT :%s",
                        strerror(errno));
            }
        }
#endif


    }
    doServer(minPort, maxPort);
}

/// set the default destination
void
UdpStack::setDestination ( const NetworkAddress* host )
{
    if ((mode == recvonly) || (mode == inactive))
    {
        cpLog(LOG_ERR, "The UdpStack is recvonly or inactive.");
        return ;
    }
    doClient(host);
}

void
UdpStack::setDestination ( const char* host, int port )
{
    assert(host);
    NetworkAddress netAddress;

    if ( host )
    {
        string rHostName = host;
        netAddress.setPort(port);
        netAddress.setHostName(rHostName);
    }

    setDestination (&netAddress);
}

int
UdpStack::getRxPort() {
        	cpLog(LOG_DEBUG_STACK, "getRxPort()");
		char service[6];
		if(getnameinfo((struct sockaddr *)data->localAddr, sizeof(*data->localAddr), NULL, 0, service, sizeof(service), NI_NUMERICSERV | NI_NUMERICHOST))
			perror("getnameinfo");
		return atoi(service);
}

int
UdpStack::getTxPort() {
        	cpLog(LOG_DEBUG_STACK, "getTxPort()");
		char service[6];
		if(getnameinfo((struct sockaddr *)data->remoteAddr, sizeof(*data->remoteAddr), NULL, 0, service, sizeof(service), NI_NUMERICSERV | NI_NUMERICHOST))
			perror("getnameinfo");
		return atoi(service);
        };


/// need to delete the ne wobject after the object is used
NetworkAddress *
UdpStack::getDestinationHost () const
{
	char host[256];
	char port[6];

    if(getnameinfo((struct sockaddr *)data->remoteAddr, sizeof(*data->remoteAddr), host, sizeof(host), port, sizeof(port), NI_NUMERICSERV | NI_NUMERICHOST))
	    perror("getnameinfo");

    NetworkAddress* desHost = new NetworkAddress(host, atoi(port)) ;
    return desHost;

}


int
UdpStack::getSocketFD ()
{
    assert(data);
    return data->socketFd;
}

void
UdpStack::addToFdSet ( fd_set* set )
{
    assert(set);
    FD_SET(data->socketFd, set);
}


int
UdpStack::getMaxFD ( int prevMax )
{
    return ( getSocketFD() > prevMax ) ? getSocketFD() : prevMax;
}


bool
UdpStack::checkIfSet ( fd_set* set )
{
    assert(set);
    return ( FD_ISSET(data->socketFd, set) ? true : false );
}


int
UdpStack::receive ( const char* buf, const int bufSize )
{
    if ((mode == sendonly) || (mode == inactive))
    {
        cpLog(LOG_ERR, "The stack is not in a state capable of receiving.");
        return -1;
    }

    int len = recv( data->socketFd,
                    (char *)buf, bufSize,
                    0 /*flags */);
    if ( len < 0 )
    {
        int err = errno;
	cpLog(LOG_ERR, "UdpStack: receive error: %s", strerror(err));
#if 0
        throw UdpStackException(errMsg.str());
        assert(0);
#endif
        //        cpLog(LOG_DEBUG_STACK, "UdpStack::receive pkt len=0");
    }
    else if (len == 0)
    {
	// received no bytes
	cpLog(LOG_DEBUG, "did not receive any data");
    }
    else
    {
        numBytesReceived += len;
        numPacketsReceived += 1;
    }

    if ( (logFlag) && (len > 0) )
    {
        strstream lenln1;
        lenln1 << ++rcvCount << " " << len << "\n" << char(0);
        in_log->write(lenln1.str(), strlen(lenln1.str()));
        in_log->write(buf, len);
        in_log->write(separator, 6);
        lenln1.freeze(false);
    }

    return len;
}


int
UdpStack::receiveFrom ( const char* buffer,
                        const int bufSize,
                        NetworkAddress* sender ) // returns bytes read
{
    if ((mode == sendonly) || (mode == inactive))
    {
        cpLog(LOG_ERR, "The stack is not capable to receive. ");
        return -1;
    }

    struct sockaddr_storage xSrc;

    int srcLen = sizeof(xSrc);
    if(sender) sender->getSockAddr(&xSrc);

#if defined(WIN32)

    /* WSAECONNRESET (10054) Connection reset by peer.
     * A existing connection was forcibly closed by the remote host. This
     * normally results if the peer application on the remote host is suddenly
     * stopped, the host is rebooted, or the remote host used a "hard close" (see
     * setsockopt for more information on the SO_LINGER option on the remote
     * socket.) This error may also result if a connection was broken due to
     * "keep-alive" activity detecting a failure while one or more operations are
     * in progress. Operations that were in progress fail with WSAENETRESET.
     * Subsequent operations fail with WSAECONNRESET. Fix suggested by Alexandr Kuzmin gin@nnov.stelt.ru.
     */

    int len = 0;
    do
    {
        len = recvfrom( data->socketFd,
                            (char *)buffer,
                            bufSize,
                            0 /*flags */ ,
                            (sockaddr*) &xSrc,
                            (socklen_t*) &srcLen);
    } while( (len == -1) && (WSAGetLastError() == WSAECONNRESET ) );
#else
    int len = 0;
    bool ipV6 = false;
    struct in6_pktinfo pktinfo;
    if(NetworkConfig::instance().getAddrFamily() == AF_INET)
    {
        len = recvfrom( data->socketFd,
                        (char *)buffer,
                        bufSize,
                        0 /*flags */ ,
                        (struct sockaddr*) &xSrc,
                        (socklen_t*) &srcLen);
    }
    else
    {
        ipV6 = true;
        int flags=0;
        len = recvfrom_flags( data->socketFd,
                        (char *)buffer,
                        bufSize,
                        &flags /*flags */ ,
                        (struct sockaddr*) &xSrc,
                        (socklen_t*) &srcLen,
                        &pktinfo);
    }

#endif
    if ( len <= 0 )
    {
        int err = errno;
        strstream errMsg;
        errMsg << "UdpStack<" << getLclName() << ">::receive error : ";
        errMsg << strerror(err);
        errMsg << char(0);

        cpLog(LOG_ERR, "%s", errMsg.str());
        errMsg.freeze(false);
    }
    else
    {
        if(sender)
        {
            struct sockaddr_storage xSrcT;
	    sender->getSockAddr(&xSrcT);
            if(memcmp(&xSrc, &xSrcT, srcLen)  == 0)
	    {
		//No need to set anything, return
                numBytesReceived += len;
                numPacketsReceived += 1;
		return len;
	    }
            //Do the heavy-weight work only if the caller shows interest
	    char hostname[256];
	    char port[64];
            hostname[0] = '\0';
            port[0] = '\0';
	    int err = getnameinfo((struct sockaddr *)&xSrc, srcLen, hostname, 256, port, 64, NI_NUMERICHOST | NI_NUMERICSERV);
	    if (err) {
                cpLog(LOG_ERR, "Failed to get the host name");
	    }

            string tmp = hostname;
            if(ipV6)
            {
                struct sockaddr_in6* sin6 = (struct sockaddr_in6*)&xSrc;
                if(IN6_IS_ADDR_LOOPBACK((struct in6_addr*)&sin6->sin6_addr) &&
                   (inet_ntop(AF_INET6, &pktinfo.ipi6_addr, hostname, 256)))
                {
	            tmp = "[";
	            tmp += hostname;
	            tmp += "]";
                }
                else
                {
	            tmp = "[";
	            tmp += hostname;
	            tmp += "]";
                }
            }
            cpLog(LOG_DEBUG, "***Received from:%s:%s", tmp.c_str(), port);
            sender->setPort(atoi(port));
            if((sender->getIpName() != Data(hostname)))
            {
                sender->setHostName(tmp.c_str());
            }
        }
        numBytesReceived += len;
        numPacketsReceived += 1;
    }

    if ( (logFlag) && (len > 0) )
    {
        strstream lenln2;
        lenln2 << ++rcvCount << " " << len << "\n" << char(0);
        in_log->write(lenln2.str(), strlen(lenln2.str()));
        in_log->write(buffer, len);
        in_log->write(separator, 6);
        lenln2.freeze(false);
    }

    return len;
}

int
UdpStack::receiveTimeout ( const char* buffer,
                           const int bufSize,
                           NetworkAddress* sender,
                           int sec,
                           int usec)
{
#ifndef __vxworks
    timeval tv;
    fd_set rset;
    int fd = getSocketFD();

// Make the coket non-blocking and then change it againb after the receivfrom
#ifndef WIN32
    int retVal;
    bool madeNonBlocking = false;
    if(blockingFlg)
    {
        madeNonBlocking = true;
        if(setModeBlocking(false) < 0)
            return -1;
    }
#else
    int retVal;
    unsigned long non_blocking = 1;
    if (ioctlsocket(fd, FIONBIO, &non_blocking))
        return -1;
#endif

    // select will return upon timeout, error or received message
    FD_ZERO(&rset);
    FD_SET(fd, &rset);
    tv.tv_sec = sec;
    tv.tv_usec = usec;
    retVal = select(fd + 1, &rset, NULL, NULL, &tv);
    // we don't care about no stinking error
    if (retVal <= 0)
        return retVal;


    retVal = receiveFrom( buffer,
                          bufSize,
                          sender);

#ifndef WIN32
    if(madeNonBlocking)
    {
        if(setModeBlocking(true) < 0);
            return -1;
    }
#else
	non_blocking = 0;
	if (ioctlsocket(fd, FIONBIO, &non_blocking) != 0)
	return -1;
#endif

    return retVal;
#else

    cpLog(LOG_ERR, "UdpStack::receiveTimeout  * not defined in vxworks *\n");
    return -1;
#endif
}

// uses send() which is better to get ICMP msg back
// function returns a 0  normally
void
UdpStack::transmit ( const char* buf, const int length )
{
    if ((mode == recvonly) || (mode == inactive))
    {
        cpLog(LOG_ERR, "The stack is not capable to transmit. ");
        return ;
    }

    assert(buf);
    assert(length > 0);

    if ( packetLossProbability > 0.0 )
    {
        static bool randInit = false;
        if (!randInit)
        {
            randInit = true;

            timeval tv;
            gettimeofday(&tv, NULL);

            long seed = tv.tv_sec + tv.tv_usec;

            srandom(seed);
        }

        double numerator( random() & 0x7FFFFFFF );
        double denominator( 0x7FFFFFFF );
        double prob = numerator / denominator;
        if ( prob < packetLossProbability )
        {
            // ok - just drop this packet
            return ;
        }
    }

    int count = send( data->socketFd,
                      (char *)buf, length,
                      0 /* flags */ );

    if ( count < 0 )
    {
        int err = errno;
        strstream errMsg;
        errMsg << "UdpStack<" << getRmtName() << ">::transmit ";

        switch (err)
        {
            case ECONNREFUSED:
            {
                // This is the most common error - you get it if the host
                // does not exist or is nor running a program to recevice
                // the packets. This is what you get with the other side
                // crashes.

                errMsg << "Connection refused by destination host";
                errMsg << char(0);
                cpLog(LOG_ERR, errMsg.str());

#if 0
                throw UdpStackExceptionConectionRefused(errMsg.str());
#endif
            }
            break;
            case EHOSTDOWN:
            {
                errMsg << "destination host is down";
                errMsg << char(0);
                cpLog(LOG_ERR, errMsg.str());

            }
            break;
            case EHOSTUNREACH:
            {
                errMsg << "no route to to destination host";
                errMsg << char(0);
                cpLog(LOG_ERR,  errMsg.str());

            }
            break;
            default:
            {
                errMsg << ": " << strerror(err);
                errMsg << char(0);
                cpLog(LOG_ERR, errMsg.str());
            }
        }
 
        cpLog (LOG_ERR, "UDP send() error: ");

        errMsg.freeze(false);
#if 0
        throw UdpStackException(errMsg.str());
        assert(0);
#endif
    }
    else if ( count != length )
    {
        /*
                int err = errno;
        */
        strstream errMsg;
        errMsg << "UdpStack<" << getRmtName()
        << ">:transmit error is send: "
        << "Asked to transmit " << length
        << " bytes but only sent " << count ;
        errMsg << char(0);

        cpLog(LOG_ERR,  errMsg.str());
        errMsg.freeze(false);
#if 0
        throw UdpStackException(errMsg.str());
        assert(0);
#endif
    }
    else
    {
        numBytesTransmitted += count;
        numPacketsTransmitted += 1;
    }


    if ( (logFlag) && (count > 0) )
    {
        strstream lenln3;
        lenln3 << ++sndCount << " " << count << char(0);
        out_log->write(lenln3.str(), strlen(lenln3.str()));
        lenln3.freeze(false);

        strstream rAddress1;
        rAddress1 << " " << getRmtName() << "\n" << char(0);
        out_log->write(rAddress1.str(), strlen(rAddress1.str()));
        rAddress1.freeze(false);

        out_log->write(buf, count);
        out_log->write(separator, 6);
    }

}


int
UdpStack::transmitTo ( const char* buffer,
                       const int length,
                       const NetworkAddress* dest )
{

    if ((mode == recvonly) || (mode == inactive))
    {
        cpLog(LOG_ERR, "The stack is not capable to transmit. ");
        return 4;
    }

    assert(buffer);
    assert(length > 0);
    assert(dest);

    //SP LockHelper helper(_lock);
    //SP setDestination(dest);
    struct sockaddr_storage xDest;
    memset(&xDest, 0, sizeof(xDest));
    dest->getSockAddr(&xDest);
    int count = sendto( data->socketFd,
                        (char*)buffer,
                        length,
                        0 ,  // flags
                        (struct sockaddr*) & xDest,
                        sizeof(struct sockaddr));

    if ( count < 0 )
    {
        int err = errno;
        strstream errMsg;
        errMsg << "UdpStack<" << getRmtName() << ">::transmitTo ";

        switch (err)
        {
            case ECONNREFUSED:
            {
                // This is the most common error - you get it if the host
                // does not exist or is nor running a program to recevice
                // the packets. This is what you get with the other side
                // crashes.

                errMsg << "Connection refused by destination host";
                errMsg << char(0);
                cpLog(LOG_ERR,  errMsg.str());
                return 1;
#if 0
                throw UdpStackExceptionConectionRefused(errMsg.str());
#endif
            }
            break;
            case EHOSTDOWN:
            {
                errMsg << "destination host is down";
                errMsg << char(0);
                cpLog(LOG_ERR,  errMsg.str());
                return 2;
            }
            break;
            case EHOSTUNREACH:
            {
                errMsg << "no route to to destination host";
                errMsg << char(0);
                cpLog(LOG_ERR,  errMsg.str());
                return 3;
            }
            break;
            default:
            {
                errMsg << ": " << strerror(err); // << " *** " << host;
                //assert(0);
                errMsg << char(0);
                cpLog(LOG_ERR,  errMsg.str());
                return 3;
            }
        }

        cpLog (LOG_ERR, "UDP sendto() error: ");

        errMsg.freeze(false);
#if 0
        throw UdpStackException(errMsg.str());
        assert(0);
#endif
    }
    else if ( count != length )
    {

        //        int err = errno;

        strstream errMsg;
        errMsg << "UdpStack<" << getRmtName()
        << ">:transmit error is send: "
        << "Asked to transmit " << length
        << " bytes but only sent " << count ;
        errMsg << char(0);

        cpLog(LOG_ERR, errMsg.str());
        errMsg.freeze(false);
#if 0
        throw UdpStackException(errMsg.str());
        assert(0);
#endif
    }
    else
    {
        numBytesTransmitted += count;
        numPacketsTransmitted += 1;
    }

    if ( (logFlag) && (count > 0) )
    {
        strstream lenln4;
        lenln4 << ++sndCount << " " << count << char(0);
        out_log->write(lenln4.str(), strlen(lenln4.str()));
        lenln4.freeze(false);

        strstream rAddress2;
        rAddress2 << " " << getRmtName() << "\n" << char(0);
        out_log->write(rAddress2.str(), strlen(rAddress2.str()));
        rAddress2.freeze(false);

        out_log->write(buffer, count);
        out_log->write(separator, 6);
    }

    return 0;
}


///
void
UdpStack::joinMulticastGroup ( NetworkAddress group,
                               NetworkAddress* iface,
                               int ifaceInexe )
{
// Previously for win32 platforms this function did nothing
// Now it does what it should do using the ip_mreq structure defined
// in W32McastCfg.hxx. contact nismail@cisco.com
//#ifndef WIN32

#if defined(__linux__)
    if(NetworkConfig::instance().getAddrFamily() == PF_INET)
    {
        cpLog(LOG_INFO, "Interface (%s) index (%d) joining multicast group (%s)",
                     iface->getIpName().c_str(), ifaceInexe,
                     group.getIpName().c_str());
        struct ip_mreqn mreqn;
        struct sockaddr_storage groupAddr;
        group.getSockAddr(&groupAddr);
        memcpy(&mreqn.imr_multiaddr,
               &(((struct sockaddr_in*)&groupAddr)->sin_addr),
               sizeof(struct in_addr));
        struct sockaddr_storage intfAddr;
        iface->getSockAddr(&intfAddr);
        memcpy(&mreqn.imr_address,
               &((struct sockaddr_in*)&intfAddr)->sin_addr,
               sizeof(struct in_addr));
        mreqn.imr_ifindex = ifaceInexe;

        int ret;
        ret = setsockopt (getSocketFD(),
                      IPPROTO_IP,
                      IP_ADD_MEMBERSHIP,
                      (char*) & mreqn,
                      sizeof(struct ip_mreqn));
        if(ret < 0)
        {
            cpLog(LOG_ERR, "Failed to join multicast group on interface %s, reason:%s", iface->getIpName().c_str(), strerror(errno));
        }
        else
        {
           cpLog(LOG_INFO, "Joined multi-cast group");
        }
    }
    else 
    {
        //Join to multi-cast group
        struct ipv6_mreq mreq6;
        string mCastGroup("ff13::1");
        if(inet_pton(AF_INET6, mCastGroup.c_str(), &mreq6.ipv6mr_multiaddr) < 0)
        {
            cpLog(LOG_ERR, "Failed to get the address for multicast group %s", mCastGroup.c_str());
            return;
        }
        cpLog(LOG_INFO, "Interface (%s) index (%d) joining multicast group (%s)",
                     iface->getIpName().c_str(), ifaceInexe,
                     mCastGroup.c_str());
        if(ifaceInexe > 0)
        {
            mreq6.ipv6mr_interface = ifaceInexe;
        }
        else
        {
            mreq6.ipv6mr_interface = 0;
        }
        int ret;
        ret = setsockopt (getSocketFD(),
                      IPPROTO_IPV6,
                      IPV6_ADD_MEMBERSHIP,
                      (char*) & mreq6,
                      sizeof(mreq6));
        if(ret < 0)
        {
            cpLog(LOG_ERR, "Failed to join multicast group on interface %s, reason:%s", iface->getIpName().c_str(), strerror(errno));
        }
        else
        {
           cpLog(LOG_INFO, "Joined multi-cast group");
        }
    }
#else
struct ip_mreq mreq;
    mreq.imr_multiaddr.s_addr = (group.getIp4Address());

    mreq.imr_interface.s_addr = (iface->getIp4Address());
    //    mreq.imr_ifindex = ifaceInexe;

    int ret;
    ret = setsockopt (getSocketFD(),
                      IPPROTO_IP,
                      IP_ADD_MEMBERSHIP,
                      (char*) & mreq,
                      sizeof(struct ip_mreq));
#endif
//#endif // !WIN32
}

///
void
UdpStack::leaveMulticastGroup( NetworkAddress group,
                               NetworkAddress* iface,
                               int ifaceInexe )
{
// Previously for win32 platforms this function did nothing
// Now it does what it should do using the ip_mreq structure defined
// in W32McastCfg.hxx. contact nismail@cisco.com//#ifndef WIN32
//#ifndef WIN32
#if defined(__linux__)
    struct ip_mreqn mreqn;
    mreqn.imr_multiaddr.s_addr = (group.getIp4Address());
    mreqn.imr_address.s_addr = (iface->getIp4Address());
    mreqn.imr_ifindex = ifaceInexe;

    setsockopt (getSocketFD(),
                IPPROTO_IP,
                IP_DROP_MEMBERSHIP,
                (char*)&mreqn,
                sizeof(struct ip_mreqn));
#else
    struct ip_mreq mreq;
    mreq.imr_multiaddr.s_addr = (group.getIp4Address());
    mreq.imr_interface.s_addr = (iface->getIp4Address());
    //    mreq.imr_ifindex = ifaceInexe;

    setsockopt (getSocketFD(),
                IPPROTO_IP,
                IP_DROP_MEMBERSHIP,
                (char*)&mreq,
                sizeof(struct ip_mreq));
#endif
//#endif // !WIN32
}

UdpStack::~UdpStack()
{
    assert(data);
    if (logFlag)
    {
        in_log->close();
        out_log->close();

        delete in_log;
        delete out_log;
    }
#ifndef WIN32
    close (data->socketFd);
#else
    closesocket(data->socketFd);
#endif

    delete data->localAddr;
    delete data->remoteAddr;
    delete data;
    data = NULL;
}


int
UdpStack::getBytesReceived () const
{
    return numBytesReceived;
}

int
UdpStack::getPacketsReceived () const
{
    return numPacketsReceived;
}

int
UdpStack::getBytesTransmitted () const
{
    return numBytesTransmitted;
}

int
UdpStack::getPacketsTransmitted () const
{
    return numPacketsTransmitted;
}

int
UdpStack::recvfrom_flags(int fd, void *ptr, size_t nbytes, int *flagsp,
                         struct sockaddr *sa, socklen_t *salenptr, 
                         struct in6_pktinfo *pktp)
{
    struct msghdr   msg;
    struct iovec    iov[1];
    int             n;
    struct cmsghdr  *cmptr;
#ifndef CMSG_LEN
#define CMSG_LEN(size)    (sizeof (struct cmsghdr) + (size))
#endif
#ifndef CMSG_SPACE
#define CMSG_SPACE(size)  (sizeof (struct cmsghdr) + (size))
#endif
    union {
      struct cmsghdr        cm;
      char   control[CMSG_SPACE(sizeof(struct in6_addr)) +
                 CMSG_SPACE(sizeof(struct in6_pktinfo))];
    } control_un;
#if defined (__SUNPRO_CC)
    msg.msg_accrights = control_un.control;
    msg.msg_accrightslen = sizeof(control_un.control);
#else

    msg.msg_control = control_un.control;
    msg.msg_controllen = sizeof(control_un.control);
    msg.msg_flags = 0;
#endif
    msg.msg_name = sa;
    msg.msg_namelen = *salenptr;
#if defined (__FreeBSD__) || defined(__SUNPRO_CC)
    iov[0].iov_base = reinterpret_cast<char *>(ptr);
#else
    iov[0].iov_base = ptr;
#endif
    iov[0].iov_len = nbytes;
    msg.msg_iov = iov;
    msg.msg_iovlen = 1;

    if ( (n = recvmsg(fd, &msg, *flagsp)) < 0)
    {
        return(n);
    }

    *salenptr = msg.msg_namelen;    
    if (pktp)
       memset(pktp, 0, sizeof(struct in6_pktinfo)); 

    if(flagsp)
#if defined (__SUNPRO_CC)
       if (msg.msg_accrightslen < sizeof(struct cmsghdr) || pktp == NULL)
    {
        return(n);
    }
#else
       *flagsp = msg.msg_flags;                /* pass back results */

    if (msg.msg_controllen < sizeof(struct cmsghdr) ||
            (msg.msg_flags & MSG_CTRUNC) || pktp == NULL)
    {
        return(n);
    }
#endif

#if defined(__sparc)
/* To maintain backward compatibility, alignment needs to be 8 on sparc. */
#ifndef  _CMSG_HDR_ALIGNMENT
#define _CMSG_HDR_ALIGNMENT     8
#endif
#endif
#ifndef  _CMSG_HDR_ALIGN
#define _CMSG_HDR_ALIGN(x)      (((uintptr_t)(x) + _CMSG_HDR_ALIGNMENT - 1) & \
                                    ~(_CMSG_HDR_ALIGNMENT - 1))
#endif

#ifndef _CMSG_DATA_ALIGNMENT
#define _CMSG_DATA_ALIGNMENT   (sizeof (int))
#endif

#ifndef  _CMSG_DATA_ALIGN
#define _CMSG_DATA_ALIGN(x)     (((uintptr_t)(x) + _CMSG_DATA_ALIGNMENT - 1) & \
                                  ~(_CMSG_DATA_ALIGNMENT - 1))
#endif

#ifndef CMSG_FIRSTHDR
//#define CMSG_FIRSTHDR(size)    (sizeof (struct cmsghdr) + (size))
#define CMSG_FIRSTHDR(m)        ((struct cmsghdr *)((m)->msg_accrights))
#endif

#ifndef  CMSG_DATA
#define CMSG_DATA(c)                                                    \
        ((unsigned char *)_CMSG_DATA_ALIGN((struct cmsghdr *)(c) + 1))
#endif

#ifndef CMSG_NXTHDR
//#define CMSG_NXTHDR(size)  (sizeof (struct cmsghdr) + (size))
#define CMSG_NXTHDR(m, c)                                               \
        ((((uintptr_t)_CMSG_HDR_ALIGN((char *)(c) +                     \
        ((struct cmsghdr *)(c))->cmsg_len) + sizeof (struct cmsghdr)) > \
        (((uintptr_t)((struct msghdr *)(m))->msg_accrights) +             \
        ((uintptr_t)((struct msghdr *)(m))->msg_accrightslen))) ?         \
        ((struct cmsghdr *)0) :                                         \
        ((struct cmsghdr *)_CMSG_HDR_ALIGN((char *)(c) +                \
            ((struct cmsghdr *)(c))->cmsg_len)))
#endif

    for (cmptr = CMSG_FIRSTHDR(&msg); cmptr != NULL;
             cmptr = CMSG_NXTHDR(&msg, cmptr))
    {
        if (cmptr->cmsg_level == IPPROTO_IPV6 &&
            cmptr->cmsg_type == IPV6_PKTINFO) 
        {
            memcpy(&pktp->ipi6_addr, CMSG_DATA(cmptr),
                               sizeof(struct in6_addr));
            continue;
        }
        cpLog(LOG_DEBUG, "unknown ancillary data, len = %d, level = %d, type = %d",
               cmptr->cmsg_len, cmptr->cmsg_level, cmptr->cmsg_type);
    }
    return(n);
}

int
UdpStack::setModeBlocking(bool flg)
{
    if(blockingFlg != flg)
    {
        blockingFlg = flg;
        if(blockingFlg)
        {
            int flags, fd;
            fd = getSocketFD();
            if ((flags = fcntl(fd, F_GETFL, 0)) < 0)
            {
                cpLog(LOG_ERR, "Failed to get block flag, reason:%s", strerror(errno));
                return -1;
            }
            flags &= ~O_NONBLOCK;
            if (fcntl(fd, F_SETFL, flags) < 0)
            {
                cpLog(LOG_ERR, "Failed to make socket blocking, reason:%s", strerror(errno));
                return -1;
            }
        }
        else
        {
            int flags, fd;
            fd = getSocketFD();
            if ((flags = fcntl(fd, F_GETFL, 0)) < 0)
            {
                cpLog(LOG_ERR, "Failed to get block flag, reason:%s", strerror(errno));
                return -1;
            }
            flags |= O_NONBLOCK;
            if (fcntl(fd, F_SETFL, flags) < 0)
            {
                cpLog(LOG_ERR, "Failed to make socket non-block, reason:%s", strerror(errno));
                return -1;
            }
        }
    }
    return 0;
}

