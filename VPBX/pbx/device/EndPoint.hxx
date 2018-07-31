#ifndef __END_POINT_HXX__
#define __END_POINT_HXX__
/*
* $Id: EndPoint.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "PbxStateObject.hxx"
#include "CallId.hxx"
#include "CtrlDevice.hxx"
#include "EpState.hxx"

namespace Assist 
{

class CallEvent;
class CallId;
class CommonMsg;
class CallInfo;

#define PHONE_NUMBER_LENGTH			128

/* structure of an EndPoint */
class EndPoint : public PbxStateObject
{
	public:
		
		EndPoint( CtrlDevice *_ctrlDev );
		virtual ~EndPoint();
		
		void setCallId(Sptr <CallId>  _callId);
		Sptr <CallId>	getCallId();

		virtual void setAudio(Sptr <AudioChannel> _audio);
		Sptr <AudioChannel> getAudio();
		
		void setIndex(int _index);
		int getIndex();

		void setPhoneNumber(const char *_number);
		const char *getPhoneNumber() const
		{
			return myPhoneNumber;
		};

#if WITH_DEBUG		
		virtual const char *debugInfo();
#endif

		Sptr <CtrlDevice> getCtrlDevice()
		{
			return myCtrlDevice;
		};

		const char *getName(void);

#if 0
		void setCallInfo(Sptr <CallInfo> call);
		Sptr <CallInfo> getCallInfo();

		void backupCall(Sptr <CallInfo> newcall);
		Sptr <CallInfo> getBackupCall() const;
#endif

		void setDialing(char c)
		{
			dialing += c;
		}
		const string& getDialing() const { return dialing; }

		void resetDialing()
		{
			dialing = "";
		}

		void genDtmfEvent(char dtmf);
//		virtual void stopDialTone(){};

		void backupCallId()
		{
			myBackupCallId = myCallId;
			myCallId = 0;
		}

		void resumeCallId()
		{
			myCallId = myBackupCallId;
			myBackupCallId = 0;
				
		}

		bool isBuildProcess() const
		{
			return myBuildProcess;			
		}

		void setBuildProcess(bool state)
		{
			myBuildProcess = state;
			myAudio->setBuildProcess(state);
			//if (myBuildProcess) backupCallId();
			//else resumeCallId();
		}

		/* whether this EP is internal extension which has phone number */
		bool checkExt()
		{
			return isExt;
		}

		/* reset this endpoint for next call */
		virtual void reset();

		void setPeerNumber(const char* tel)
		{
			peerNumber = tel;
		}
		string getPeerNumber()
		{
			return peerNumber;
		}
		
		int callToConsole();

		void setPeerSignalType(dtmf_signal_type_t type)
		{
			myPeerSignalType = type;
		}
		dtmf_signal_type_t getPeerSignalType()
		{
			return myPeerSignalType;
		}
			
				
	protected:

		Sptr <AudioChannel>			myAudio;

		char							myName[VPBX_NAME_LENGTH];
		
		Sptr <CtrlDevice>				myCtrlDevice;
		
		Sptr <CallId>					myCallId;
		Sptr <CallId>					myBackupCallId;

		int							myIndex;		/* assigned by gatewayMgr*/

		char							myPhoneNumber[PHONE_NUMBER_LENGTH];

		/* for FXS EP, it is Caller ID; for FXO EP, it is Callee ID */
		string 	                          		peerNumber;	/* for Caller ID display */
		
//		Sptr<CallWaitingIdList> callWaitingIdList;

#if 0
		/*keep data between State change */
		Sptr <CallInfo>				myCall;
		Sptr <CallInfo>              		myBackupCall;
#endif

/*		has defined in PbxStateObject
		Sptr <PbxTimerEvent>			myTimer;
*/		
		string						dialing;			/* hold phone number dialing */

		Sptr <EndPoint>				mySelf;

		bool                                      	myBuildProcess;
		
		bool							isExt;			/* this endpoint is internal extension */

		dtmf_signal_type_t				myPeerSignalType;
	private:
		
};

}

#endif

