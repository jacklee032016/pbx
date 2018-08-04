#ifndef __ISDN_END_POINT_HXX__
#define __ISDN_END_POINT_HXX__
/*
* $Id: IsdnEndPoint.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "EndPoint.hxx"
#include "EpState.hxx"
#include "as_msg.h"

namespace Assist 
{

#define FRAME_INFO_NOT_EXIST				-1

class CtrlDevice;

typedef enum
{ /* states as viewed from io port (state of calls are always connected) */
	ISDN_EP_STATE_IDLE = 0,				/* no call */
	ISDN_EP_STATE_IN_SETUP,			/* setup sent */
	ISDN_EP_STATE_OUT_SETUP,			/* setup sent */
#if 0	
	/* following 2 states are not offered in INS */
	EP_STATE_IN_OVERLAP,			/* more information */
	EP_STATE_OUT_OVERLAP,		/* more information */
#endif
	ISDN_EP_STATE_IN_PROCEEDING,		/* proceeding */
	ISDN_EP_STATE_OUT_PROCEEDING,	/* proceeding */
	ISDN_EP_STATE_IN_ALERTING,		/* ringing */
	ISDN_EP_STATE_OUT_ALERTING,		/* ringing */
	ISDN_EP_STATE_CONNECT,			/* connected */
	ISDN_EP_STATE_IN_DISCONNECT,		/* disconnected receiving tones */
	ISDN_EP_STATE_OUT_DISCONNECT,	/* disconnected sending tones */
	ISDN_EP_STATE_WAIT_CLEAR_CR,	/* wait to RELEASE_CR msg from driver */
	ISDN_EP_STATE_ERROR
}isdn_ep_state_t;

typedef enum
{ /* release actions: see epoint.release */
	RELEASE_NONE,
	RELEASE_CALL,		/* call, hold */
	RELEASE_PORT_CALLONLY,	/* call, port */
	RELEASE_ALL,		/* call, hold, port */
}release_t;

/* structure of an IsdnEndPoint */
class IsdnEndPoint : public EndPoint
{
	public:
		IsdnEndPoint( CtrlDevice *_ctrlDev, int isNt );
		virtual ~IsdnEndPoint();
		
		int			handler(void);

		void setCallIdByFrame( Sptr <CallId> callId, int _frameInfo );
		Sptr <CallId> getCallIdByFrame(int _frameInfo);

		void setAudio(Sptr <AudioChannel> _audio) ;
		
		void setFrameInfo(int _frameInfo);
		int getFrameInfo();

#if 0
		void setCallRef( int _callRef);
		int getCallRef();
#endif		

#if WITH_DEBUG		
		virtual const char *debugInfo();
#endif

		virtual void reset();

	protected:


	private:

		int							myFrameInfo;		/*frame dinfo field for this call */
#if 0		
		int							myCallRef;			/* CallRef in L3 MSG */
#endif
};

}

#endif

