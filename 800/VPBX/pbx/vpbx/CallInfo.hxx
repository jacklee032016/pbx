#ifndef __CALL_INFO_HXX__
#define __CALL_INFO_HXX__
/*
 * $Id: CallInfo.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */

#include "PbxStateObject.hxx"

#include "CallState.hxx"

#include "CallId.hxx"

namespace Assist
{

class PbxTimerEvent;

typedef enum
{ 
	CALL_STATE_IDLE,				/* no call */
	CALL_STATE_SETUP,				/* setup sent */
#if 0	
	CALL_STATE_OVERLAP,				/* more information */
#endif

	CALL_STATE_PROCEEDING,		/* proceeding */
	CALL_STATE_ALERTING,			/* ringing */
	CALL_STATE_CONNECT,			/* connected */
	CALL_STATE_DISCONNECT,		/* disconnected receiving tones */
	CALL_STATE_ERROR				
}call_state_t;

typedef enum
{
	CALL_CAUSE_NO_ROUTE,			/* No EP for this phone number to call out */
	CALL_CAUSE_BUSY_HERE,			/* dest EP is busy now */
	CALL_CAUSE_CALLER_CANCEL,
	CALL_CAUSE_NO_ERROR			/* no error, this must be last item in this enum */
}call_cause_t;


class CallInfo : public PbxStateObject
{
	public:
		CallInfo();
//		PbxCallInfo(const PbxCallInfo& rhs);
		virtual ~CallInfo();

		/** Assignment Operator * @param rhs the entry to assign
		* @return *this */
		const CallInfo & operator=(const CallInfo& rhs);

#if 0
		void setStateMachine(const Sptr <PbxStateMachine> _stateMachine);
		Sptr <PbxStateMachine > getStateMachine() const;
		
//		void setState(const Sptr <CallState > nextState);
//		Sptr <CallState > getState() const;
		void setState(const Sptr <PbxState > nextState);
		Sptr <PbxState > getState() const;
		/** CallData mutator*/
		void setCallStateData(Sptr < StateMachnData > callData);

		Sptr < StateMachnData > getCallStateData() const;
#endif

		Sptr <CallId> getCallId() const;
		void setCallId(Sptr <CallId> _callId);

		Sptr <EndPoint> getSrcEP() const;
		void setSrcEP(Sptr <EndPoint> _src);

		Sptr <EndPoint> getDestEP() const;
		void setDestEP(Sptr <EndPoint> _dest);
		
		const char *getName()
		{
			return "CALLInfo";
		};

	protected:
		

	private:

		/** Copy method  @param rhs builder to copy
		*  @return void        */
		void copyRhsToThis(const CallInfo& rhs);

		/** Pointer to the call's PbxTimerEvent*/
//		Sptr < PbxTimerEvent > 		myTimer;

		Sptr <CallId> 				myCallId;

		Sptr <EndPoint>				mySrc;
		Sptr <EndPoint>				myDest;

};
 
}

#endif

