
/*
 * $Id: OpStopFastBusyTone.cxx,v 1.2 2007/03/02 04:19:28 lizhijie Exp $
 */

#include "global.h"
#include "OpStopFastBusyTone.hxx"
#include "UaHardwareEvent.hxx"
#include "UaDevice.hxx"

using namespace Vocal;

OpStopFastBusyTone::OpStopFastBusyTone()
{
}


///
const Sptr<State>
OpStopFastBusyTone::process( const Sptr<SipProxyEvent> event )
{
    cpDebug(LOG_DEBUG, "Stop Fast Busy Tone" );

    Sptr<UaHardwareEvent> signal =
	new UaHardwareEvent( UaDevice::getDeviceQueue() );
    signal->type = HardwareSignalType;
    signal->signalOrRequest.signal = DeviceSignalFastBusyStop;

	/* added by lizhijie */
	signal->setDeviceId(event->getDeviceId() );

    UaDevice::getDeviceQueue()->add( signal );

    return 0;
}


///
OpStopFastBusyTone::~OpStopFastBusyTone()
{
}


///
const char* const
OpStopFastBusyTone::name() const
{
    return "OpStopFastBusyTone";
}
