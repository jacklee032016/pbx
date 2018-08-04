#ifndef __ASSIST_DEVICE_H__
#define __ASSIST_DEVICE_H__
/*
 * $Id: AssistDevice.hxx,v 1.3 2007/09/03 06:50:41 lizhijie Exp $
*/
#include <VMutex.h>
#include "VException.hxx"

#include <Sptr.hxx>
#include <SipProxyEvent.hxx>
#include <SipCallId.hxx>
#include "AssistRtpTransmitter.hxx"
#include "RtpSession.hxx"
#include "ResGwDevice.hxx"
#include "VMutex.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "pbx_globals.h"
#include "UaConfiguration.hxx"
#include "CircularBuffer.hxx"

using Vocal::ADT::CircularBuffer;

namespace Vocal
{

class  AssistNullDevice;


#define AS_PBX_DTMF_BUFFER_LENGTH		64

typedef struct 
{
	unsigned char digits[AS_PBX_DTMF_BUFFER_LENGTH];
	int in_pos;
	int out_pos;
}as_pbx_dtmf_buffer;


class  AssistDevice
{
#if 0 //modified by lijie 2005-06-03
	public:

		AssistDevice(AssistNullDevice *_agent, char *devicename, device_type_t type, int deviceId, Sptr < Fifo < Sptr < SipProxyEvent > > > sessionQ) ;
		
		virtual ~AssistDevice();

		virtual void ProcessRTP() = 0;
		
		int getRtpPort();
		int reserveRtpPort(RtpSession *audioStack,int localMin, int localMax, int jitterNew);
		void releaseRtpPort();
	
		int audioStart (const HardwareAudioRequest& request );
		int audioStop();
		int audioSuspend ();
		int audioResume (const HardwareAudioRequest& request);

		Sptr < DigitCollector > getDigitCollector();
		
		HardwareStatusType getHardwareStatus();
		bool isMyHardware(const SipCallId& callid );

		void setCallId( const Sptr<SipCallId> newCallId );
		Sptr<SipCallId> getCallId() const;
		
		//added by lijie
		void addCallWaitingId( const Sptr<SipCallId> newCallId );
		Sptr<SipCallId> getCallWaitingId() const;
		void removeCallWaitingId( const SipCallId& callId );
		bool isOnCallWaiting(const SipCallId& callId);
		//added ends here
		int getFd();

		bool getstatus();
		
		virtual void startSendRingback() = 0;
		virtual void stopSendRingback() = 0;

		virtual void startSendGreeting() = 0;
		virtual void stopSendGreeting() = 0;

		virtual void startSendBusy() = 0;
		virtual void stopSendBusy() = 0;

		RtpSession* getAudioStack()
		{
			return audioStack;
		};

		void setAudioStack(RtpSession *_audioStack);
		
		//added by lijie to query device status
		virtual void provideQueryStatus() = 0;
		virtual void provideDialToneStart() = 0;
		virtual void provideDialToneStop() =0 ;

		virtual void provideRingStart() = 0;
		virtual void provideRingStop() = 0;

		virtual void provideLocalRingbackStart() = 0;;
		virtual void provideLocalRingbackStop() = 0;

		virtual void provideBusyToneStart() = 0;
		virtual void provideBusyToneStop() = 0;

		virtual void provideFastBusyToneStart() = 0;
		virtual void provideFastBusyToneStop() = 0;

		virtual void provideCallWaitingBeepStart() = 0;
		virtual void provideCallWaitingBeepStop() = 0;

		virtual void provideCallInfo(string, string, string) ;
		virtual void provideDtmf(DeviceSignalType);

		/* DTMF handling callback function, for template class of RTPDevice
		* only used in pbx virtual device , called by PbxRTPThread
		*/
		void recvRTPDTMF ( int digit, int duration );

		/* called by DeviceThread, for both hardware and virtual devices */
		virtual unsigned char  reportDtmfDigit( ) = 0;

		int getDeviceId()
		{
			return myDeviceId;
		};

		device_type_t getDeviceType()
		{
			return type;
		};

	protected:
		/* called by RTP pbx thread to simulate DTMF signal of virtual device */
		virtual bool emitDtmfSignal(char signal) = 0;

		/* added for PBX pcm channel remapping */
		virtual int openDeviceFile() =0 ;
		virtual void closeDeviceFile() = 0;
		
		/// Current call id
		Sptr<SipCallId> myCallId;
//		Sptr <CallInfo> myCallInfo;	/* added for second call of this device */

		//added by lijie
		Sptr<CallWaitingIdList> callWaitingIdList;
		//Sptr<CallIdList>	callIdList;
		//added ends here
		RtpSession* audioStack;
		AssistRtpTransmitter *as_tran;
		bool audioActive;      // true if two way speech is enabled
		bool audioHalfActive;  // true if one way speech is enabled

		RtpPacket* inRtpPkt;
		char outBufferPkt[480];

		bool hookStateOffhook;
		
		VMutex deviceMutex;
		
		device_type_t  type;
		int 			myDeviceId;
		int 			myFd;
		char 		*myDeviceName;

		Sptr < Fifo < Sptr < SipProxyEvent > > > callProcessingQueue;
		
		AssistNullDevice  		*agent;
		
		as_pbx_dtmf_buffer  	dtmf_buf;
		
		as_pbx_dtmf_mode_t  	pbxDtmfMode;

		assist_dsp_t  			*dsp;
		
		CircularBuffer <char> myReadBuffer; /* read from rtp and send to fd or dsp engine */
		CircularBuffer <char> myWriteBuffer;/* read from fd and send to rtp */

	private:

		Sptr<DigitCollector> myDigitCollector;

		DTMFInterface* _DTMFInterface;
#else
	public:

		friend class AssistNullDevice;
		
		AssistDevice(AssistNullDevice *_agent, char *devicename, device_type_t type, int deviceId, Sptr < Fifo < Sptr < SipProxyEvent > > > sessionQ) ;
		
		virtual ~AssistDevice();

		virtual void ProcessRTP() = 0;
		
		int getRtpPort();
		void releaseRtpPort();
	
		int audioStart (const HardwareAudioRequest& request );
		int audioStop();
		int audioSuspend ();
		int audioResume (const HardwareAudioRequest& request);

		Sptr < DigitCollector > getDigitCollector();
		
		HardwareStatusType getHardwareStatus();
		bool isMyHardware(const SipCallId& callid );

		void setCallId( const Sptr<SipCallId> newCallId );
		Sptr<SipCallId> getCallId() const;
		
		//added by lijie
		void addCallWaitingId( const Sptr<SipCallId> newCallId );
		Sptr<SipCallId> getCallWaitingId() const;
		void removeCallWaitingId( const SipCallId& callId );
		bool isOnCallWaiting(const SipCallId& callId);

		string  getAllTransferUrl();
		string  getTimeOutTransferUrl();
		string  getBusyTransferUrl();
		bool getCallWaitingOn();		

		//added ends here
		int getFd();

		bool getstatus();

		RtpSession* getAudioStack();

		void setAudioStack(RtpSession *_audioStack);

		//added by lijie to query device status
		virtual void provideQueryStatus() = 0;
		virtual void provideDialToneStart() = 0;
		virtual void provideDialToneStop() =0 ;

		virtual void provideRingStart() = 0;
		virtual void provideRingStop() = 0;

		virtual void provideLocalRingbackStart() = 0;;
		virtual void provideLocalRingbackStop() = 0;

		virtual void provideRemoteRingbackStart() = 0;;
		virtual void provideRemoteRingbackStop() = 0;
		virtual void provideBusyToneStart() = 0;
		virtual void provideBusyToneStop() = 0;

		virtual void provideFastBusyToneStart() = 0;
		virtual void provideFastBusyToneStop() = 0;

		virtual void provideCallWaitingBeepStart() = 0;
		virtual void provideCallWaitingBeepStop() = 0;

		virtual void provideCallInfo(string, string, string) ;
		virtual void provideDtmf(DeviceSignalType);

		/* DTMF handling callback function, for template class of RTPDevice
		* only used in pbx virtual device , called by PbxRTPThread
		*/
		void recvRTPDTMF ( int digit, int duration );

		/* called by DeviceThread, for both hardware and virtual devices */
		virtual unsigned char  reportDtmfDigit( ) = 0;

		int getDeviceId()
		{
			return myDeviceId;
		};

		device_type_t getDeviceType()
		{
			return type;
		};

		char *getDeviceName()
		{
			return myDeviceName;
		}

		void setPhoneNumber(char *phone);

		const char* getPhoneNumber();
		
		int getCallCount()
		{
			return callCount;
		}
		
		void setCallCount(int count)
		{
			callCount = count;
		}

		void incCallCount()
		{
			callCount++;
		}

		void decCallCount()
		{
			callCount--;
		}

		void setCallerId(string _callerId);

	protected:

		virtual int getSendBackTone(char *ringData, int size);

		/* called by RTP pbx thread to simulate DTMF signal of virtual device */
		virtual bool emitDtmfSignal(char signal) = 0;

		/* added for PBX pcm channel remapping */
		virtual int openDeviceFile() =0 ;
		virtual void closeDeviceFile() = 0;
		
		Sptr<SipCallId> myCallId;

		Sptr<CallWaitingIdList> callWaitingIdList;

		char phonenumber[AS_PBX_DTMF_BUFFER_LENGTH];
		
		bool audioActive;      // true if two way speech is enabled
		bool audioHalfActive;  // true if one way speech is enabled

		int ringbackFd;
		int greetingFd;
		int busyFd;
		int callwaitingFd;
	
		AudioActionType actionType;

		struct timeval prevTime; 
			
		char outBufferPkt[480];

		bool hookStateOffhook;

		device_type_t  type;
		int 			myDeviceId;
		int 			myFd;
		char 		*myDeviceName;

		Sptr < Fifo < Sptr < SipProxyEvent > > > callProcessingQueue;
		
		AssistNullDevice  		*agent;
		
		as_pbx_dtmf_buffer  	dtmf_buf;
		
		as_pbx_dtmf_mode_t  	pbxDtmfMode;

		assist_dsp_t  			*dsp;
		
		CircularBuffer <char> myReadBuffer; /* read from rtp and send to fd or dsp engine */
		CircularBuffer <char> myWriteBuffer;/* read from fd and send to rtp */


		VMutex audioMutex;
		VMutex actionMutex;
		RtpSession* audioStack;
		AssistRtpTransmitter *as_tran;
		RtpPacket* inRtpPkt;
		DTMFInterface* _DTMFInterface;

		bool					isOn;	/* li zhijie, 2007.05.12 */

		string 				callerNumber;	/* Li Zhijie, added 2008.09.02 */

	private:
		int reserveRtpPort(int localMin, int localMax, int jitterNew);
		Sptr<DigitCollector> myDigitCollector;
		int callCount;
#endif
};

}
#endif

