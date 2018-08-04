/*
* $Id: PstnCtrlDevice.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#ifndef __PSTN_CTRL_DEVICE_HXX__
#define __PSTN_CTRL_DEVICE_HXX__

#include <VMutex.h>
#include "VException.hxx"

#include "DeviceMgr.hxx"
#include "PbxHwEvent.hxx"

#include "assist_lib.h"
#include "pstn.hxx"

namespace Assist
{

class DeviceMgr;
class PstnEndPoint;
class PstnAudioChannel;

#define AS_PBX_DTMF_BUFFER_LENGTH		16

class PstnCtrlDevice : public CtrlDevice         
{
	public:
		PstnCtrlDevice(DeviceMgr *_parent, int _portnum, as_device_t *_asspan, char *_name);
		virtual ~PstnCtrlDevice();
		int init( );
		
		/* send HW command to device */
		virtual void sendHwCommand( Sptr <PbxEvent> hwCommand) ;

		virtual Sptr <EndPoint> lookupFreeEP() ;
		virtual int hardwareMain( void *params) { return 0;}
		Sptr <EndPoint> lookupEPByPhoneNumber(const char *_number);

	protected:		
	
 		/** Trigger the device to send the specified signal; returns 0 if successful, errorcode otherwise. */
		int provideSignal( DeviceSignalType signal );

	private:

};
#endif 
}


