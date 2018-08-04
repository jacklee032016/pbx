/*
 * $Id: OpStartBusyTone.cxx,v 1.2 2007/03/02 04:19:28 lizhijie Exp $
 */

#include "global.h"
#include <cassert>

#include <cpLog.h>

#include "OpStartBusyTone.hxx"
#include "UaHardwareEvent.hxx"
#include "UaDevice.hxx"


///
using namespace Vocal;


OpStartBusyTone::OpStartBusyTone()
{}


///

const Sptr < State >
OpStartBusyTone::process( Sptr < SipProxyEvent > event )
{
    cpDebug(LOG_DEBUG, "Start Busy Tone" );

    Sptr < UaHardwareEvent > signal = new UaHardwareEvent( UaDevice::getDeviceQueue() );
    signal->type = HardwareSignalType;
    signal->signalOrRequest.signal = DeviceSignalBusyStart;

	/* added by lizhijie */
	signal->setDeviceId(event->getDeviceId() );

    UaDevice::getDeviceQueue()->add( signal );

    return 0;
}


///
OpStartBusyTone::~OpStartBusyTone()
{}


const char* const
OpStartBusyTone::name() const
{
    return "OpStartBusyTone";
}
