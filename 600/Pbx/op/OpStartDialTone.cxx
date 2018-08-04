/*
 * $Id: OpStartDialTone.cxx,v 1.2 2007/03/02 04:19:28 lizhijie Exp $
 */

#include "pbx_globals.h"
#include "global.h"
#include <cassert>

#include <cpLog.h>

#include "OpStartDialTone.hxx"
#include "UaHardwareEvent.hxx"
#include "UaDevice.hxx"
#include "SipMsg.hxx"
#include "AckMsg.hxx"
#include "SipEvent.hxx"

///
using namespace Vocal;


OpStartDialTone::OpStartDialTone()
{}

///
const Sptr < State >
OpStartDialTone::process( Sptr < SipProxyEvent > event )
{
    cpDebug(LOG_DEBUG, "StartDialTone operation" );

	/* modify by wuaiwu,2005/11/21 */
#if 0

	/* modify by wuaiwu, 2005/10/22 */

	int deviceId = event->getDeviceId();
	if (deviceId >= VIRTUAL_DEVICES_EPOCH)
      {
		Sptr <SipEvent> sipevent;
		sipevent.dynamicCast(event);
		if (sipevent != 0)
		{
			Sptr <SipMsg> sipMsg = sipevent->getSipMsg();
			Sptr <AckMsg> ackMsg;
			ackMsg.dynamicCast(sipMsg);
			if (ackMsg == 0)
			{
				return 0;
			}
		}
		else
			return 0;
    	}
	else
	{

		Sptr < UaDeviceEvent > deviceEvent;
		deviceEvent.dynamicCast( event );
		if ( deviceEvent == 0 )
		{
			return 0;
		}
		if ( deviceEvent->type != DeviceEventHookUp &&
			deviceEvent->type != DeviceEventFlash )
		{
			return 0;
		}
	}

	/* end of */
#endif 

    cpDebug(LOG_DEBUG, "Start Dial Tone on device %d" , event->getDeviceId());

    //cout << "Dialing ";

    UaDevice::instance()->getDigitCollector(event->getDeviceId() )->reset();

    Sptr < UaHardwareEvent > signal = new UaHardwareEvent( UaDevice::getDeviceQueue() );
	
    signal->type = HardwareSignalType;
    signal->signalOrRequest.signal = DeviceSignalDialToneStart;

	signal->setDeviceId(event->getDeviceId());/* added by lizhijie*/

    UaDevice::getDeviceQueue()->add( signal );

    return 0;
}


///
OpStartDialTone::~OpStartDialTone()
{}


const char* const
OpStartDialTone::name() const
{
    return "OpStartDialTone";
}
