#ifndef VmcpDevice_HXX
#define VmcpDevice_HXX

/*
 * $Id: VmcpDevice.hxx,v 1.3 2007/03/03 18:39:28 lizhijie Exp $
 */

#include "RtpSession.hxx"
#include "ResGwDevice.hxx"
#include "VMutex.h"
#include "Vmcp.h"
#include "PlayQueue.h"
#include "Recorder.h"
#include "VmTime.h"

namespace Vocal
{
    

/**
 * This class is used as a Sip Talking front end for voice mail server 
 * It talks Voice Mail Control Protocol with voicemail server.
 * It is used to control audio signals, it also handles the DTMF digits.
 */
class VmcpDevice: public ResGwDevice
{
    public:
        /// Creates a hardware object to control a single port gateway.
        VmcpDevice(char* hostName, int port,
                   Sptr < Fifo < Sptr < SipProxyEvent > > > inputQ,
                   Sptr < Fifo < Sptr < SipProxyEvent > > > outputQ );

        /// Destructor
        ~VmcpDevice(void);

        /// Starts the main processing loop of the hardware
        void* hardwareMain (void* parms);

        /// add device's socket to the fd
        /// returns 0 if successful, errorcode otherwise
        int addToFdSet (fd_set* fd);

        /// process all pending hardware events; returns 0 if successful,
        /// otherwise returns an errorcode.
        int process (fd_set* fd);

        /// start audio channel on device
        /// returns 0 if successful, errorcode otherwise
        int audioStart (const HardwareAudioRequest& request);

        /// stops audio channel on device
        /// returns 0 if successful, errorcode otherwise
        int audioStop (void);

        // process RTP audio packets
	void processRTP (); 

        /// instantiates a rtp session and reserves a rtp port
	int getRtpPort(); 

        /// release the port and deallocate the rtp session
 	void releaseRtpPort();

        /// DTMF handling callback function
        void recvRTPDTMF ( int digit, int duration );

    private:
        void provideDialToneStart();
        void provideDialToneStop();
        void provideLocalRingbackStart();
        void provideLocalRingbackStop();
        void provideBusyToneStart();
        void provideBusyToneStop();
        void provideFastBusyToneStart();
        void provideFastBusyToneStop();
        void provideCallWaitingBeepStart();
        void provideCallWaitingBeepStop();
        void provideRingStart();
        void provideRingStop();
        void provideDtmf(DeviceSignalType);
        void provideCallInfo(string, string, string);

        int audioSuspend()
        {
            return 0;
        }
        int audioResume(const HardwareAudioRequest &request)
        {
            return 0;
        }


        // output queue to notify endpoint of occurence of hardware events
        Sptr < Fifo < Sptr < SipProxyEvent > > > sessionQ;
        bool hasPlayed; 


        // VoiceMail Control Protocol stack
        Vmcp vm;

        char* myHostName;
        int myPortNumber;

        bool serverAvailable;

        struct timeval prevRtpPacketTime;

        // this is used to lock the RTP thread and the hardware thread so that
        // they don't access the hardware device simultaneously
        VMutex deviceMutex;

        string CallerId;
        string CalleeId;
        string ForwardReason;
        int NumberOfForwards;
        int fwdFlag;

        // Vmcp socket
	int ss; 	

        // RTP Stack
        RtpSession* audioStack;
        RtpPacket* inRtpPkt;
        RtpPacket* outRtpPkt;
        int packetCount;
        DTMFInterface* _DTMFInterface;

        // Player and recorder
        PlayQueue player;
        Recorder recorder;
        VmTime nextTime;

        int networkPktSize;
};
 
}

#endif

