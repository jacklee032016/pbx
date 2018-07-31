/*
* $Id: ConsoleCtrlDevice.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/
#ifndef __CONSOLE_CTRL_DEVICE_HXX__
#define __CONSOLE_CTRL_DEVICE_HXX__

#include "Sptr.hxx"

#include <VMutex.h>
#include "vpbx_globals.h"
#include "VException.hxx"

#include "PbxEvent.hxx"
#include "CallId.hxx"
#include "console.hxx"

#include "CtrlDevice.hxx"
#include "ConsoleAudioChannel.hxx"

namespace Assist
{

class DeviceMgr;
class ConsoleEndPoint;
class PbxTimerEvent;

class ConsoleCtrlDevice : public CtrlDevice 
{
	public:
		ConsoleCtrlDevice(DeviceMgr *_parent, int index );
		virtual ~ConsoleCtrlDevice();
		int init( );

		virtual int hardwareMain(void *params);
		
		/* send HW command to device */
		virtual void sendHwCommand( Sptr <PbxEvent> hwCommand) ;
		
		Sptr < AudioChannel> getAudioCh(int index);

		Sptr <EndPoint> lookupFreeEP();
		virtual Sptr <EndPoint> lookupEPByPhoneNumber( const char *phoneNumber);

		char *getPhoneNumber()
		{
			return myPhoneNumber;
		};


		virtual int activate() {return 0;};

		virtual int timerEventExpired(Sptr <PbxTimerEvent> timerEvent);

		Sptr <ConsoleEndPoint> getMasterEndPoint();
		Sptr <ConsoleAudioChannel> getMasterAudioChannel();

		Sptr <ConsoleEndPoint> getSlaveEndPoint();
		Sptr <ConsoleAudioChannel> getSlaveAudioChannel();

	protected:

	private:

};

}

#endif

