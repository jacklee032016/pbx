#ifndef __CALL_TIMER_EVENT_HXX__
#define __CALL_TIMER_EVENT_HXX__

/*
 * $Id: CallTimerEvent.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */

#include <PbxTimerEvent.hxx>

namespace Assist
{
/* Timer Event used in VPBX Call Scheduler */
class CallTimerEvent : public PbxTimerEvent
{
	public:
		CallTimerEvent( const Sptr < Fifo < Sptr <PbxEvent> > > destFifo );
		virtual ~CallTimerEvent();

		virtual const char* const name() const;

		void setCallId( const Sptr <CallId> callid );
		const Sptr <CallId> getCallId() const;
		void removeCall();
		
	protected:

	private:
		Sptr <CallId> callId;

};
 
}

#endif

