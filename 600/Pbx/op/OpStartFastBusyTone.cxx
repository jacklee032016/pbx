/*
 * $Id: OpStartFastBusyTone.cxx,v 1.2 2007/03/02 04:19:28 lizhijie Exp $
 */

#include "global.h"
#include <cassert>

#include <cpLog.h>

#include "OpStartFastBusyTone.hxx"
#include "UaHardwareEvent.hxx"
#include "UaDevice.hxx"

///
using namespace Vocal;


OpStartFastBusyTone::OpStartFastBusyTone()
{}

///
const Sptr < State >
OpStartFastBusyTone::process( const Sptr < SipProxyEvent > event )
{
    cpDebug(LOG_DEBUG, "Start FastBusy Tone" );

    Sptr < UaHardwareEvent > signal =
	new UaHardwareEvent( UaDevice::getDeviceQueue() );
    signal->type = HardwareSignalType;
    signal->signalOrRequest.signal = DeviceSignalFastBusyStart;

	/* added by lizhijie */
	signal->setDeviceId(event->getDeviceId() );

    UaDevice::getDeviceQueue()->add( signal );

    return 0;
}

///
OpStartFastBusyTone::~OpStartFastBusyTone()
{}

const char* const
OpStartFastBusyTone::name() const
{
    return "OpStartFastBusyTone";
}
