#ifndef __ASSIST_DEVICE_INFO_H__
#define __ASSIST_DEVICE_INFO_H__
/*
 * $Id: AssistDeviceInfo.hxx,v 1.2 2007/03/07 20:06:43 lizhijie Exp $
*/

#include <VMutex.h>
#include "VException.hxx"

#include <Sptr.hxx>
#include <SipProxyEvent.hxx>
#include <SipCallId.hxx>

#include <RtpSession.hxx>
#include <RtpPacket.hxx>

#include "ResGwDevice.hxx"

#include "AssistDevice.hxx"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "pbx_globals.h"

namespace Vocal
{

class  AssistDeviceInfo:public AssistDevice
{
#if 0 //modified by lijie 2005-06-03
	public:

		AssistDeviceInfo(AssistNullDevice *agent, as_span_t *_span, int id, Sptr < Fifo < Sptr < SipProxyEvent > > > sessionQ) ;

		
		~AssistDeviceInfo();

		void ProcessRTP();
		
		//added by lijie to query device status
		void provideQueryStatus();
		void provideDialToneStart();
		void provideDialToneStop();

		void provideRingStart();
		void provideRingStop();

		void provideLocalRingbackStart();
		void provideLocalRingbackStop();

		void provideBusyToneStart();
		void provideBusyToneStop();

		void provideFastBusyToneStart();
		void provideFastBusyToneStop();

		void provideCallWaitingBeepStart();
		void provideCallWaitingBeepStop();

		as_device_t*  getMyDevice()
		{
			return myDev;
		}

		virtual unsigned char  reportDtmfDigit( );

		void setPhoneNumber(char *phone);
		char *getPhoneNumber()
		{
			return phonenumber;
		};

		virtual void startSendRingback() {};
		virtual void stopSendRingback() {};

		virtual void startSendGreeting() {};
		virtual void stopSendGreeting() {};

		virtual void startSendBusy() {};
		virtual void stopSendBusy() {};
		
	protected:
		virtual int openDeviceFile()  ;
		virtual void closeDeviceFile() ;
		
		bool emitDtmfSignal(char signal) ;

		/* phone number for this hardware device */
		char phonenumber[AS_PBX_DTMF_BUFFER_LENGTH];
#if 0
		void startSendRingback()
		{
			isSendRingback = true;
		};

		void stopSendRingback()
		{
			isSendRingback = false;
		};
#endif		
		/// Current call id
//		Sptr<SipCallId> myCallId;

//		Sptr < Fifo < Sptr < SipProxyEvent > > > callProcessingQueue;

//		RtpSession* audioStack;
//		RtpPacket* inRtpPkt;
		
//		bool isSendRingback;

//		Sptr<DigitCollector> myDigitCollector;

		as_device_t 		*myDev;
		
//		bool audioActive;      // true if two way speech is enabled
//		bool audioHalfActive;  // true if one way speech is enabled

		int getSendBackTone(char *ringData, int size);
		int busyFd;

		VMutex deviceMutex;
	private:
#else
	public:
		friend class AssistNullDevice;
		
		AssistDeviceInfo( AssistNullDevice *agent, as_device_t *device, Sptr < Fifo < Sptr < SipProxyEvent > > > sessionQ);
		
		~AssistDeviceInfo();

		void ProcessRTP();
		
		void provideQueryStatus();
		void provideDialToneStart();
		void provideDialToneStop();

		void provideRingStart();
		void provideRingStop();

		void provideLocalRingbackStart();
		void provideLocalRingbackStop();

		void provideRemoteRingbackStart();
		void provideRemoteRingbackStop();
		void provideBusyToneStart();
		void provideBusyToneStop();

		void provideFastBusyToneStart();
		void provideFastBusyToneStop();

		void provideCallWaitingBeepStart();
		void provideCallWaitingBeepStop();

		as_device_t*  getMyDevice()
		{
			return myDev;
		}

		virtual unsigned char  reportDtmfDigit( );

	protected:
		virtual int openDeviceFile()  ;
		virtual void closeDeviceFile() ;
		
		bool emitDtmfSignal(char signal) ;

		as_device_t 		*myDev;

		string 	pstnUrl;
	private:
		
#endif	
};

}
#endif

