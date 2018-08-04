#ifndef __CONSOLE_END_POINT_HXX__
#define __CONSOLE_END_POINT_HXX__
/*
* $Id: ConsoleEndPoint.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "EpState.hxx"
#include "console.hxx"

namespace Assist 
{

typedef enum
{ /* states as viewed from io port (state of calls are always connected) */
	CONSOLE_EP_STATE_IDLE,				/* no call */
	CONSOLE_EP_STATE_IN_ALERTING,			/* ringing */
	CONSOLE_EP_STATE_IN_GREETING,			/* play greeting audio */
	CONSOLE_EP_STATE_OUT_CALLING,		/* ringing */
	CONSOLE_EP_STATE_OUT_ALERTING,		/* ringing */
	CONSOLE_EP_STATE_CONNECT,			/* connected */
	CONSOLE_EP_STATE_BUILDECALL
}console_ep_state_t;


#define MAX_PHONE_NUMBER_LENGTH		32

class CtrlDevice;

/* structure of an ConsoleEndPoint */
class ConsoleEndPoint : public EndPoint
{
	public:
		ConsoleEndPoint( CtrlDevice *_ctrlDev, console_dev_t  isMaster );
		virtual ~ConsoleEndPoint();
		
		int			handler(void);

		Sptr <ConsoleEndPoint> getSlave();
		void setSlave(Sptr <ConsoleEndPoint> slave);
		
#if WITH_DEBUG		
		virtual const char *debugInfo();
#endif
		void reset();


	virtual void stopDialTone();

	protected:

	private:

		Sptr <ConsoleEndPoint>		mySlave;

		console_dev_t					myDevType;

};

}

#endif

