#ifndef QuickNetDevice_HXX
#define QuickNetDevice_HXX

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

static const char* const QuickNetDeviceVersion =
    "$Id: QuickNetDevice.hxx,v 1.1.1.1 2006/11/29 09:50:50 lizhijie Exp $";




#include "RtpSession.hxx"
#include "ResGwDevice.hxx"
#include "VMutex.h"

/**********************************************************************
**********************************************************************/

namespace Vocal
{

enum IPJackTone
{
    IPJackDialTone,
    IPJackRingbackTone,
};

class QuickNetDevice : public ResGwDevice
{
    public:
        /** Creates a hardware object to control a single port gateway.
         */
        QuickNetDevice( const char* deviceName,
                        Sptr < Fifo < Sptr < SipProxyEvent > > > inputQ,
                        Sptr < Fifo < Sptr < SipProxyEvent > > > outputQ );

        /** Destructor
         */
        ~QuickNetDevice();

        /** Starts the main processing loop of the hardware
         */
        void* hardwareMain (void* parms);

        /** add device's socket to the fd
            returns 0 if successful, errorcode otherwise
         */
        int addToFdSet (fd_set* fd);

        /** process all pending stdin event for URL dialing
         */
        void processUrlInput ();

        /** process all pending hardware events; returns 0 if successful,
         ** otherwise returns an errorcode.  
         */
        int process (fd_set* fd);

        /// start audio channel on device
        /// returns 0 if successful, errorcode otherwise
        int audioStart (const HardwareAudioRequest& request);

        /// stops audio channel on device
        /// returns 0 if successful, errorcode otherwise
        int audioStop ();

        /// suspend audio channel on device
        /// returns 0 if successful, errorcode otherwise
        int audioSuspend ();

        /// resume audio channel on device
        /// returns 0 if successful, errorcode otherwise
        int audioResume (const HardwareAudioRequest& request);

        /// process RTP audio packets
        virtual void processRTP ();

        /// instantiates a rtp session and reserves a rtp port
        virtual int getRtpPort();
        /// release the port and deallocate the rtp session
        virtual void releaseRtpPort();

    private:
        // ****************
        // ****************

        /**
         */
        int getRingbackTone(char* buf, int size);

        /** added to allow for 'deviceEventFlash'
         */
        void
        onhookOrFlash();

        /**
         */
        char* getCwBeep();

        /**
         */
        void startSendRingback()
        {
            sendRingback = true;
        };
        /**
         */
        void stopSendRingback()
        {
            sendRingback = false;
        };

        /**
         */
        void provideDialToneStart();

        /**
         */
        void provideDialToneStop();

        /**
         */
        void provideRingStart();

        /**
         */
        void provideRingStop();

        /**
         */
        void provideLocalRingbackStart();

        /**
         */
        void provideLocalRingbackStop();

        /**
         */
        void provideBusyToneStart();

        /**
         */
        void provideBusyToneStop();

        /**
         */
        void provideFastBusyToneStart();

        /**
         */
        void provideFastBusyToneStop();

        /**
         */
        void provideCallWaitingBeepStart();

        /**
         */
        void provideCallWaitingBeepStop();

        /**
        */
        void provideCallInfo(string, string, string);

        /**
        */
        void provideDtmf(DeviceSignalType);

        // ****************
        // ****************

        //// provide simulated tones by playing corresponding audio file
        void provideTone (IPJackTone tone);


        // output queue to notify endpoint of occurence of hardware events
        Sptr < Fifo < Sptr< SipProxyEvent > > > sessionQ;

        // RTP session variables
        RtpSession* audioStack;
        RtpPacket* inRtpPkt;
        char outBufferPkt[480];

        //variables for remote ringback tone
        int ringbackFd;
        bool sendRingback;

        // quicknet card device name
        const char* myDeviceName;

        // call waiting beep related variables
        // call waiting beep is on for 0.3s and off 10s
        char* cwBeep;
        bool cwCadenceOn;
        int cwCnt;
        bool cwBeepOn;
        static const int CWBEEP_ON_TIME = 2400;   // 0.3 * 8000
        static const int CWBEEP_OFF_TIME = 80000;  // 10 * 8000

        // this is used to lock the RTP thread and the hardware thread so that
        // they don't access the hardware device simultaneously
        VMutex deviceMutex;

};
 
}

#endif
