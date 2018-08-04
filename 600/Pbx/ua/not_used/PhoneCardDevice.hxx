#ifndef PhoneCardDevice_HXX
#define PhoneCardDevice_HXX

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

static const char* const PhoneCardDeviceVersion =
    "$Id: PhoneCardDevice.hxx,v 1.1.1.1 2006/11/29 09:50:50 lizhijie Exp $";



#include "RtpSession.hxx"
#include "ResGwDevice.hxx"
#include "VMutex.h"
#include <termios.h>

//**********************************************************************
// The PhoneCardDevice driver is, effectively, a merge of the
// QuickNetDevice and the NullHardwareDevice.  This was done to provide
// dialing / onhook / offhook / flash  functionality to the pcmcia
// Quicknet PhoneCARD which currently does not support plugging in an
// analog phone.  The key bindings are identical to the
// NullHardwareDevice.
//**********************************************************************

namespace Vocal
{

enum PhoneCardTone
{
    PhoneCardDialTone,
    PhoneCardRingbackTone,
};

/**
 * Initializes a QuickNet PhoneCARD and handles hardware control
 * @author zolli
 * @see QuickNetDevice
 * @see NullHwDevice
 * @see ResGwDevice
 */

class PhoneCardDevice : public ResGwDevice
{
public:
    /** Creates a hardware object to control a single port gateway. 
     * @param deviceName name of PhoneCARD linux system device
     * @param inputQ input queue to the hardware
     * @param outputQ output queue to the hardware
     */
    PhoneCardDevice( const char* deviceName,
		     Sptr < Fifo < Sptr < SipProxyEvent > > > inputQ,
		     Sptr < Fifo < Sptr < SipProxyEvent > > > outputQ );
    
    /** Destructor
     */
    ~PhoneCardDevice();
    
    /** Starts the main processing loop of the hardware
     * @param parms
     */
    void* hardwareMain (void* parms);
    
    /** Add device's socket to the fd (File Descriptor)
     * @param fd File Descriptor
     * @return 0 if successful, error code otherwise
     */
    int addToFdSet (fd_set* fd);
    
    /** Process all pending hardware events
     * @param fd File Descriptor
     * @return 0 if successful, error code otherwise
     */
    int process (fd_set* fd);
    
    /** Start audio channel on device
     * @param request
     * @return 0 if successful, errorcode otherwise
     */
    int audioStart (const HardwareAudioRequest& request);
    
    /** Stops audio channel on device
     * @return 0 if successful, error code otherwise
     */
    int audioStop ();
    
    /** Suspend audio channel on device
     * @return 0 if successful, error code otherwise
     */
    int audioSuspend ();
    
    /** Resume audio channel on device
     * @param request
     * @return 0 if successful, error code otherwise
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
    // ****************
    // ****************
    
    /** ???
     */
    int getRingbackTone(char *, int size);
    
    /** Used to determine when an onhook event becomes a flash event
     * @see deviceEventFlash
     */
    void onhookOrFlash();
    
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
    
    /** Currently used only in VmcpDevice
     */
    void provideCallInfo(string, string, string);
    
    /** Prints dtmf digit or symbol to stdout
     */
    void provideDtmf(DeviceSignalType);
    
    /** Sends ioctl to hardware to stop dialtone
     */
    void killTone();
    
    /**
     */
    void provideCallWaitingBeepStart();
    
    /**
     */
    void provideCallWaitingBeepStop();
    
    
    // ****************
    // ****************
    
    /// Provide simulated tones by playing corresponding audio file
    void provideTone( PhoneCardTone tone );
    
    /// Output queue to notify endpoint of occurence of hardware events
    Sptr < Fifo < Sptr < SipProxyEvent > > > sessionQ;
    
    /// true if RTP audio packets are being processed
    bool audioActive;

    /// true if only sending RTP audio packets
    bool audioHalfActive;

    /// Used to turn off dialtone after pressing a digit
    bool playDialTone;
    
    /// RTP session variables
    RtpSession* audioStack;
    RtpPacket* inRtpPkt;
    char outBufferPkt[480];
    
    /// variables for remote ringback tone
    int ringbackFd;
    bool sendRingback;

    const char* myDeviceName;
    
    /** This is used to lock the RTP thread and the hardware thread so that
     * they don't access the hardware device simultaneously.
     */
    VMutex deviceMutex;
    
    /// needed for proper terminal control
    int stdinFD;
    struct termios newTerm;
    
};
 
}

#endif
