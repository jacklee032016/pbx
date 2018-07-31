#ifndef __PBX_TIMER_EVENT_HXX__
#define __PBX_TIMER_EVENT_HXX__
/*
 * $Id: PbxTimerEvent.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */
#include "PbxEvent.hxx"

namespace Assist
{

/* timer event that send to a thread's input queue
 * child class include : CallTimerEvent, EpTimerEvent 
*/
class PbxTimerEvent : public PbxEvent
{
	public:
		/** Create a timer event given the associate fifo.       */
		PbxTimerEvent(Sptr < Fifo < Sptr <PbxEvent> > > destFifo);

		/** Virtual destructor*/
		virtual ~PbxTimerEvent();

		/** Post the given event to the associated fifo to expire in timerMs milliseconds.*/
		void startTimer(Sptr <PbxEvent> newEvent, const int timerMs);

#if 0
		void startTimer( const int timerMs);
#endif

		/** Cancel this previously started timer.*/
		void cancelTimer();

		/** Remove the call associated with the call info from the call container.*/
		void removeCall();

		/** Return the timer id of this previously started timer.*/
		FifoEventId getTimerId();

		/** Name of this class. */
		virtual const char* const name() const
		{
			return ( "PbxTimerEvent" );
		}

		//virtual Sptr <PbxStateObject> getStateObject() { return myCallInfo; }

	private:
		PbxTimerEvent(const PbxTimerEvent &);
		const PbxTimerEvent & operator=(const PbxTimerEvent &);
		
};
 
}

#endif

