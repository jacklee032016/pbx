/*
* $Log: RtpSession.cxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:09  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: RtpSession.cxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#include "global.h"
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/types.h>
#include "vtypes.h"
#include "cpLog.h"
#include "vsock.hxx"

#include "rtpTypes.h"
#include "Rtp.hxx"
#include "Rtcp.hxx"
#include "RtpSession.hxx"



/* ----------------------------------------------------------------- */
/* --- rtp session Constructor ------------------------------------- */
/* ----------------------------------------------------------------- */

RtpSession::RtpSession (const char* remoteHost, int remotePort, int localPort,
                        int rtcpRemotePort, int rtcpLocalPort, int portRange,
                        RtpPayloadType apiPayloadType,
                        RtpPayloadType networkPayloadType, int jitterNew)
{
    constructRtpSession (remoteHost, remotePort, localPort, rtcpRemotePort,
                         rtcpLocalPort, portRange, apiPayloadType,
                         networkPayloadType, jitterNew);
}

RtpSession::RtpSession (const char* remoteHost, int remotePort, int localPort,
                        int rtcpRemotePort, int rtcpLocalPort,
                        RtpPayloadType apiPayloadType,
                        RtpPayloadType networkPayloadType, int jitterNew)
{
    constructRtpSession (remoteHost, remotePort, localPort, rtcpRemotePort,
                         rtcpLocalPort, 0, apiPayloadType,
                         networkPayloadType, jitterNew);
}

void RtpSession::constructRtpSession (const char* remoteHost,
                                      int remotePort, int localPort,
                                      int rtcpRemotePort, int rtcpLocalPort,
                                      int portRange, RtpPayloadType apiPayloadType,
                                      RtpPayloadType networkPayloadType,
                                      int jitterNew)
{

    cpDebug(LOG_DEBUG, "Creating RTP session.");
    cpDebug(LOG_DEBUG, "Listening on port: %d", localPort);
    cpDebug(LOG_DEBUG, "Sending to remote host: %s", remoteHost);
    cpDebug(LOG_DEBUG, "Sending to remote port: %d", remotePort);

    recv = NULL; tran = NULL;
    rtcpTran = NULL, rtcpRecv = NULL;

    if (localPort != 0)
    {
        if (portRange != 0)
            recv = new RtpReceiver (localPort, localPort + portRange,
                                    apiPayloadType, networkPayloadType,
                                    jitterNew);
        else
            recv = new RtpReceiver (localPort, apiPayloadType,
                                    networkPayloadType, jitterNew);
    }

    if (remotePort != 0)
    {
        if (portRange != 0)
            tran = new RtpTransmitter (remoteHost, remotePort,
                                       remotePort + portRange, apiPayloadType,
                                       networkPayloadType, recv);
        else
            tran = new RtpTransmitter (remoteHost, remotePort, apiPayloadType,
                                       networkPayloadType, recv);
    }

    if (rtcpLocalPort != 0)
    {
        if (portRange != 0)
            rtcpRecv = new RtcpReceiver (rtcpLocalPort,
                                         rtcpLocalPort + portRange);
        else
            rtcpRecv = new RtcpReceiver (rtcpLocalPort);
    }

    if (rtcpRemotePort != 0)
    {
        if (portRange != 0)
            rtcpTran = new RtcpTransmitter (remoteHost, rtcpRemotePort,
                                            rtcpRemotePort + portRange, rtcpRecv);
        else
            rtcpTran = new RtcpTransmitter (remoteHost,
                                            rtcpRemotePort, rtcpRecv);
    }


    // update interlinks
    if (rtcpTran && tran) rtcpTran->setRTPtran (tran);
    if (rtcpTran && recv) rtcpTran->setRTPrecv (recv);
    if (rtcpTran && rtcpRecv) rtcpTran->setRTCPrecv (rtcpRecv);
    if (rtcpRecv && recv) recv->setRTCPrecv(rtcpRecv);


    // SDES infromation for transmitter
    if (rtcpTran && tran)
    {
        char dummy[2] = "";

        rtcpTran->setSdesCname();
        rtcpTran->setSdesName(dummy);
        rtcpTran->setSdesEmail(dummy);
        rtcpTran->setSdesPhone(dummy);
        rtcpTran->setSdesLoc(dummy);
        rtcpTran->setSdesTool(dummy);
        rtcpTran->setSdesNote(dummy);
    }


    // session states
    // Currently the states are for RTP stack only, not for RTCP
    if (tran && recv)
    {
        sessionState = rtp_session_sendrecv;
        recv->getUdpStack()->setMode(sendrecv);
    }
    else if (tran && !recv)
    {
        sessionState = rtp_session_sendonly;
        tran->getUdpStack()->setMode(sendonly);
    }
    else if (recv && !tran)
    {
        sessionState = rtp_session_recvonly;
        recv->getUdpStack()->setMode(recvonly);
    }
    else
    {
        cpLog (LOG_ERR, "Session undefined");
        sessionState = rtp_session_undefined;
    }


    if (tran) cpLog (LOG_DEBUG_STACK, "RTP Tran Port: %d",
                         tran->getUdpStack()->getDestinationPort());
    if (recv) cpLog (LOG_DEBUG_STACK, "RTP Recv Port: %d", recv->getPort());
    if (rtcpTran) cpLog (LOG_DEBUG_STACK, "RTCP Tran Port: %d",
                             rtcpTran->getUdpStack()->getDestinationPort());
    if (rtcpRecv) cpLog (LOG_DEBUG_STACK, "RTCP Recv Port: %d",
                             rtcpRecv->getPort());
}

RtpSession::~RtpSession ()
{
    if( rtcpTran )
    {
        delete rtcpTran;
        rtcpTran = NULL;
    }
    if( rtcpRecv )
    {
        delete rtcpRecv;
        rtcpRecv = NULL;
    }
    if( tran )
    {
        delete tran;
        tran = NULL;
    }
    if( recv )
    {
        delete recv;
        recv = NULL;
    }
}

int
RtpSession::reserveRtpPort(int localMin, int localMax)
{
    int port = 0;

    if ( recv == 0 )
    {
        try
        {
            //let RtpReceiver() automatically generate a port number
            recv = new RtpReceiver(localMin, localMax);
            port = recv->getPort();
        }
        catch ( UdpStackExceptionPortsInUse& )
        {
            if ( localMin == localMax )
                cpLog( LOG_ERR, "port %d is not available", localMin );
            else
                cpLog( LOG_ERR, "No ports between %d and %d are available", localMin, localMax);
            recv = 0;
        }
    }
    else
    {
        port = recv->getPort();
    }

    return port;
}

int
RtpSession::releaseRtpPort()
{
    int port = 0;
    if ( recv != 0 )
    {
        port = recv->getPort();
        delete recv;
        recv = 0;
    }
    return port;
}

int
RtpSession::reserveRtcpPort(int rtcpLocalPort, int portRange)
{
    int port = 0;

    if ( rtcpRecv == 0 )
    {
        try
        {
            if (rtcpLocalPort != 0)
            {
                if (portRange != 0)
                    rtcpRecv = new RtcpReceiver (rtcpLocalPort,
                                                 rtcpLocalPort + portRange);
                else
                    rtcpRecv = new RtcpReceiver (rtcpLocalPort);
            }

            port = rtcpRecv->getPort();
        }
        catch ( UdpStackExceptionPortsInUse& )
        {
            if ( portRange == 0 )
                cpLog( LOG_ERR, "port %d is not available", rtcpLocalPort );
            else
                cpLog( LOG_ERR, "no ports between %d and %d are available", rtcpLocalPort, rtcpLocalPort + portRange );
            rtcpRecv = 0;
        }
    }
    else
    {
        port = rtcpRecv->getPort();
    }

    return port;
}

int
RtpSession::releaseRtcpPort()
{
    int port = 0;
    if ( rtcpRecv != 0 )
    {
        port = rtcpRecv->getPort();
        delete rtcpRecv;
        rtcpRecv = 0;
    }
    return port;
}

/* --- Send and Receive RTP Functions ------------------------------ */
void RtpSession::setNetworkFormat (RtpPayloadType type, int no_samples, int packetSize)
{
    if (tran) tran->setNetworkFormat (type, no_samples, packetSize);
    if (recv) recv->setNetworkFormat (type, no_samples, packetSize);
}

void RtpSession::setApiFormat (RtpPayloadType type, int no_samples, int packetSize)
{
    if (tran) tran->setApiFormat (type, no_samples, packetSize);
    if (recv) recv->setApiFormat (type, no_samples, packetSize);
}

void RtpSession::setCodecString (const char* codecStringInput)
{
    if (tran) tran->setCodecString (codecStringInput);
    if (recv) recv->setCodecString (codecStringInput);
}

void RtpSession::setNetworkPktSize (int no_samples)
{
    setNetworkPktSampleSize (no_samples);
}

void RtpSession::setNetworkPktSampleSize (int no_samples)
{
    if (tran) tran->setNetworkPktSize (no_samples);
    if (recv) recv->setNetworkPktSize (no_samples);
}


void RtpSession::setApiPktSize (int no_samples)
{
    setApiPktSampleSize (no_samples);
}

void RtpSession::setApiPktSampleSize (int no_samples)
{
    if (tran) tran->setApiPktSize (no_samples);
    if (recv) recv->setApiPktSize (no_samples);
}


int RtpSession::getNetworkPktSampleSize ()
{
    if (tran)
        return tran->getNetworkPktSampleSize ();
    else
        return 0;
}

int RtpSession::getApiPktSampleSize ()
{
    if (recv)
        return recv->getApiPktSampleSize();
    else
        return 0;
}

// If there are multiple dests, the return will be the total
// of all dests. - ?
int RtpSession::getPacketSent ()
{
    if (tran)
        return tran->getPacketSent ();
    else
        return 0;
}

// If there are multiple dests, the return will be the total
// of all dests. - ?
int RtpSession::getByteSent ()
{
    if (tran)
        return tran->getPayloadSent ();
    else
        return 0;
}

// If there are multiple srcs, the return will be the total
// of all srcs. - ?
int RtpSession::getPacketReceived ()
{
    if (recv)
        return recv->getPacketReceived ();
    else
        return 0;
}

// If there are multiple srcs, the return will be the total
// of all srcs. - ?
int RtpSession::getByteReceived ()
{
    if (recv)
        return recv->getPayloadReceived ();
    else
        return 0;
}

// If there are multiple srcs, this func returns the packect lost
// for all the srcs
int RtpSession::getPacketLost ()
{
    if (rtcpTran)
    {
        if (rtcpRecv)
        {
            int lost = 0;
            for (int i = 0; i < (rtcpRecv->getTranInfoCount()); i++)
            {
                lost += rtcpTran->calcLostCount(rtcpRecv->getTranInfoList(i));
            }
            return lost;
        }
    }
    return 0;
}

// If there are multiple srcs, this func returns the jitter
// calculation results accumulated by all srcs
int RtpSession::getJitter ()
{
    if (recv)
    {
        return (recv->getJitter() >> 4);
    }
    return 0;
}

// If there are multiple srcs, what this func returns
// is not clear - ?
int RtpSession::getLatency ()
{
    if (rtcpRecv)
    {
        return (rtcpRecv->getAvgOneWayDelay());
        //        return (rtcpRecv->getAvgRoundTripDelay());
    }

    return 0;
}

void RtpSession::setSessionState (RtpSessionState state)
{
    switch (state)
    {
        case (rtp_session_inactive):
                    if (recv) recv->getUdpStack()->setMode(inactive);
            else if (tran) tran->getUdpStack()->setMode(inactive);
        sessionState = state;
        break;
        case (rtp_session_sendonly):
                    if (recv) recv->getUdpStack()->setMode(sendonly);
            else if (tran) tran->getUdpStack()->setMode(sendonly);
        sessionState = state;
        break;
        case (rtp_session_recvonly):
                    if (recv) recv->getUdpStack()->setMode(recvonly);
            else if (tran) tran->getUdpStack()->setMode(recvonly);
        sessionState = state;
        break;
        case (rtp_session_sendrecv):
                    if (recv) recv->getUdpStack()->setMode(sendrecv);
            else if (tran) tran->getUdpStack()->setMode(sendrecv);
        sessionState = state;
        break;
        case (rtp_session_undefined):
                    sessionState = state;
        break;
        default:
        cpLog (LOG_ERR, "Unknown state: %d", (int)state);
        assert (0);
    }

    if (recv) recv->emptyNetwork();
}


RtpSessionState RtpSession::getSessionState ()
{
    return sessionState;
}

int
RtpSession::setReceiver ( int localPort, int rtcpLocalPort, int portRange,
                          RtpPayloadType apiFormat,
                          RtpPayloadType networkFormat,
                          int jitterNew /* = 5 */)
{
    if ( !(sessionState == rtp_session_sendrecv
            || sessionState == rtp_session_recvonly) )
    {
        cpLog(LOG_ERR, "wrong state of RTP stack.");
        return -1;
    }

    if (localPort != 0)
    {
        if (portRange != 0)
        {
            if (recv)
            {
                assert (recv->getUdpStack());
                recv->getUdpStack()->setLocal(localPort, localPort + portRange);
            }
            else if (tran)
            {
                assert (tran->getUdpStack());
                tran->getUdpStack()->setLocal(localPort, localPort + portRange);
                recv = new RtpReceiver(tran->getUdpStack(), apiFormat,
                                       networkFormat, jitterNew);
            }
            else
                recv = new RtpReceiver (localPort, localPort + portRange,
                                        apiFormat, networkFormat,
                                        jitterNew);
        }
        else
            if (recv)
            {
                assert (recv->getUdpStack());
                recv->getUdpStack()->setLocal(localPort, localPort + portRange);
            }
            else if (tran)
            {
                assert (tran->getUdpStack());
                tran->getUdpStack()->setLocal(localPort, localPort + portRange);
                recv = new RtpReceiver(tran->getUdpStack(), apiFormat,
                                       networkFormat, jitterNew);
            }
            else
                recv = new RtpReceiver (localPort, apiFormat,
                                        networkFormat, jitterNew);
    }

    if (rtcpLocalPort != 0)
    {
        if (portRange != 0)
            if (rtcpRecv)
            {
                assert (rtcpRecv->getUdpStack());
                rtcpRecv->getUdpStack()->setLocal(rtcpLocalPort,
                                                  rtcpLocalPort + portRange);
            }
            else if (rtcpTran)
            {
                assert (rtcpTran->getUdpStack());
                rtcpTran->getUdpStack()->setLocal(rtcpLocalPort,
                                                  rtcpLocalPort + portRange);
                rtcpRecv = new RtcpReceiver(rtcpTran->getUdpStack());
            }
            else
                rtcpRecv = new RtcpReceiver (rtcpLocalPort,
                                             rtcpLocalPort + portRange);
        else
            if (rtcpRecv)
            {
                assert(rtcpRecv->getUdpStack());
                rtcpRecv->getUdpStack()->setLocal(rtcpLocalPort);
            }
            else if (rtcpTran)
            {
                assert(rtcpTran->getUdpStack());
                rtcpTran->getUdpStack()->setLocal(rtcpLocalPort);
                rtcpRecv = new RtcpReceiver(rtcpTran->getUdpStack());
            }
            else
                rtcpRecv = new RtcpReceiver (rtcpLocalPort);
    }

    // update interlinks
    if (rtcpTran && recv) rtcpTran->setRTPrecv (recv);
    if (rtcpTran && rtcpRecv) rtcpTran->setRTCPrecv (rtcpRecv);
    if (rtcpRecv && recv) recv->setRTCPrecv(rtcpRecv);

    if (recv) cpLog (LOG_DEBUG_STACK, "RTP Recv Port: %d", recv->getPort());
    if (rtcpRecv) cpLog (LOG_DEBUG_STACK, "RTCP Recv Port: %d",
                             rtcpRecv->getPort());

    return 0;
}


int
RtpSession::setTransmiter ( const char* remoteHost, int remotePort,
                            int rtcpRemotePort,
                            RtpPayloadType apiFormat,
                            RtpPayloadType networkFormat )
{
    if ( !(sessionState == rtp_session_sendrecv
            || sessionState == rtp_session_sendonly) )
    {
        cpLog(LOG_ERR, "wrong state of RTP stack.");
        return -1;
    }

    if (remotePort != 0)
    {
        if (tran)
        {
            tran->getUdpStack()->setDestination(remoteHost, remotePort);
            NetworkAddress netAddress;
            if ( remoteHost )
            {
                netAddress.setPort(remotePort);
                netAddress.setHostName(remoteHost);
            }
            tran->setRemoteAddr(netAddress);
        }
        else
            tran = new RtpTransmitter ( remoteHost, remotePort, apiFormat,
                                        networkFormat, recv);
    }

    if (rtcpRemotePort != 0)
    {
        if (rtcpTran)
        {
            rtcpTran->getUdpStack()->setDestination( remoteHost,
                    rtcpRemotePort);
            NetworkAddress netAddress1;
            if ( remoteHost )
            {
                netAddress1.setPort(rtcpRemotePort);
                netAddress1.setHostName(remoteHost);
            }
            rtcpTran->setRemoteAddr(netAddress1);
        }
        else
            rtcpTran = new RtcpTransmitter ( remoteHost,
                                             rtcpRemotePort, rtcpRecv);
    }

    // update interlinks
    if (rtcpTran && tran) rtcpTran->setRTPtran (tran);
    if (rtcpTran && recv) rtcpTran->setRTPrecv (recv);
    if (rtcpTran && rtcpRecv) rtcpTran->setRTCPrecv (rtcpRecv);

    // SDES infromation for transmitter
    if (rtcpTran && tran)
    {
        char dummy[2]= "";

        rtcpTran->setSdesCname();
        rtcpTran->setSdesEmail(dummy);
        rtcpTran->setSdesPhone(dummy);
        rtcpTran->setSdesLoc(dummy);
        rtcpTran->setSdesTool(dummy);
        rtcpTran->setSdesNote(dummy);
    }

    if (tran) cpLog (LOG_DEBUG_STACK, "RTP Tran Port: %d",
                         tran->getUdpStack()->getDestinationPort());
    if (rtcpTran) cpLog (LOG_DEBUG_STACK, "RTCP Tran Port: %d",
                             rtcpTran->getUdpStack()->getDestinationPort());

    return 0;
}


/* --- Send and Receive RTP Functions ------------------------------ */

RtpPacket* RtpSession::createPacket (int npadSize, int csrcCount)
{
    assert (tran);
    if (npadSize != 0)
        cpDebug(LOG_DEBUG, "Nonzero npadSize not supported");
    return tran->createPacket(npadSize, csrcCount);
}



RtpSeqNumber RtpSession::getPrevSequence ()
{
    assert (tran);
    return tran->getPrevSequence ();
}



RtpTime RtpSession::getPrevRtpTime ()
{
    assert (tran);
    return tran->getPrevRtpTime();
}



void RtpSession::setMarkerOnce()
{
    if( tran )
    {
        tran->setMarkerOnce();
    }
}


int RtpSession::transmit (RtpPacket* p)
{
    if ( !( sessionState == rtp_session_sendrecv
            || sessionState == rtp_session_sendonly ) )
    {
        sessionError = session_wrongState;
        cpLog (LOG_ERR, "RTP stack can't transmit. Wrong state");
        return -1;
    }

    assert (tran);
    sessionError = session_success;
    return tran->transmit(p);
}


int RtpSession::transmitRaw (char* inbuffer, int len)
{
    if ( !( sessionState == rtp_session_sendrecv
            || sessionState == rtp_session_sendonly ) )
    {
        sessionError = session_wrongState;
        cpLog (LOG_ERR, "RTP stack can't transmit. Wrong state");
        return -1;
    }

    assert (tran);
    sessionError = session_success;
    return tran->transmitRaw(inbuffer, len);
}


int RtpSession::transmitEvent( int event )
{
    if ( !( sessionState == rtp_session_sendrecv
            || sessionState == rtp_session_sendonly ) )
    {
        sessionError = session_wrongState;
        cpLog (LOG_ERR, "RTP stack can't transmit event. Wrong state");
        return -1;
    }

    cpDebug(LOG_DEBUG_STACK,"Sending DTMF event %d in RTP stream", event );
    assert (tran);
    sessionError = session_success;
    RtpPacket* eventPacket = createPacket(0, 0);
    eventPacket->setPayloadType( rtpPayloadDTMF_RFC2833 );
    eventPacket->setPayloadUsage( sizeof( RtpEventDTMFRFC2833 ) );
    RtpEventDTMFRFC2833* eventPayload = reinterpret_cast<RtpEventDTMFRFC2833*>
                                        ( eventPacket->getPayloadLoc() );

    // reset event payload
    eventPayload->event = event; 
    eventPayload->volume = 0x0A;
    eventPayload->reserved = 0;
    eventPayload->edge = 0;
    eventPayload->duration = 0x00A0;
 //   eventPayload->duration = 0x0A00;

    // send onedge packet
    tran->transmit( eventPacket, true );

    // send on packet
    eventPayload->edge = 1;
    tran->transmit( eventPacket, true );

    /* comment by lizhijie, 2005.02.22 */
#if 0	
    // send offedge packet
    eventPacket->setPayloadUsage( 0 );
    tran->transmit( eventPacket, true );
#endif

    if( eventPacket )
        delete eventPacket; eventPacket = NULL;

    return 0;
}


RtpPacket* RtpSession::receive ()
{
    if ( !( sessionState == rtp_session_sendrecv
            || sessionState == rtp_session_recvonly ) )
    {
        if (recv) recv->receive();
        sessionError = session_wrongState;
        cpLog (LOG_ERR, "RTP stack can't receive. Wrong state");
        return NULL;
    }

    assert (recv);
    sessionError = session_success;
    return recv->receive();
}


RtpPacket* RtpSession::getPacket()
{
    if ( !( sessionState == rtp_session_sendrecv
            || sessionState == rtp_session_recvonly ) )
    {
        if (recv) recv->getPacket();
        sessionError = session_wrongState;
        cpLog (LOG_ERR, "RTP stack can't receive. Wrong state");
        return NULL;
    }

    assert (recv);
    sessionError = session_success;
    return recv->getPacket();
}



/* --- Send and Receive RTCP Functions ----------------------------- */

int RtpSession::processRTCP ()
{
    int ret=-1;
	
    if (rtcpTran)
    {
        if (checkIntervalRTCP()) transmitRTCP();
    }
    if (rtcpRecv)
    {
        ret=receiveRTCP();
    }
    return ret;
}


int RtpSession::transmitRTCP ()
{
    if ( !( sessionState == rtp_session_sendrecv
            || sessionState == rtp_session_sendonly ) )
    {
        sessionError = session_wrongState;
        cpLog (LOG_ERR, "RTCP stack can't transmit. Wrong state");
        return -1;
    }

    assert (rtcpTran);

    // generate compound packet
    RtcpPacket* p = new RtcpPacket();

    // load with report packet
    rtcpTran->addSR(p);

    // load with SDES information
    // currently only sender sends SDES, recv only receiver doesn't
    if (tran) rtcpTran->addSDES(p);

    // transmit packet
    int ret = rtcpTran->transmit(p);

    if (p) delete p;
    return ret;
}



int RtpSession::transmitRTCPBYE ()
{
    assert (rtcpTran);

    // generate compound packet
    RtcpPacket* p = new RtcpPacket();

    // load with report packet
    rtcpTran->addSR(p);

    // load with SDES CNAME
    if (tran) rtcpTran->addSDES(p, rtcpSdesCname);

    // load with BYE packet
    if (tran)
    {
        char reason[] = "Program Ended.";
        rtcpTran->addBYE(p, reason);
    }

    // transmit packet
    int ret = rtcpTran->transmit(p);

    if (p) delete p;
    return ret;
}



int RtpSession::receiveRTCP ()
{
    if ( !( sessionState == rtp_session_sendrecv
            || sessionState == rtp_session_recvonly ) )
    {
        RtcpPacket* p1 = rtcpRecv->getPacket();
        if (p1) delete p1;
        sessionError = session_wrongState;
        cpLog (LOG_ERR, "RTCP stack can't receive. Wrong state");
        return -1;
    }

    assert (rtcpRecv);

    // generate compound packet
    RtcpPacket* p = rtcpRecv->getPacket();
    if (p == NULL) return -1;

    int ret = 0;

    // read compound packet
    if (rtcpRecv->readRTCP(p) == 1)
    {
       ret = 1;
    }

    if (p) delete p;
    return ret;
}



int RtpSession::checkIntervalRTCP ()
{
    assert (rtcpTran);
    return rtcpTran->checkInterval();
}




/* --- DTMF Callback functions ------------------------------------- */
void RtpSession::setDTMFInterface( DTMFInterface* t )
{
    if ( !recv )
        cpLog(LOG_ERR, "RTP receiver not set, can't set DTMF Interface");
    else
        recv->setDTMFInterface( t );
}

void RtpSession::unsetDTMFInterface( DTMFInterface* t )
{
    if ( !recv )
        cpLog(LOG_ERR, "RTP receiver not set, can't unset DTMF Interface");
    else
        recv->unsetDTMFInterface( t );
}
