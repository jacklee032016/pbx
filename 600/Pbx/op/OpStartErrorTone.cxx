/*
 * $Id: OpStartErrorTone.cxx,v 1.2 2007/03/02 04:19:28 lizhijie Exp $
 */

#include "global.h"
#include <cassert>

#include <cpLog.h>

#include "OpStartErrorTone.hxx"
#include "UaHardwareEvent.hxx"
#include "UaDevice.hxx"

///
using namespace Vocal;


OpStartErrorTone::OpStartErrorTone()
{}

///
const Sptr < State >
OpStartErrorTone::process( Sptr < SipProxyEvent > event )
{
	cpDebug(LOG_DEBUG, "Start Error Tone" );

	error_info_t et = event->getErrorInfo();
	DeviceSignalType st;


	switch (et)
	{
	case ERR_DIAL:
		st = DeviceSignalDialErrorTone; break;
	case ERR_CONNECT:
		st = DeviceSignalConnErrorTone; break;
	case ERR_LINEDROP:
		st =  DeviceSignalNetDropTone; break;
	default:
		st = DeviceSignalFastBusyStart; break;
	}

	cpDebug(LOG_DEBUG, "--- signal = %d", st);
    Sptr < UaHardwareEvent > signal =
	new UaHardwareEvent( UaDevice::getDeviceQueue() );
    signal->type = HardwareSignalType;
    signal->signalOrRequest.signal = st;

    signal->setDeviceId( event->getDeviceId() );
	
    UaDevice::getDeviceQueue()->add( signal );
    cpDebug(LOG_DEBUG, "---------event added");

    return 0;
}


///
OpStartErrorTone::~OpStartErrorTone()
{}


const char* const
OpStartErrorTone::name() const
{
    return "OpStartErrorTone";
}
