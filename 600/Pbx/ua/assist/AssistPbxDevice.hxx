#ifndef  __ASSIST_PBX_DEVICE_H__
#define __ASSIST_PBX_DEVICE_H__
/*
 * $Id: AssistPbxDevice.hxx,v 1.1.1.1 2006/11/29 09:50:50 lizhijie Exp $
*/

#include "AssistDevice.hxx"

namespace Vocal
{

class AssistNullDevice;


class AssistPbxDevice: public AssistDevice
{
#if 0 //modified by lijie 2005-06-03
	public:
		AssistPbxDevice(AssistNullDevice *agent, char *devicename, int deviceid, Sptr < Fifo < Sptr < SipProxyEvent > > > sessionQ);
		~AssistPbxDevice();

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


		void setCalleeInfo(string _callernum, string _calleenum);

		pbx_stats getStatus()
		{
			return status;
		};
		
		void setStatus(pbx_stats stats)
		{
			status = stats;
		};

		virtual unsigned char  reportDtmfDigit( );

		void pbxStartTimer(DeviceSignalType signal);
		void pbxTimeoutHandler(Sptr < UaHardwareEvent > timerEvent);

		/* for internal call flow from pbx virtual device to hardware device */
		void setPbxCallId( const Sptr<SipCallId> newCallId );
		Sptr<SipCallId> getPbxCallId() const;
	protected:

		bool checkInterval();
		
		virtual int openDeviceFile()
		{
			return 0;
		};
		
		virtual void closeDeviceFile(){} ;
		
//		AssistNullDevice  *agent;
		bool emitDtmfSignal(char signal) ;

		
	private:
		virtual void startSendRingback() ;
		virtual void stopSendRingback() ;

		virtual void startSendGreeting() ;
		virtual void stopSendGreeting() ;

		virtual void startSendBusy() ;
		virtual void stopSendBusy() ;

		void resetPbxDtmfBuffer();
		
		char* getCwBeep();
		int getSendBackTone(char *ringData, int size);
		// call waiting beep related variables
		// call waiting beep is on for 0.3s and off 10s
		char* cwBeep;
		bool cwCadenceOn;
		int cwCnt;
		bool cwBeepOn;
		static const int CWBEEP_ON_TIME = 2400;   // 0.3 * 8000
		static const int CWBEEP_OFF_TIME = 80000;  // 10 * 8000

 		//variables for remote ringback tone
		bool sendRingback;
		bool sendGreeting;
		bool sendbusy;
		int ringbackFd;
		int greetingFd;
		int busyFd;

		pbx_stats status;
		string *callerNum;
		string *calleeNum;


		struct timeval lastTime; 
		struct timeval finish;  

		long lastTimeForRtp;

//		Sptr<SipCallId> pbxCallId;
#else
	public:
		friend class AssistNullDevice;
		
		AssistPbxDevice(AssistNullDevice *agent, char *devicename, int deviceid, Sptr < Fifo < Sptr < SipProxyEvent > > > sessionQ);
		virtual ~AssistPbxDevice();

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


		void setCalleeInfo(string _callernum, string _calleenum);

		virtual unsigned char  reportDtmfDigit( );

		void pbxStartTimer(DeviceSignalType signal);
		void pbxTimeoutHandler(Sptr < UaHardwareEvent > timerEvent);

	protected:

		virtual int openDeviceFile()
		{
			return 0;
		};
		
		virtual void closeDeviceFile(){} ;
		
		bool emitDtmfSignal(char signal) ;

		
	private:
		
		void resetPbxDtmfBuffer();
		
		char* getCwBeep();

		// call waiting beep related variables
		// call waiting beep is on for 0.3s and off 10s
		char* cwBeep;
		bool cwCadenceOn;
		int cwCnt;
		bool cwBeepOn;
		static const int CWBEEP_ON_TIME = 2400;   // 0.3 * 8000
		static const int CWBEEP_OFF_TIME = 80000;  // 10 * 8000

		string *callerNum;
		string *calleeNum;

		int count;
		string pbxUrl;
#endif

};

}

#endif

