#ifndef __ISDN_DEVICE_MGR_HXX__
#define __ISDN_DEVICE_MGR_HXX__
/*
* $Id: IsdnDeviceMgr.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "Sptr.hxx"
#include "Fifo.h"
#include "Random.hxx"

#include "PbxEvent.hxx"
#include "PbxHwEvent.hxx"
#include "PbxDeviceEvent.hxx"
#include "DeviceMgr.hxx"

#include "CallId.hxx"

#include "as_isdn.h"

#define  ASNT_DEBUG_OPTION \
		(DBGM_NET| \
		DBGM_FSM|	\
		DBGM_TEI|	\
		DBGM_L2	|	\
		DBGM_L3|		\
		DBGM_L3DATA|	\
		DBGM_BC|	\
		DBGM_TONE|	\
		DBGM_BCDATA|\
		DBGM_MAN| \
		DBGM_APPL|	\
		DBGM_ISDN|	\
		DBGM_SOCK|	\
		DBGM_CONN|	\
		DBGM_CDATA|\
		DBGM_DDATA	|\
		DBGM_SOUND|\
		DBGM_SDATA|\
		DBGM_MSG| \
		DBGM_TOPLEVEL)

/*
*/

typedef enum
{
	DialToneEmulation,
	RingbackToneEmulation
} ToneEmulation;


typedef enum
{
	pbx_unused = 0,
	pbx_ring,
	pbx_offhook,
	pbx_hanging,
	pbx_audio_stop
		
}pbx_stats;

#include "CtrlDevice.hxx"
#include "AudioChannel.hxx"

namespace Assist
{

class IsdnMgrThread;
class IsdnCtrlDevice;

#define ISDN_PORT_COUNT		4

class IsdnDeviceMgr : public DeviceMgr
{
	public:
		IsdnDeviceMgr(PbxAgent *agent);
		virtual ~IsdnDeviceMgr();
		
		virtual void  mainLoop( void* parms );

		Sptr <CtrlDevice> getDeviceByStackId( int  stackId );
		Sptr <CtrlDevice> getDeviceByAddress( unsigned int address);

		/* called by callScheduler to routing a call */		
		Sptr <EndPoint> lookupEndPointByNumber(const char *extPhoneNumber);

		int getMyFd()
		{
			return myFd;
		}

		/* used in CC_NEW_CR_REQ to get a callRef from driver */
		int newFrameInfo();

		int startup();

		void mgrThreadMainLoop();

		int reportMsgEvent(msg_t *msg, Sptr <EndPoint> ep);

	protected:
		void myMainLoop(void *params) ;

		virtual void activate()
		{
		};
		
		virtual int init();

		/// Process a msg received from an session
		void myProcessHwEvent( Sptr <PbxHwEvent > event );

	private:
		CtrlDevice *createDevice(int nt_mode, int port, int ptp_mode);
		void destory(void);
		void devsInfo(void);

		/* Process functions for ISDN-Mgr thread */
		/* for both NT/TE raw msg send by ISDN-Monitor thread 
		and NT msg send by GatewayThread (TE msg send by GatewayThread is not direct to myQueue */
		int processMsgEvent(Sptr <PbxDeviceEvent> msgEvent);

		/* only NT device has TimerEvent */
		int processTimerEvent(Sptr <PbxTimerEvent>  ntTimerEvent);

//		int processIsndMsgEvent(Sptr <>);	

		int 						myDebugOption;
		
		int 						myEntity;
		int 						myFd;	/* for D channel fd of all ports */
		bool 					originator;

		Sptr <Random>			myRandom;

		Sptr <IsdnMgrThread>		myMgrThread;
};
 
}

#endif
