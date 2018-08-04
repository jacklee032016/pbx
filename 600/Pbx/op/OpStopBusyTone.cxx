/*
 * $Id: OpStopBusyTone.cxx,v 1.2 2007/03/02 04:19:28 lizhijie Exp $
 */

#include "global.h"
#include "OpStopBusyTone.hxx"
#include "UaHardwareEvent.hxx"
#include "UaDevice.hxx"


///
using namespace Vocal;


OpStopBusyTone::OpStopBusyTone()
{}


///
const Sptr < State >
OpStopBusyTone::process( const Sptr < SipProxyEvent > event )
{
    cpDebug(LOG_DEBUG, "Stop Busy Tone" );

    Sptr < UaHardwareEvent > signal =
	new UaHardwareEvent( UaDevice::getDeviceQueue() );
    signal->type = HardwareSignalType;
    signal->signalOrRequest.signal = DeviceSignalBusyStop;

	/* added by lizhijie */
	signal->setDeviceId(event->getDeviceId() );

    UaDevice::getDeviceQueue()->add( signal );

    return 0;
}


///
OpStopBusyTone::~OpStopBusyTone()
{}


const char* const
OpStopBusyTone::name() const
{
    return "OpStopBusyTone";
}
