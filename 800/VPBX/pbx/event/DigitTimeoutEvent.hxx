#ifndef __DIGIT_TIMTER_EVENT_HXX__
#define __DIGIT_TIMTER_EVENT_HXX__

/* 
 * $Id: DigitTimeoutEvent.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */

#include <PbxEvent.hxx>

namespace Assist
{

/** Inter-digit Timer Events  **/
class DigitTimeoutEvent : public PbxEvent
{
	public:
		DigitTimeoutEvent();
		DigitTimeoutEvent( const Sptr < Fifo < Sptr <PbxEvent> > > outputFifo );

		virtual ~DigitTimeoutEvent();
		//virtual Sptr <PbxStateObject> getStateObject() { return myEndPoint; }
		virtual const char* const name() const;

		void setDeviceId( int devId)
		{
			myDeviceId = devId;
		};
		
	protected:	

	private:
//		Sptr <CallId> 				callId;
//		Sptr <CtrlDevice>				myCtrlDevice;

		/*tmply defined */
		int 							myDeviceId;

};
 
}

#endif

