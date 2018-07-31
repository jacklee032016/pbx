/*
* $Id: IsdnTeDevice.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/
#ifndef __ISDN_TE_DEVICE_HXX__
#define __ISDN_TE_DEVICE_HXX__

#include "IsdnAudioChannel.hxx"
#include "PbxDeviceEvent.hxx"
#include "IsdnCtrlDevice.hxx"

#include "asnt_isdn.h"

namespace Assist
{

class PbxEvent;

class IsdnTeDevice : public IsdnCtrlDevice
{
	public :
		IsdnTeDevice( DeviceMgr *_parent, int fd,  int _portnum, int _ptp );
		~IsdnTeDevice();
		
		int activate();

		/* send to HW command thread for TE device */
		int sendOut(msg_t *msg, Sptr <EndPoint> ep);

		virtual int kernelTimerMsg( msg_t *msg);

	protected:
		virtual int maintainDataLink( );
		virtual int myIsdnHardwareMain( msg_t *msg);
		virtual int myL3MsgHandler( msg_t *msg) ;
		
		void myInit( stack_info_t *stack_info) ;

		virtual void initStack();

#if 0
		/* lookup EndPoint by frameInfo 
		* If found ,return this EP, 
		* else return a free EP, 
		* otherwise return NULL when no EP available 
		*/
		using IsdnCtrlDevice::lookupEPByFrame;
		Sptr <EndPoint> lookupEPByFrame(Sptr <IsdnMsg> msg ) ;
#endif

//		virtual msg_t *myCreateL3Msg(int prim, int mt, int dinfo, int size);
		virtual msg_t *myCreateL3Msg(int prim, int mt,  int size, Sptr <IsdnEndPoint> isdnEp);

	private:
		void setupDownQueue( int link);
		
};

}

#endif

