#ifndef __PSTN_EVENT_HXX__
#define __PSTN_EVENT_HXX__
/*
 * $Id: PstnEvent.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */

#include "PbxEvent.hxx"

namespace Assist
{

typedef enum
{
	DeviceEventUndefined = 0,
	DeviceEventShutdown,
	DeviceEventHookUp,			/* from on-hook to off-hook */
	DeviceEventHookDown,
	DeviceEventFlash,
	DeviceEventRingerOff,		/* for CallerId in Chinese standard */

	DeviceEventIdle,
	DeviceEventBusy,
	DeviceEventDropTalk,
	DeviceEventDtmf0 = 100,
	DeviceEventDtmf1,
	DeviceEventDtmf2,
	DeviceEventDtmf3,
	DeviceEventDtmf4,
	DeviceEventDtmf5,
	DeviceEventDtmf6,
	DeviceEventDtmf7,
	DeviceEventDtmf8,
	DeviceEventDtmf9,
	DeviceEventDtmfStar,
	DeviceEventDtmfHash,

	DeviceEventCar,
	DeviceEventIncomingEnd,		/* FSK demodulation ended after CAR, eg. notify onhook FXO port */
	
	DeviceEventCallUrl = 200
} DeviceEventType;

/**
* Event sended from PstnDeviceMgr to GateWayMgr. Bringed by 
* PstnDeviceMsg (DeviceThread) and processed by EndPoint StateMachine
* The inverted event is 
*/
class PstnEvent : public PbxEvent
{
	public:
		PstnEvent();

		/// Create given an associated fifo.
		PstnEvent(Sptr < Fifo < Sptr <PbxEvent> > > fifo);

		virtual ~PstnEvent();

		// The name of the extending class.
		virtual const char* const name() const 
		{
			return "PSTN-MSG";
		};

		//virtual Sptr <PbxStateObject> getStateObject() { return myEndPoint; }

	protected:

	private:
		PstnEvent(const PstnEvent &);
		const PstnEvent & operator=(const PstnEvent &);

	public:
		DeviceEventType type;
};
 
}

#endif

