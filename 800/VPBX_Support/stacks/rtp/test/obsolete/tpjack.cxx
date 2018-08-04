/*
* $Log: tpjack.cxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:09  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: tpjack.cxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/
/*----------------------------< Defines >-------------------------------*/
/* stdlib */
#include <stdio.h>
//#include <varargs.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <syslog.h>

#include <fstream>

/* other */
#include "ixjuser.h"
#include "RtpSession.hxx"


/*----------------------------< Includes >------------------------------*/
/*---------------------------< Definitions >----------------------------*/
#define LINELEN 128
#define BUFSIZE 256
/*--------------------------< Declarations >----------------------------*/
/*------------------------< Global Variables >--------------------------*/
static char szDevice[32];
/*-------------------------< Local Variables >--------------------------*/
int RTPjack(char *host, int nPort);
int connectTCP(char *host, char *service);
int connectsock(char *host, char *service, char *protocol);
/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
int
main(int argc, char *argv[])
{
    char *host = "localhost";
    int nPort = 7000;

    switch (argc)
    {
        //    case 2:
        //      strncpy(szDevice,argv[1],sizeof(szDevice));
        //      break;
        case 3:
        // strncpy(szDevice,"/dev/ixj0",sizeof(szDevice));
        strncpy(szDevice, "/dev/phone0", sizeof(szDevice));
        host = argv[1];
        nPort = atoi(argv[2]);
        break;
        //    case 4:
        //      strncpy(szDevice,argv[1],sizeof(szDevice));
        //      host=argv[2];
        //      nPort=atoi(argv[3]);
        break;
        default:
        //      fprintf(stderr,"usage: tpjack dev [host [port]]\n");
        //      fprintf(stderr," - dev is probably /dev/ixj0\n");
        fprintf(stderr, "usage: tpjack host port\n");
        exit(1);
    }
    RTPjack(host, nPort);
    exit(0);
}
/*----------------------------------------------------------------------*/
int
RTPjack(char *host, int nPort)
{
    char serv[16];
    int fd, read_fd, send_fd, readc_fd, sendc_fd, cc, ixj, hook, retval, nMax;
    fd_set rfds;
    struct timeval tv;

    printf("Opening Phone Jack (%s)\n", szDevice);
    ixj = open(szDevice, O_RDWR);
    if (ixj < 0)
    {
        printf("Error opening voice card, exiting\n");
        return -1;
    }

    printf("Waiting for hookswitch\n");
    hook = ioctl(ixj, IXJCTL_HOOKSTATE);
    while (hook != 1)
    {
        hook = ioctl(ixj, IXJCTL_HOOKSTATE);
    }
    /* connect the signalling port */
    sprintf(serv, "%d", nPort);
    fd = connectTCP(host, serv);

    printf("connecting to %s\n", host);
    printf(" - signalling on port %d with tcp socket %d\n", nPort, fd);

    /* connect the data ports */
    RtpSession dataStack (host, nPort + 1, nPort + 1, nPort + 2, nPort + 2);
    RtpPacket* outPacket = dataStack.createPacket (BUFSIZE);
    RtpPacket* inPacket = NULL;

    /* setting SDES information */
    //RtcpTransmitter* rtcpTran = dataStack.getRtcpTran();
    //rtcpTran->setSDES_LOC("Left computer");

    read_fd = (dataStack.getRtpRecv())->getSocketFD();
    send_fd = (dataStack.getRtpTran())->getSocketFD();
    readc_fd = (dataStack.getRtcpRecv())->getSocketFD();
    sendc_fd = (dataStack.getRtcpTran())->getSocketFD();

    if ((read_fd < 0) || (send_fd < 0) || (readc_fd < 0) || (sendc_fd < 0))
        printf("failure on RTP socket\n");
    printf(" - incoming voice on port %d using RTP sockets %d and %d\n",
           nPort, read_fd, send_fd);

    tv.tv_sec = 0;
    tv.tv_usec = 300;
    if (nMax < ixj) nMax = ixj;
    if (nMax < fd) nMax = fd;
    if (nMax < read_fd) nMax = read_fd;
    if (nMax < send_fd) nMax = send_fd;
    if (nMax < readc_fd) nMax = readc_fd;
    if (nMax < sendc_fd) nMax = sendc_fd;

    nMax++;
    /* do the loop */
    if (read_fd != 0)
    {
        ioctl(ixj, IXJCTL_AEC_START);
        ioctl(ixj, IXJCTL_PLAY_CODEC, ULAW);
        ioctl(ixj, IXJCTL_REC_CODEC, ULAW);
        ioctl(ixj, IXJCTL_REC_START);
        ioctl(ixj, IXJCTL_PLAY_START);

        while (hook = ioctl(ixj, IXJCTL_HOOKSTATE))
        {
            FD_ZERO(&rfds);
            FD_SET(read_fd, &rfds);
            FD_SET(readc_fd, &rfds);
            FD_SET(ixj, &rfds);
            tv.tv_sec = 0;
            tv.tv_usec = 300;
            retval = select(nMax, &rfds, NULL, NULL, &tv);
            if (FD_ISSET(ixj, &rfds))                    // read data from phone
            {
                cc = read(ixj, outPacket->getPayloadLoc(), 240);
                if (cc > 0)
                {
                    assert (cc == 240);
                    outPacket->setPayloadUsage(cc);
                    outPacket->setRtpTime(dataStack.getPrevRtpTime() + 240);
                    dataStack.transmit(outPacket);
                }
            }
            if (FD_ISSET(read_fd, &rfds))                // read data from network
            {
                inPacket = dataStack.receive();
                if (inPacket)
                {
                    assert (inPacket->getPayloadUsage() == 240);
                    write(ixj, inPacket->getPayloadLoc(), inPacket->getPayloadUsage());
                }
                else
                {
                    /* if(errno!=EAGAIN)
                       {
                         ioctl(ixj, IXJCTL_REC_STOP);
                         ioctl(ixj, IXJCTL_PLAY_STOP);
                         ioctl(ixj, IXJCTL_AEC_STOP);
                         printf("echo read: %s\n",sys_errlist[errno]);
                       }  */
                }
            }
            dataStack.processRTCP();
        } /* end while() */
    }
    dataStack.transmitRTCPBYE();
    close(read_fd);
    close(send_fd);
    close(readc_fd);
    close(sendc_fd);
    close(fd);
    close(ixj);
    return 0;
}
/*----------------------------------------------------------------------*/
int
connectTCP(char *host, char *service)
{
    return connectsock(host, service, "tcp");
}
/*----------------------------------------------------------------------*/
int
connectsock(char *host, char *service, char *protocol)
{
    struct hostent *phe;
    struct servent *pse;
    struct protoent *ppe;
    struct sockaddr_in sin;
    int s, type;

    bzero((char *)&sin, sizeof(sin));
    sin.sin_family = AF_INET;

    if (pse = getservbyname(service, protocol))
        sin.sin_port = pse->s_port;
    else if ((sin.sin_port = htons((u_short)atoi(service))) == 0)
        printf("can't get \"%s\" service entry\n", service);

    if (phe = gethostbyname(host))
        bcopy(phe->h_addr, (char *) & sin.sin_addr, phe->h_length);
    else if ((sin.sin_addr.s_addr = inet_addr(host)) == INADDR_NONE)
        printf("can't get \"%s\" host entry\n", host);

    if ((ppe = getprotobyname(protocol)) == 0)
        printf("can't get \"%s\" protocol entry\n", protocol);

    if (strcmp(protocol, "udp") == 0)
        type = SOCK_DGRAM;
    else
        type = SOCK_STREAM;

    s = socket(PF_INET, type, ppe->p_proto);
    if (s < 0)
        printf("can't create socket: %s\n", sys_errlist[errno]);

    if (connect(s, (struct sockaddr *)&sin, sizeof(sin)) < 0)
        printf("can't connect to %s.%s: %s\n", host, service, sys_errlist[errno]);

    return s;
}
/*----------------------------------------------------------------------*/
