#ifndef __CALL_SCHEDULER_HXX__
#define __CALL_SCHEDULER_HXX__
/*
 * $Id: CallScheduler.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */
 
#include "PbxManager.hxx"

#ifdef API
#include "api.h"
#include "intf.hxx"
#endif

namespace Assist
{

class PbxEvent;
class DigitTimeoutEvent;
class CallTimerEvent;
class PbxDeviceEvent;
class CallEvent;
class CallContainer;
class CallId;
class CallInfo;
class PbxAgent;


class CallScheduler : public PbxManager
{
	public:
		CallScheduler(PbxAgent *agent);
		~CallScheduler();

		Sptr <CallContainer> getCallContainer();

#ifdef API
		void set_queue_put(api_queue_t *queue);
		 api_msg_t * event2Struc(const Sptr <CallEvent> callEvent);
		api_result_t struc2Event(api_msg_t * msg);

#if 0
		void create_api_thread(api_queue_t *queue);
		api_queue_t *get_getQ();
		api_queue_t *get_putQ();
		void struc2Event(api_msg_t * msg);
		api_msg_t *event2Struc(const Sptr <CallEvent> callEvent);
		void struc2SetAudio(api_msg_t * msg);
		void sendResult(api_msg_t * msg, api_result_t result);
#endif
#endif
	protected:
		/* scheduling function */
		void myMainLoop( void *params );
		void myProcessEvent(Sptr <PbxEvent> signal);

		int startup()
		{
			return 0;
		};

	private:

		bool processCallEvent( const Sptr <CallEvent> callEvent );

		bool processCallTimerEvent( const Sptr <PbxTimerEvent> uaTimerEvent );

		//bool handleCallWaiting( Sptr< CallId > callInfo , int deviceId);
		bool handleCallWaiting( const Sptr <PbxEvent> sipEvent );
	

		Sptr <CallContainer> 				myCalls;

#ifdef API
		Sptr <CallInfo> 					myCallInfo;
		
	 	Sptr <CallId> 					myCallId;
		
		api_queue_t						*myPutQ;
//		api_queue_t 	*myGetQ;
#endif		
		CallScheduler(const CallScheduler &);
		const CallScheduler& operator=(const CallScheduler &);
		
};
 
}

#endif

