#ifndef __PBX_EVENT_HXX__
#define __PBX_EVENT_HXX__
/*
 * $Id: PbxEvent.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */

#include <vector>
#include "Sptr.hxx"
#include "Fifo.h"
#include "vpbx_globals.h"

#include "CallInfo.hxx"
#include "EndPoint.hxx"

namespace Assist
{

class PbxEvent;

/*
* common base class for Signal, Message and all other event in PBX environments 
* Children of this class are :
* 	PbxMsg		: hardware--->DeviceMgr
* 	CallEvent 	: DeviceMgr-->CallScheduler
* 	PbxHwEvent	: GatewayMgr(DeviceMgr)--->hardware
*	TimerEvent	: 
*/
class PbxEvent
{
	public:
		PbxEvent();
		/// Create given an associated fifo.
		PbxEvent(Sptr < Fifo < Sptr <PbxEvent> > > fifo);
		virtual ~PbxEvent();

		/** Post the given event to the fifo associated with this event.
		*  This only really makes sense if this event is newEvent.       */
		virtual void postEvent(const Sptr <PbxEvent> newEvent);

		/* Post the given event to the given fifo. */
		virtual void postEvent( const Sptr <PbxEvent> newEvent, const Sptr < Fifo < Sptr <PbxEvent> > > newFifo) const;

		/** Accessor to the fifo associate with this event. May be zero if not set. */
		Sptr < Fifo < Sptr <PbxEvent> > > getFifo() const;

		Sptr <EndPoint> getEndPoint();
		void setEndPoint(Sptr <EndPoint> _ep);
		
		Sptr <PbxStateObject> getStateObject() ;

		/* this method is only used in event of CallInfo, never event of EndPoint */
		void setStateObject(Sptr <PbxStateObject> _obj);

		// The name of the extending class.
		virtual const char* const name() const = 0;


		void setPrivate( void *_priv);
		void *getPrivate( );

		/* used only when reportEvent of queue*/
		Sptr < Fifo < Sptr <PbxEvent> > >  getQueue() const;
		/* used only when emit a timerout event */
		void setQueue(Sptr < Fifo < Sptr <PbxEvent> > > _queue);

	protected:
		/// The fifo associated with this event.
		Sptr < Fifo < Sptr <PbxEvent> > > 	myFifo;

		/** The FifoEventId associated with this event. This only makes
		*  sense for events that were posted to a fifo with a delay. */
		FifoEventId 						myId;


		Sptr <EndPoint> 					myEndPoint;

		/* refer to EndPoint or CallInfo in different cases */
		Sptr <PbxStateObject>				myObject;

		/* can not refer this temp object like this, lizhijie, 2006.05.26 */
//		Sptr <PbxEvent>					mySelf;
		
	private:
		PbxEvent(const PbxEvent &_isdnEvent);
		const PbxEvent & operator=(const PbxEvent &);

		void 							*priv;
		
};
 
}

#endif

