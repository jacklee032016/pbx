/*
* $Id: IsdnCtrlDevice.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/
#ifndef __ISDN_CTRL_DEVICE_HXX__
#define __ISDN_CTRL_DEVICE_HXX__

#include "Sptr.hxx"
#include <VMutex.h>
#include "VException.hxx"
#include "vpbx_globals.h"

#include "DeviceMgr.hxx"
#include "CtrlDevice.hxx"
#include "PbxHwEvent.hxx"

#include "CallId.hxx"

#include "IsdnAudioChannel.hxx"

#include "as_isdn.h"

namespace Assist
{

class DeviceMgr;
class IsdnMsg;
class IsdnEndPoint;
class MsgDecoder;
class MsgEncoder;


#define AS_PBX_DTMF_BUFFER_LENGTH		16
class IsdnCtrlDevice : public CtrlDevice 
{
	public:
		IsdnCtrlDevice(DeviceMgr *_parent, int fd, int _portnum, int _ptp, char *_name);
		virtual ~IsdnCtrlDevice();
		int init( );

		virtual int hardwareMain(void *params/*msg_t *msg*/);
		
		/* send HW command to device */
		virtual void sendHwCommand( Sptr <PbxEvent> hwCommand ) ;
		
//		msg_t *createL3Msg(int prim, int mt, int dinfo, int size);
		msg_t *createL3Msg(int prim, int mt, int size,  Sptr <IsdnEndPoint> isdnEp );
		
//		void addIsdnMsgQueue( msg_t *msg);

#if 0		
		Sptr < AudioChannel> lookupAudioCh(iframe_t *frm);
		Sptr < AudioChannel> lookupAudioCh(msg_t *msg);
		Sptr < AudioChannel> lookupAudioCh(int  address);
#endif

		Sptr < AudioChannel> getAudioCh(int index);
		/* get CHAN_ID_B1_CHANNEL or CHAN_ID_B2_CHANNEL channel and BChan is idle */
		Sptr < AudioChannel> getAudioChByChanId(int chanId);
		Sptr <AudioChannel> getNextAudio( Sptr <AudioChannel> _audio);
		/* chanID can be CHAN_ID_ANY_CHANNEL, CHAN_ID_B1_CHANNEL, not CHAN_ID_B2_CHANNEL */
		Sptr <AudioChannel> allocateBChan(int chanId);
		Sptr <AudioChannel> allocateAnyBChan();

		Sptr <EndPoint> getNextEP( Sptr <EndPoint> _ep);
		Sptr <EndPoint> lookupFreeEP();
		Sptr <EndPoint> lookupEPByFrame( int _frameInfo);
		virtual Sptr <EndPoint> lookupEPByPhoneNumber( const char *phoneNumber);
		char *getPhoneNumber()
		{
			return myPhoneNumber;
		};

		int replaceEpFrameInfo(int oldFrameInfo, int newFrameInfo);
		
		int getProcId(int bChIndex)
		{
			return 0;//getAudioCh(bChIndex)->getProcId();
		}
		
		void setProcId(int index, int procId)
		{
			;//getBCh(index)->setProcId(procId );
		};

		virtual int activate()= 0;
#if 0
		int handlerMesgFromQueue(unsigned long fromId, mesg_type_t type, union parameter *param)
		{
			return 1;
		};
#endif

		unsigned int getAddress()
		{
			return myAddress;
		};

		int getStackId()
		{
			return myStackId;
		}

		int getFd()
		{
			return myMainFd;
		};

		int getPortNumber()
		{
			return myPortNum;
		}

//		msg_queue_t downqueue;		/* l4->l3 */
		int l1Link; 				/* if l1 is available (only works with nt-mode) */
		int l2Link; 				/* if l2 is available (at PTP we take this serious) */
	//	time_t l1timeout; 			/* timout when establishing link */
		time_t l2Establish; 		/* time until establishing after link failure */

#if WITH_DEBUG
		virtual const char *debugInfo();
#endif


#if 0
		void setPhoneNumber(char *phone);
		char *getPhoneNumber()
		{
			return phonenumber;
		};
#endif
		int getCallRef();
		
		/* only L3 msg are handled in this function, called by myIsdnHardwareMain */
		virtual int myL3MsgHandler( msg_t *msg) = 0;

		Sptr <MsgDecoder> getDecoder() const;
		Sptr <MsgEncoder> getEncoder() const;

		/* send to HW command thread or User Space NT stack */
		virtual int sendOut(msg_t *msg, Sptr <EndPoint> ep) = 0;

		int getNtMode();
		
		virtual int kernelTimerMsg( msg_t *msg)= 0;
		virtual int timerEventExpired(Sptr <PbxTimerEvent> timerEvent);

	protected:
		
		virtual int maintainDataLink( )= 0;
		virtual int HandleDownMsg(msg_t *msg);

		/* both L2 and L3 msg are handled in this function */
		virtual int myIsdnHardwareMain( msg_t *msg) = 0;

		int delUpperLayer(int UpperLayer);

		virtual void initStack()= 0;

		virtual void myInit( stack_info_t *stack_info) = 0;
		
//		virtual msg_t *myCreateL3Msg(int prim, int mt, int dinfo, int size)= 0;
		virtual msg_t *myCreateL3Msg(int prim, int mt, int size, Sptr <IsdnEndPoint> isdnEp)= 0;

		int 						myStackId;					/* D channel stack ID */
		
		int 						myUppestLayer;
		int 						myUppestProtocol;

		int 						myAddress;
		int 						myUpperId; 				/* id to transfer data down */
		int 						myLowerId; 				/* id to transfer data up */
		int 						myMultilink; 			/* if set, this port will not support callwaiting */

		Sptr <AudioChannel>		myAudioChans[B_CHAN_PERPORT];	
		Sptr <EndPoint> 			myEndPoints[B_CHAN_PERPORT];

		int 						myPtp; 					/* if ptp is set, we keep track of l2link */
		int						myMainFd;				/* file descriptor for D channel */
		int 						myPortNum;				/* port number for D channel, begin from 1(defined in stack)*/

		Sptr <MsgDecoder>		myDecoder;
		Sptr <MsgEncoder>		myEncoder;
		
		int 						ntMode;

	private:
		int newUpperLayer();

#if 0
		/* keep the Call Ref for the call originated from this D channel */
		int						myCallRef;
#endif

};

}

#endif

