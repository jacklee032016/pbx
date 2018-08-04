#ifndef __VOIP_CTRL_DEVICE_HXX__
#define __VOIP_CTRL_DEVICE_HXX__
/*
* $Id: VoipCtrlDevice.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include <VMutex.h>
#include "VException.hxx"
#include "DeviceMgr.hxx"
#include "VoipEndPoint.hxx"

namespace Assist
{

class DeviceMgr;
class DigitCollector;

class VoipCtrlDevice : public CtrlDevice         
{
	public:
		VoipCtrlDevice(DeviceMgr *_parent, Sptr <SipTransceiver> sipStack, int _myIndex,  char *_name, bool callLegHistory=true);
		virtual ~VoipCtrlDevice();
		int init( );
		
		/* send HW command to device */
		virtual void sendHwCommand( Sptr <PbxEvent> hwCommand) ;

		virtual Sptr <EndPoint> lookupFreeEP() ;
		virtual int hardwareMain( void *params) { return 0;}
		Sptr <EndPoint> lookupEPByPhoneNumber(const char *_number);

	protected:		
		
	private:

};

}

#endif 

