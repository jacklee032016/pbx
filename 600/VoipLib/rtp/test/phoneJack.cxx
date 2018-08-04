/*
* $Id: phoneJack.cxx,v 1.2 2007/03/02 07:29:48 lizhijie Exp $
*/
#include <iostream.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/time.h>


#include <linux/ixjuser.h>
#include "RtpSession.hxx"
#include "cpLog.h"

#define ULAW_PAYLOAD 240
#define RESID_RTP_RATE 240
#define NETWORK_RTP_RATE 160

static int deviceFD;
RtpSession* audioStack;
RtpPacket* outRtpPkt;
RtpPacket* inRtpPkt;

int audioStart (char* rmtHost, int rmtPort, int lclPort)
{
    cerr << "%%%   Starting audio   %%%\n";

    deviceFD = open("/dev/phone0", O_RDWR);

    audioStack = new RtpSession(rmtHost, rmtPort, lclPort, rmtPort + 1,
                                lclPort + 1, rtpPayloadPCMU, rtpPayloadPCMU, 5);
    assert(audioStack);

    outRtpPkt = audioStack->createPacket();
    assert (outRtpPkt);
    inRtpPkt = NULL;

    audioStack->setApiPktSize (RESID_RTP_RATE);
    audioStack->setNetworkPktSize (NETWORK_RTP_RATE);

    ioctl(deviceFD, IXJCTL_PLAY_CODEC, ULAW);
    ioctl(deviceFD, IXJCTL_REC_CODEC, ULAW);

    ioctl(deviceFD, IXJCTL_PLAY_START);
    ioctl(deviceFD, IXJCTL_REC_START);

    ioctl(deviceFD, IXJCTL_AEC_START, 1);
    cpLog (LOG_DEBUG, "echoCancellation is on");

    ioctl(deviceFD, IXJCTL_MIXER, 0x0C03);   // 06 is too soft
    ioctl(deviceFD, IXJCTL_MIXER, 0x0F01);   // 02 is too soft

    return 0;
}


int audioStop ()
{
    cerr << "%%%   Stopping audio   %%%\n";

    // make hardware calls to stop audio
    ioctl(deviceFD, IXJCTL_REC_STOP);
    ioctl(deviceFD, IXJCTL_PLAY_STOP);
    ioctl(deviceFD, IXJCTL_AEC_STOP);

    close(deviceFD);
    deviceFD = open("/dev/phone0", O_RDWR);
    if (deviceFD < 0)
    {
        cerr << "Cannot reopen /dev/phone0" << endl;
        exit(1);
    }

    // close RTP session
    audioStack->transmitRTCPBYE();

    delete audioStack; audioStack = NULL;

    delete outRtpPkt; outRtpPkt = NULL;
    if (inRtpPkt)
    {
        delete inRtpPkt;
        inRtpPkt = NULL;
    }

    return 0;

}

int main (int argc, char* argv[])
{
    audioStart(argv[1], atoi(argv[2]), atoi(argv[3]));
    usleep(1000);

    cpLogSetLabel ("sampleUsage");
    cpLogSetPriority (LOG_DEBUG);


    int rtpFD = (audioStack->getRtpRecv())->getSocketFD();
    int rtcpFD = (audioStack->getRtcpRecv())->getSocketFD();
    int maxFD = 0;

    if ( (rtpFD < 0) || (rtcpFD < 0) )
        printf("failure on RTP socket\n");

    fd_set rfds;
    struct timeval tv;

    tv.tv_sec = 0;
    tv.tv_usec = 300;

    if ( maxFD < deviceFD ) maxFD = deviceFD;
    if ( maxFD < rtpFD) maxFD = rtpFD;
    if ( maxFD < rtcpFD) maxFD = rtcpFD;
    maxFD++;

    int hook, retval, cc;

    printf("Waiting for hookswitch\n");
    hook = ioctl(deviceFD, IXJCTL_HOOKSTATE);
    while (hook != 1)
    {
        hook = ioctl(deviceFD, IXJCTL_HOOKSTATE);
    }

    //    while ( (hook = (ioctl(deviceFD, IXJCTL_HOOKSTATE))) )
    while (1)
    {
        FD_ZERO(&rfds);
        FD_SET(rtpFD, &rfds);
        FD_SET(rtcpFD, &rfds);
        FD_SET(deviceFD, &rfds);
        tv.tv_sec = 0;
        tv.tv_usec = 300;

        retval = select(maxFD, &rfds, NULL, NULL, &tv);

        if (retval >= 0)
        {
            usleep(0);
            if ( hook != (ioctl(deviceFD, IXJCTL_HOOKSTATE)) )
            {
                break;
            }

            //        if (FD_ISSET(rtpFD, &rfds))                // read data from network
            //        {
            inRtpPkt = audioStack->receive();
            if (inRtpPkt)
            {
                //                cerr << "r";
                //                assert (inRtpPkt->getPayloadUsage() == RESID_RTP_RATE);
                write (deviceFD, inRtpPkt->getPayloadLoc(),
                       inRtpPkt->getPayloadUsage());

                delete inRtpPkt;
                inRtpPkt = NULL;
            }
            else
            {
                //                cerr << "n";
            }
            //        }

            //        if (FD_ISSET(deviceFD, &rfds))        // read data from phone
            //        {

            cc = read (deviceFD, outRtpPkt->getPayloadLoc(), RESID_RTP_RATE);
            if (cc > 0)
            {
                //                assert (cc == RESID_RTP_RATE);
                outRtpPkt->setPayloadUsage(cc);
                //                outRtpPkt->setRtpTime(dataStack.getPrevRtpTime() + 240);
                audioStack->transmit(outRtpPkt);
                //                cerr << "t";
            }

            else
            {
                //                cerr << "m";
            }
            //        }


            audioStack->processRTCP();
        } /* if (retval >=0) */

    } /* end while() */

    audioStop();

}
