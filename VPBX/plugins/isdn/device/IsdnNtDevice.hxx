/*
* $Id: IsdnNtDevice.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/
#ifndef __ISDN_NT_DEVICE_HXX__
#define __ISDN_NT_DEVICE_HXX__

#include "IsdnAudioChannel.hxx"
#include "IsdnCtrlDevice.hxx"

#include "asnt_isdn.h"

namespace Assist
{

class IsdnNtDevice : public IsdnCtrlDevice
{
	public :
		IsdnNtDevice(DeviceMgr *_parent, int fd, int _portnum, int _ptp );
		~IsdnNtDevice();
		
		int activate();

		/* RX L3 call msg(CC_XXX) from User Space NT stack */
		virtual int myL3MsgHandler( msg_t *msg);

		/* send to User Space NT stack for NT device  */
		int enterStatckfromUpper(msg_t *msg);
		/* send msg to queue of IsdnDeviceMgr */
		int sendOut(msg_t *msg, Sptr <EndPoint> ep);

		/*  */
		int hwSendOutMsg( msg_t *msg);
		int hwSendOutFrame(int addr, int prim, int  dinfo, int dlen, void *dbuf );
		int hwSendOut(msg_t *msg);

		virtual int kernelTimerMsg( msg_t *msg);
		virtual int timerEventExpired(Sptr <PbxTimerEvent> timerEvent);

		PbxTimerEvent *createNtTimer( );

	protected:
		virtual int maintainDataLink( );
		
		virtual int myIsdnHardwareMain( msg_t *msg);
		
		virtual void myInit( stack_info_t *stack_info) ;
		virtual void initStack();

//		virtual msg_t *myCreateL3Msg(int prim, int mt, int dinfo, int size);
		virtual msg_t *myCreateL3Msg(int prim, int mt, int size, Sptr <IsdnEndPoint> isdnEp);

	private:	
		msg_t *createL2Msg(int addr, int prim, int dinfo, int size); /* NT only */

		NTSTACK 			*myNtStack;
};

}

#endif

