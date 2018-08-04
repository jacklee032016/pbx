#ifndef __ASSIST_NULL_DEVICE_H__
#define __ASSIST_NULL_DEVICE_H__

/*
 *$Id: AssistNullDevice.hxx,v 1.1.1.1 2006/11/29 09:50:50 lizhijie Exp $
 */

#include "RtpSession.hxx"
#include "ResGwDevice.hxx"
#include "VMutex.h"

#include "AssistDeviceInfo.hxx"
#include "AssistPbxDevice.hxx"

#include "pbx_globals.h"
#include "aslib.h"
#include <map>
#include <string> 
using namespace std;

/**********************************************************************
**********************************************************************/

namespace Vocal
{

typedef map<int, int> DeviceInfoPos;
class AssistNullDevice : public ResGwDevice
{

#if 0 //modified by lijie 2005-06-03
	public:
        /** Creates a hardware object to control a single port gateway.
         */
        AssistNullDevice( const char* deviceName, as_span_t *span,
                        Sptr < Fifo < Sptr < SipProxyEvent > > > inputQ,
                        Sptr < Fifo < Sptr < SipProxyEvent > > > outputQ , assist_app_type apptype );

        ~AssistNullDevice();


        void* hardwareMain (void* parms);

        /** add device's socket to the fd , returns 0 if successful, errorcode otherwise */
        int addToFdSet (fd_set* fd);

        /** process all pending hardware events; returns 0 if successful,otherwise returns an errorcode.  */
        int process (fd_set* fd);

        /// start audio channel on device
        /// returns 0 if successful, errorcode otherwise
        int audioStart (const HardwareAudioRequest& request,int deviceId);

        /// stops audio channel on device
        /// returns 0 if successful, errorcode otherwise
        int audioStop (int deviceId);

        /// suspend audio channel on device
        /// returns 0 if successful, errorcode otherwise
        int audioSuspend (int deviceId);

        /// resume audio channel on device
        /// returns 0 if successful, errorcode otherwise
        int audioResume (const HardwareAudioRequest& request,int deviceId);

        /// process RTP audio packets
		virtual void processRTP ();
		virtual void processPbxRTP();

		virtual int getRtpPort(int deviceId);
		virtual void releaseRtpPort(int deviceId);
		
		virtual HardwareStatusType getHardwareStatus(int deviceId);
		virtual bool isMyHardware(const SipCallId& callid, int deviceId);
   private:
/*********************   *************/
//        pbx_stats  getPbxStatus();

        /**          */
        int getRingbackTone(char* buf, int size);

        /** added to allow for 'deviceEventFlash'   */
        void onhookOrFlash(as_device_t *dev);



		void  *slicHardwareMain(void *param);
		void  *pcmHardwareMain(void *param);
		

		//added by lijie to query device status
		void provideQueryStatus(int deviceId);
		void provideDialToneStart(int deviceId);
		void provideDialToneStop(int deviceId);
		void provideRingStart(int deviceId);
		void provideRingStop(int deviceId) ;
		void provideLocalRingbackStart(int deviceId) ;
		void provideLocalRingbackStop(int deviceId) ;
		void provideBusyToneStart(int deviceId);
		void provideBusyToneStop(int deviceId) ;
		void provideFastBusyToneStart(int deviceId);
		void provideFastBusyToneStop(int deviceId) ;
		void provideCallWaitingBeepStart(int deviceId);
		void provideCallWaitingBeepStop(int deviceId) ;

		virtual void providePbxTimerEvent(Sptr<UaHardwareEvent> msg ) ;


        void provideCallInfo(string, string, string);

        /**        */
        void provideDtmf(DeviceSignalType,int deviceId);


	void reportHardwareDtmfEvent(AssistDevice *dev);
        // ****************

//	virtual Sptr<SipCallId> getPbxCallId(int deviceId );
//	virtual void setPbxCallId( const Sptr<SipCallId> newCallId , int deviceId);
	virtual void setCallId( const Sptr<SipCallId> newCallId , int deviceId);
	virtual Sptr<SipCallId> getCallId( int deviceId) ;
	//added by lijie
	virtual void addCallWaitingId( const Sptr<SipCallId> newCallId, int deviceId );
	virtual Sptr<SipCallId> getCallWaitingId(int deviceId);// const;
	virtual void removeCallWaitingId( const SipCallId& callId, int deviceId );
	virtual bool isOnCallWaiting(const SipCallId &callid, int deviceId);
	//added ends here
	virtual int getDeviceId( const SipCallId& callId ) ;
	virtual int getPbxDeviceId( const SipCallId& callId ) ;
	virtual int getFreePbxDevice( ) ;
	virtual Sptr<DigitCollector> getDigitCollector(int deviceId) ;
	virtual AssistDevice* getAssistDevice(int deviceId);
	virtual AssistDevice*  getAssistDeviceInfo(const char *deviceName);
	virtual AssistDevice*  getAssistDeviceInfoByPhoneNumber(const char *phonenumbre) ;

        // ****************

	as_span_t *span;

#if 0
        // RTP session variables
        RtpSession* audioStack;
        RtpPacket* inRtpPkt;
        char outBufferPkt[480];
#endif

		const char* myDeviceName;


        // this is used to lock the RTP thread and the hardware thread so that
        // they don't access the hardware device simultaneously
        VMutex deviceMutex;

		/* added by lizhijie, 2005.03.09 , for mutex lock between AssistPbxRtp, Hardware, Worker 3 threads*/
		VMutex pbxDeviceMutex;
		
		AssistDeviceInfo *currentDevInfo; /* current device name for PBX calling */

//		AssistPbxDevice  *myPbxDev;

		/* added by lizhijie, 2005.03.04 */
		assist_app_type  appType;
#else
   public:
        /** Creates a hardware object to control a single port gateway.
         */
        AssistNullDevice( const char* deviceName, as_span_t *span,
                        Sptr < Fifo < Sptr < SipProxyEvent > > > inputQ,
                        Sptr < Fifo < Sptr < SipProxyEvent > > > outputQ , assist_app_type apptype );

        ~AssistNullDevice();


        void* hardwareMain (void* parms);

        /** add device's socket to the fd , returns 0 if successful, errorcode otherwise */
        int addToFdSet (fd_set* fd);

        /** process all pending hardware events; returns 0 if successful,otherwise returns an errorcode.  */
        int process (fd_set* fd);

        /// start audio channel on device
        /// returns 0 if successful, errorcode otherwise
        int audioStart (const HardwareAudioRequest& request,int deviceId);

        /// stops audio channel on device
        /// returns 0 if successful, errorcode otherwise
        int audioStop (int deviceId);

        /// suspend audio channel on device
        /// returns 0 if successful, errorcode otherwise
        int audioSuspend (int deviceId);

        /// resume audio channel on device
        /// returns 0 if successful, errorcode otherwise
        int audioResume (const HardwareAudioRequest& request,int deviceId);

        /// process RTP audio packets
	virtual void processRTP ();
	virtual void processPbxRTP();

	virtual int getRtpPort(int deviceId);
	virtual void releaseRtpPort(int deviceId);
		
	virtual HardwareStatusType getHardwareStatus(int deviceId);
	virtual bool isMyHardware(const SipCallId& callid, int deviceId);
	virtual void setAudioChannel(int destDeviceId, int sourceDeviceId);

   private:

      	 /** added to allow for 'deviceEventFlash'   */
      	void onhookOrFlash(as_device_t *dev);

	void  *slicHardwareMain(void *param);
	void  *pcmHardwareMain(void *param);

	void provideQueryStatus(int deviceId);
	void provideDialToneStart(int deviceId);
	void provideDialToneStop(int deviceId);
	void provideRingStart(int deviceId);
	void provideRingStop(int deviceId) ;
	void provideLocalRingbackStart(int deviceId) ;
	void provideLocalRingbackStop(int deviceId) ;
	void provideRemoteRingbackStart(int deviceId) ;
	void provideRemoteRingbackStop(int deviceId) ;
	void provideBusyToneStart(int deviceId);
	void provideBusyToneStop(int deviceId) ;
	void provideFastBusyToneStart(int deviceId);
	void provideFastBusyToneStop(int deviceId) ;
	void provideCallWaitingBeepStart(int deviceId);
	void provideCallWaitingBeepStop(int deviceId) ;
	virtual void provideDtmf(DeviceSignalType, int deviceId);

	virtual void providePbxTimerEvent(Sptr<UaHardwareEvent> msg ) ;

	void provideLocalErrorTone(int deviceId, DeviceSignalType tsignal);

        void provideCallInfo(string, string, string);

	void reportHardwareDtmfEvent(AssistDevice *dev);
        // ****************

	virtual void setCallId( const Sptr<SipCallId> newCallId , int deviceId);
	virtual Sptr<SipCallId> getCallId( int deviceId) ;
	virtual void addCallWaitingId( const Sptr<SipCallId> newCallId, int deviceId );
	virtual Sptr<SipCallId> getCallWaitingId(int deviceId);
	virtual void removeCallWaitingId( const SipCallId& callId, int deviceId );
	virtual bool isOnCallWaiting(const SipCallId &callid, int deviceId);
	
	virtual int getDeviceId( const SipCallId& callId );
	virtual int getPbxDeviceId( const SipCallId& callId );
	virtual int getFreePbxDevice( ) ;
	
	virtual string  getAllTransferUrl(int deviceId);
	virtual string  getTimeOutTransferUrl(int deviceId);
	virtual string  getBusyTransferUrl(int deviceId);

	virtual bool getCallWaitingOn(int deviceId);
	
	virtual Sptr<DigitCollector> getDigitCollector(int deviceId) ;
	virtual string  getPhoneNumber(int deviceId);
	virtual string  getDeviceName(int deviceId);

	virtual AssistDevice* getAssistDevice(int deviceId);
	virtual AssistDevice*  getAssistDeviceInfo(const char *deviceName);
	virtual AssistDevice*  getAssistDeviceByPhoneNumber(const char *phonenumber, const SipCallId& callid) ;

	 virtual int getCallCount(int deviceId);
	 virtual void setCallCount(int count, int deviceId);
	 virtual void incCallCount(int deviceId );
	 virtual void decCallCount(int deviceId);
    
	  inline int getDevInfoPosition(int deviceType, int deviceId);
	  virtual void  reportDeviceInfo(int deviceId, CallAction action, const string& wrstr = "Default");
	 //report all devices' info
        //virtual void reportDeviceInfo();
	as_span_t *span;

	const char* myDeviceName;

	/* added by lizhijie, 2005.03.04 */
	assist_app_type  appType;
	
	DeviceInfoPos devInfoPos[3];
	static char bufstatus[21];
	static int curline;
#endif

};
 
}

#endif
