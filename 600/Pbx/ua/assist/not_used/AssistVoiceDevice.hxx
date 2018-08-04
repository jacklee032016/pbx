#ifndef __ASSIST_SLIC_DEVICE_H__
#define __ASSIST_SLIC_DEVICE_H__

/*
 * $Log: AssistVoiceDevice.hxx,v $
 * Revision 1.1.1.1  2006/11/29 09:50:50  lizhijie
 * AS600 Pbx
 *
 * Revision 1.1.1.1  2006/07/12 16:16:48  lizhijie
 * rebuild 
 *
 * Revision 1.1.1.1  2005/03/14 07:38:05  lizhijie
 * PBX program
 *
 * Revision 1.1  2005/02/23 05:18:12  lizhijie
 * no message
 *
 * Revision 1.1.1.1  2005/02/16 04:51:10  lizhijie
 * new pbx code
 *
 * Revision 1.1.1.1  2005/02/05 05:30:17  lizhijie
 * pbx from UA of vocal
 *
 */

static const char* const QuickNetDeviceVersion =
    "$Id: AssistVoiceDevice.hxx,v 1.1.1.1 2006/11/29 09:50:50 lizhijie Exp $";


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

class AssistVoiceDevice : public ResGwDevice
{
    public:
        /** Creates a hardware object to control a single port gateway.
         */
        AssistVoiceDevice( const char* deviceName,
                        Sptr < Fifo < Sptr < SipProxyEvent > > > inputQ,
                        Sptr < Fifo < Sptr < SipProxyEvent > > > outputQ );

        ~AssistVoiceDevice();


        void* hardwareMain (void* parms);

        /** add device's socket to the fd , returns 0 if successful, errorcode otherwise */
        int addToFdSet (fd_set* fd);

#if 0
        /** process all pending stdin event for URL dialing    */
        void processUrlInput ();
#endif
        /** process all pending hardware events; returns 0 if successful,otherwise returns an errorcode.  */
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
/*********************   *************/

        /**          */
        int getRingbackTone(char* buf, int size);

        /** added to allow for 'deviceEventFlash'   */
        void onhookOrFlash();

        /**         */
        char* getCwBeep();

        /**      */
        void startSendRingback()
        {
            sendRingback = true;
        };
		
        /**    */
        void stopSendRingback()
        {
            sendRingback = false;
        };

        /**        */
        void provideDialToneStart();

        /**        */
        void provideDialToneStop();

        /**      */
        void provideRingStart();

        /**      */
        void provideRingStop();

        /**      */
        void provideLocalRingbackStart();

        /**  */
        void provideLocalRingbackStop();

        /**   */
        void provideBusyToneStart();

        /**          */
        void provideBusyToneStop();

        /**         */
        void provideFastBusyToneStart();

        /**        */
        void provideFastBusyToneStop();

        /**      */
        void provideCallWaitingBeepStart();

        /**       */
        void provideCallWaitingBeepStop();

        /**        */
        void provideCallInfo(string, string, string);

        /**        */
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
