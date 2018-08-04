#ifndef SoundCardDevice_HXX
#define SoundCardDevice_HXX

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
static const char* const SoundCardDeviceVersion =
    "$Id: SoundCardDevice.hxx,v 1.2 2007/03/03 18:39:28 lizhijie Exp $";

#include "RtpSession.hxx"
#include "ResGwDevice.hxx"
#include "VMutex.h"


#ifdef WIN32
#include <Mmsystem.h>
#include "ASoundCardWin.hxx"
#endif

namespace Vocal
{
    
/**
 * The SoundCardDevice driver is, effectively, a copy of
 * SoundCardDevice.  The key bindings are identical to the
 * NullHardwareDevice.
 */
class SoundCardDevice : public ResGwDevice
{
    public:
        /** Creates a hardware object to control a single port gateway. 
         *  @param deviceName
         *  @param inputQ
         *  @param outputQ
         */
        SoundCardDevice( const char* deviceName,
                         Sptr < Fifo < Sptr<SipProxyEvent> > > inputQ,
                         Sptr < Fifo < Sptr<SipProxyEvent> > > outputQ );

        /// Destructor
        ~SoundCardDevice();

        /** Starts the main processing loop of the hardware
         *  @param parms
         */
        void* hardwareMain (void* parms);

        /** Add device's socket to the fd (File Descriptor)
         *  @param fd File Descriptor
         *  @return 0 if successful, errorcode otherwise
         */
        int addToFdSet (fd_set* fd);

        /** Process all pending hardware events
         * @param fd File Descriptor
         * @return 0 if successful, errorcode otherwise
         */
        int process (fd_set* fd);

        /** Start audio channel on device
         * @param request
    	 * @return 0 if successful, errorcode otherwise
	     */
        int audioStart (const HardwareAudioRequest& request);

        /** Stops audio channel on device
       	 * @return 0 if successful, errorcode otherwise
    	 */
        int audioStop ();

        /** Suspend audio channel on device
    	 * @return 0 if successful, errorcode otherwise
    	 */
        int audioSuspend ();

        /** Resume audio channel on device
         * @param request
         * @return 0 if successful, errorcode otherwise
         */
        int audioResume (const HardwareAudioRequest& request);

        /** process RTP audio packets
    	 */
        virtual void processRTP ();

        /** Instantiates a rtp session and reserves a rtp port
         * @return reserved RTP port
      	 */
        virtual int getRtpPort();

        /** Release the port and deallocate the rtp session
         */
        virtual void releaseRtpPort();

    private:
        ///
        int getRingbackTone(unsigned char *, const int size);

        /// added to allow for 'deviceEventFlash'
        void onhookOrFlash();

        ///
        void startSendRingback() { sendRingback = true; };
        ///
        void stopSendRingback() { sendRingback = false; };

        ///
        void provideDialToneStart();
        ///
        void provideDialToneStop();

        ///
        void provideRingStart();
        ///
        void provideRingStop();

        ///
        void provideLocalRingbackStart();
        ///
        void provideLocalRingbackStop();

        ///
        void provideBusyToneStart();
        ///
        void provideBusyToneStop();

        ///
        void provideFastBusyToneStart();
        ///
        void provideFastBusyToneStop();

        ///
        void provideCallInfo(string, string, string);

        ///
        void provideDtmf(DeviceSignalType);

#if 0
        ///
        void killDialTone();
#endif

        ///
        void provideCallWaitingBeepStart();
        ///
        void provideCallWaitingBeepStop();

        /// provide simulated tones by playing corresponding audio file
        void provideTone ( const ToneEmulation tone );

        // output queue to notify endpoint of occurence of hardware events
        Sptr < Fifo < Sptr<SipProxyEvent> > > sessionQ;

        bool audioActive;     // true if RTP audio packets are being processed
        bool audioHalfActive; // true if only sending RTP audio packets
        bool playDialTone;    // turn off tone after pressing a digit

        // RTP session variables
        RtpSession* audioStack;
        RtpPacket* inRtpPkt;
        unsigned char dataBuffer[480];
        signed short longdataBuffer[480];

        // variables for local ringback tone
        int ringbackFd;
        bool sendRingback;

        const char* myDeviceName;

        // this is used to lock the RTP thread and the hardware thread so that
        // they don't access the hardware device simultaneously
        VMutex deviceMutex;

        // audio hardware opening and closing
        //   return -1 in failures
        int openAudioHardware( const char* deviceName );
        int closeAudioHardware();
        int reopenAudioHardware();

        void writeToSoundCard( const unsigned char* dataBuffer, const int size);
        int readFromSoundCard( unsigned char* dataBuffer, const int size);

        // CODEC supported by card
        RtpPayloadType apiCodec;
        
#ifdef WIN32
	//AND:Win32 specific members (handles, etc...)
	ASoundCardWin	m_aSoundCardWinIn;
	ASoundCardWin	m_aSoundCardWinOut;
#endif

        void testSoundFile();
};
 
}
#endif

