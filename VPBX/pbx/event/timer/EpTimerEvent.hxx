#ifndef __EP_TIMER_EVENT_HXX__
#define __EP_TIMER_EVENT_HXX__

/*
 * $Id: EpTimerEvent.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */
#include <PbxTimerEvent.hxx>

namespace Assist
{

/* Timer Event used in Device Manager thread */
class EpTimerEvent : public PbxTimerEvent
{
	public:
		EpTimerEvent( const Sptr < Fifo < Sptr <PbxEvent> > > destFifo );
		virtual ~EpTimerEvent();
		//virtual Sptr <PbxStateObject> getStateObject() { return myEndPoint; }
		virtual const char* const name() const;

		void setEpId( int _epid )
		{
			myEpId = _epid;
		};

		const int getEpId() const
		{
			return myEpId;
		};
		
	protected:

	private:
		int			myEpId;

};
 
}

#endif

