/*
* $Log: SessionData.cxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:09  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: SessionData.cxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include "SessionData.hxx"


SessionData::SessionData()
        : stack(0),
        playFileFd( -1),
        recFileFd( -1),
        state(0),
        pktSize(40)
{}



void
SessionData::receivePacket()
{
    RtpPacket* packet = 0;
    if (stack)
    {
        cout << "-";
        packet = stack->receive();
    }
    if (packet)
    {
        if (recFileFd >= 0)
        {
            cout << "x";
            if (packet->getPayloadUsage() != (8 * pktSize))
            {
                cout << "not the right size!" << endl;
            }
            write (recFileFd,
                   packet->getPayloadLoc(), packet->getPayloadUsage());
        }
        delete packet;
        packet = 0;
    }
    else
    {
        cout << "no packet!" << endl;
    }
}

void
SessionData::transmitPacket()
{
    RtpPacket* packet = 0;

    if (stack)
    {
//        packet = stack->createPacket(pktSize * 8);
        packet = stack->createPacket();

        if (playFileFd >= 0)
        {
            int bytecount =
                read(playFileFd,
                     packet->getPayloadLoc(), packet->getPayloadSize());
            if (bytecount)
            {
                packet->setPayloadUsage (bytecount);
                //      outRtpPkt->setRtpTime(audioStack->getPrevRtpTime() + NETWORK_RTP_RATE);
                stack->transmit (packet);
            }
            else
            {
                // this file is now empty -- close and quit
                close(playFileFd);
                playFileFd = -1;
                delete packet;
                packet = 0;
                if (state == 1)
                {
                    // i'm done!
                    state = 2;
                }
            }
        }
    }
}


void SessionData::newStack(
    const string& localPort,
    const string& remoteHost,
    const string& remotePort )
{
    int localPortNumber = atoi(localPort.c_str());
    int remotePortNumber = atoi(remotePort.c_str());

    if ((localPortNumber < 1024) || (remotePortNumber < 1024))
    {
        return ;
    }

    closeStack();

    stack = new RtpSession(remoteHost.c_str(), remotePortNumber,
                           localPortNumber, remotePortNumber + 1,
                           localPortNumber + 1, rtpPayloadPCMU,
                           rtpPayloadPCMU, 5);

    stack->setApiPktSize (pktSize * 8);
    stack->setNetworkPktSize (pktSize * 8);

    // set SDES information - optional
    RtcpTransmitter* rtcpTran = stack->getRtcpTran();
    rtcpTran->setSdesEmail("development@vovida.com");

    state = 0;
}


void
SessionData::addQueue(const string& str)
{
    queueList.push_back(str);
}


void
SessionData::shouldUseQueue(bool val)
{
    if (val)
    {
        if (state == 0)
        {
            state = 2;
        }
    }
    else
    {
        state = 0;
    }
}

void
SessionData::closeStack()
{
    close(playFileFd);
    close(recFileFd);
    playFileFd = -1;
    recFileFd = -1;
    if (stack)
    {
        stack->transmitRTCPBYE();
        delete stack;
    }
    state = 0;
    stack = 0;
}


void
SessionData::setPktSize(int size)
{
    pktSize = size;
}

void
SessionData::setPlayFile(const string& filename)
{
    cout << "playing file: " << filename << endl;

    if (playFileFd >= 0)
    {
        close(playFileFd);
    }

    // do something about this
    int fd = open(filename.c_str(), O_RDONLY);
    if (fd < 0)
    {
        cout << "error opening file for reading: " << filename << endl;
        cout << "reason: " << strerror(errno) << endl;
        // this is an error
    }

    playFileFd = fd;
}


void SessionData::stopRecord()
{
    // stop recording
    close(recFileFd);
    recFileFd = -1;
}

void SessionData::setRecordFile(const string& filename)
{
    cout << "record file: " << filename << endl;

    if (recFileFd >= 0)
    {
        close(recFileFd);
    }

    // do something about this
    int fd = open(filename.c_str(), O_WRONLY | O_CREAT | O_TRUNC,
                  S_IRUSR | S_IWUSR);
    if (fd < 0)
    {
        cout << "error opening file for writing!" << endl;
        cout << "reason: " << strerror(errno) << endl;
        // this is an error
    }

    recFileFd = fd;
}


void
SessionData::handleQueue()
{
    // do something about the queue items

    if (state == 2)
    {
        if (queueList.size())
        {
            string cmd = (queueList.front());
            queueList.pop_front();
            state = 1;
            // now, figure out what this means
            if (cmd.length() >= 2)
            {
                if (cmd[0] == 'p')
                {
                    // set the play buffer
                    setPlayFile(cmd.substr(2, cmd.length() - 2));
                }
                if (cmd[0] == 'r')
                {
                    // set the record buffer
                    setRecordFile(cmd.substr(2, cmd.length() - 2));
                }
            }
        }
        else
        {
            // all done!
            state = 0;
        }
    }
}


int SessionData::getFd()
{
    if (stack)
    {
        return stack->getRtpRecv()->getSocketFD();
    }
    else
    {
        return -1;
    }
}

void SessionData::periodicProcess()
{
    VTime currentTime;
    int deltaMs =
        currentTime - lastTimePktSent;
    if (deltaMs > (pktSize - 2))
    {
        // send a packet
        transmitPacket();
        lastTimePktSent = lastTimePktSent + pktSize;
    }

    if (state == 2)
    {
        handleQueue();
    }

    if (stack)
    {
        stack->processRTCP();
    }
}
