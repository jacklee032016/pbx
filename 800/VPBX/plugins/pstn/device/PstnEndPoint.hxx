#ifndef __PSTN_END_POINT_HXX__
#define __PSTN_END_POINT_HXX__
/*
* $Id: PstnEndPoint.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "EndPoint.hxx"
#include "PbxTimerEvent.hxx"
#include "PbxHwEvent.hxx"

#include "assist_lib.h"
#include "pstn.hxx"

namespace Assist 
{

class CtrlDevice;

typedef enum
{
	PSTN_EP_STATE_IDLE = 0,
	PSTN_EP_STATE_DIALING,
	PSTN_EP_STATE_RINGING,
	PSTN_EP_STATE_TRYING,
	PSTN_EP_STATE_INCALL,
	PSTN_EP_STATE_BUSY,
	PSTN_EP_STATE_ERROR,
	PSTN_EP_STATE_CAR,
	PSTN_EP_STATE_CALLID,
	PSTN_EP_STATE_INCOMING
}pstn_ep_state_t;

/* structure of an PstnEndPoint */
class PstnEndPoint : public EndPoint
{
	public:
		PstnEndPoint( CtrlDevice *_ctrlDev , as_device_t* asdev);
		virtual ~PstnEndPoint();
		
		int			handler(void);

		int getDeviceId() const
		{
			return myDev->id;
		}

		as_device_t *getMyDev() const
		{
			return myDev;
		}

		void setTimer(const Sptr<PbxTimerEvent>& newTev) { myTimer = newTev; }
		const Sptr <PbxTimerEvent>& getTimer() const { return myTimer; }

		unsigned char  PstnEndPoint::reportDtmfDigit( );

		void reportPEvent(DeviceEventType singal );
		
		void reportDTMFEvent(DeviceEventType singal );

	private:
		VMutex 						audioMutex;
		VMutex 						actionMutex;
		as_device_t 		            		*myDev;
		

		Sptr <PbxTimerEvent> 			myTimer;

};

}

#endif

