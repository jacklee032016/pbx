#ifndef __PBX_SIGNAL_HXX__
#define __PBX_SIGNAL_HXX__
/*
 * $Id: CallEvent.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */

#include "PbxEvent.hxx"

namespace Assist
{

class CommonMsg;
class CallContainer;

/*
* common super class for all Call Signal in PBX environments
* Signal are exchaned between EndPoint and Call Scheduler
* In all CallEvent send to CallScheduler by DeviceMgr, no callInfo is provided, CallEvent->calllInfo is 
* set by CallScheduler before enter into CallOperations
* In all CallEvent send to DeviceMgr by CallScheduler, CallEvent->callInfo has been set by CallScheduler 
*/
class CallEvent : public PbxEvent
{
	public:
		CallEvent();
		CallEvent(Sptr < Fifo < Sptr <PbxEvent> > > fifo);
		virtual ~CallEvent();

		virtual const char* const name() const 
		{
			return "CALL-EVENT";
		};

		Sptr <CallId> getCallId();
		void setCallId(Sptr <CallId> _callId);
		
		/* Set the call info and the call container associated with this event. */
//		void setCallInfo(const Sptr <CallInfo> callInfo, const Sptr <CallContainer> container );

		void setCallInfo(const Sptr <CallInfo> callInfo );

		/* Access the associated call info. May be zero if not set. */
		Sptr <CallInfo> getCallInfo() const;

		/* Remove the call info and call container associated with this event.*/
		void removeCallInfo();

		Sptr <CommonMsg>  getCallMsg();
		void setCallMsg(Sptr <CommonMsg> _msg);

		//void setEndPoint(Sptr <EndPoint> _ep);

	protected:

	private:
		/* passed from DeviceMgr to Scheduler */
		Sptr <CallId>							myCallId;
		
		/// The call info associated with this event.
		Sptr <CallInfo> 					myCallInfo;

		/// The call container associated with this event.
//		Sptr <CallContainer> 				myCallContainer;


		Sptr <CommonMsg>					myMsg;

		CallEvent(const CallEvent &_isdnEvent);
		const CallEvent & operator=(const CallEvent &);

};
 
}

#endif

