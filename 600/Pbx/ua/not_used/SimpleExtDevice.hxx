#ifndef SIMPLE_EXT_DEVICE_HXX
#define SIMPLE_EXT_DEVICE_HXX

/* ====================================================================
 * The Vovida Software License, Version 1.0 
 * 
 * Copyright (c) 2000 Vovida Networks, Inc.  All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 
 * 3. The names "VOCAL", "Vovida Open Communication Application Library",
 *    and "Vovida Open Communication Application Library (VOCAL)" must
 *    not be used to endorse or promote products derived from this
 *    software without prior written permission. For written
 *    permission, please contact vocal@vovida.org.
 *
 * 4. Products derived from this software may not be called "VOCAL", nor
 *    may "VOCAL" appear in their name, without prior written
 *    permission of Vovida Networks, Inc.
 * 
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESSED OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, TITLE AND
 * NON-INFRINGEMENT ARE DISCLAIMED.  IN NO EVENT SHALL VOVIDA
 * NETWORKS, INC. OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT DAMAGES
 * IN EXCESS OF $1,000, NOR FOR ANY INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 * 
 * ====================================================================
 * 
 * This software consists of voluntary contributions made by Vovida
 * Networks, Inc. and many individuals on behalf of Vovida Networks,
 * Inc.  For more information on Vovida Networks, Inc., please see
 * <http://www.vovida.org/>.
 *
 */
static const char* const SimpleExtDevice_hxx_Version =
    "$Id: SimpleExtDevice.hxx,v 1.2 2007/03/03 18:39:28 lizhijie Exp $";

#include "RtpSession.hxx"
#include "ResGwDevice.hxx"
#include "VMutex.h"
#include "PlayQueue.h"
#include "Recorder.h"
#include "VTime.hxx"

namespace Vocal
{
    
/**
 * This class is used as a Sip Talking front end for voice mail server 
 * It talks Voice Mail Control Protocol with voicemail server.
 * It is used to control audio signals, it also handles the DTMF digits.
 */
class SimpleExtDevice : public AssistDevice
{
    public:
        /// Creates a hardware object to control a single port gateway.
        SimpleExtDevice(char* hostName, int port,
                   Sptr < Fifo < Sptr < SipProxyEvent > > > inputQ,
                   Sptr < Fifo < Sptr < SipProxyEvent > > > outputQ );

        /// Destructor
        ~SimpleExtDevice(void);

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
        int audioStart (const HardwareAudioRequest& request, int deviceId);

        /// stops audio channel on device
        /// returns 0 if successful, errorcode otherwise
        int audioStop (int deviceId);

        // process RTP audio packets
	void processRTP (); 

        /// instantiates a rtp session and reserves a rtp port
	int getRtpPort(); 

        /// release the port and deallocate the rtp session
 	void releaseRtpPort();

        /// DTMF handling callback function
        void recvRTPDTMF ( int digit, int duration );

    private:
        void provideDialToneStart(int deviceId);
        void provideDialToneStop(int deviceId);
        void provideLocalRingbackStart(int deviceId);
        void provideLocalRingbackStop(int deviceId);
        void provideBusyToneStart(int deviceId);
        void provideBusyToneStop(int deviceId);
        void provideFastBusyToneStart(int deviceId);
        void provideFastBusyToneStop(int deviceId);
        void provideCallWaitingBeepStart(int deviceId);
        void provideCallWaitingBeepStop(int deviceId);
        void provideRingStart(int deviceId);
        void provideRingStop(int deviceId);
        void provideDtmf(DeviceSignalType,int deviceId);
        void provideCallInfo(string, string, string);

        int audioSuspend(int deviceId)
        {
            return 0;
        }
        int audioResume(const HardwareAudioRequest &request,int deviceId)
        {
            return 0;
        }

	/* added by lizhijie 2005.02.11 */
		void setCallId( const Sptr<SipCallId> newCallId , int deviceId);
		Sptr<SipCallId> getCallId( int deviceId) const;
		int getDeviceId( const SipCallId& callId ) ;
		Sptr<DigitCollector> getDigitCollector(int deviceId) ;

		virtual int getRtpPort(int deviceId);
		virtual void releaseRtpPort(int deviceId);
		
		virtual HardwareStatusType getHardwareStatus(int deviceId);
		virtual bool isMyHardware(const SipCallId& callid, int deviceId);
	/* end of added */

        // output queue to notify endpoint of occurence of hardware events
        Sptr < Fifo < Sptr < SipProxyEvent > > > sessionQ;
        bool hasPlayed; 


        // VoiceMail Control Protocol stack
//        Vmcp vm;

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
        VTime nextTime;

        int networkPktSize;


		Sptr<SipCallId> callId;
};
 
}

#endif

