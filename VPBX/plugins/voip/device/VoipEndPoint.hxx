#ifndef __VOIP_END_POINT_HXX__
#define __VOIP_END_POINT_HXX__
/*
* $Id: VoipEndPoint.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "EndPoint.hxx"
#include "PbxState.hxx"
#include "PbxTimerEvent.hxx"  
#include "PbxHwEvent.hxx"
#include "VMutex.h"
#include "EndPoint.hxx"
#include "RtpSession.hxx"
#include <vector>
#include <string>

#include "SipMsg.hxx"
#include "SipMsgQueue.hxx"
#include "SipTransceiver.hxx"

#include "voip_sip.hxx"
#include "VoipAudioChannel.hxx"

using namespace std;

namespace Assist 
{

class CtrlDevice;
class SipCallId;
class SipContext;

typedef enum
{
	VOIP_EP_STATE_IDLE 	= 0,
	VOIP_EP_STATE_TRYING,
	VOIP_EP_STATE_CANCEL_TRYING,		/* after send CANCEL msg in state of TRYING */
	VOIP_EP_STATE_RINGING,
#if WITH_CALLWAIT
	VOIP_EP_STATE_CALL_ENDED,
	VOIP_EP_STATE_ONHOLD,
#endif

	VOIP_EP_STATE_INCALL
}voip_ep_state_t;


typedef enum
{
	NEW_CALL = 0,
	DELETE_CALL,
	REPLACE_CALL,
	ACTION_UNKNOW = 99
}CallAction;


/* structure of an PstnEndPoint */
class VoipEndPoint : public EndPoint
{
	public:
		VoipEndPoint( CtrlDevice *_ctrlDev, Sptr <SipTransceiver> sipStack, bool callLegHistory=true );
		virtual ~VoipEndPoint();
		
		int			handler(void);

		const char *getName(void);
#if 1
		bool emitDtmfSignal(char signal);
		void recvRTPDTMF( int digit, int duration );
		int openDeviceFile();
		int audioStart (const HardwareAudioRequest& request );
		int audioStop();
		int audioSuspend ();
		int audioResume (const HardwareAudioRequest& request);
#endif 
	
		bool isConnected() const { return myconnected;}
		void setConnected(bool con = true) {myconnected = con;}

		virtual void reportDeviceInfo(CallAction action, const string& wrstr = "Default");

		virtual void reportSipEvent(Sptr <SipMsg> msg, Sptr <SipMsgQueue> sipRcv);

		void setSipCallId(SipCallId& sipCallId);
		SipCallId &getSipCallId();
		
		void setSipContext(Sptr <SipContext> context);
		Sptr <SipContext> getSipContext();

		int getRtpPort();

		Sptr <SipTransceiver> getSipStack();
		
		virtual void reset();

	private:

	private:

		VMutex 							audioMutex;
		VMutex 							actionMutex;
		
		bool							   	myconnected;

		SipCallId							mySipCallId;
		
		Sptr <SipContext>					mySipContext;

		Sptr <SipTransceiver>				mySipStack;
		
		bool 							myCallLegHistory;
#if 0
		int 								ringbackFd;
		int 								greetingFd;
		int 								busyFd;
		int 								callwaitingFd;
		int 								dialFd;
		
		AudioActionType 					actionType;
		as_pbx_dtmf_buffer  				dtmf_buf;
		as_pbx_dtmf_mode_t  				pbxDtmfMode;
		bool 							audioActive;      // true if two way speech is enabled
		bool 							audioHalfActive;  // true if one way speech is enabled
//		device_type_t					type;
		int								type;

		//CircularBuffer <char> myReadBuffer; /* read from rtp and send to fd or dsp engine */
		//CircularBuffer <char> myWriteBuffer;/* read from fd and send to rtp */
	
		RtpSession						*audioStack;
		//AssistRtpTransmitter *as_tran;
		RtpPacket						*inRtpPkt;
		DTMFInterface						*_DTMFInterface;		
#endif 

};

}

#endif

