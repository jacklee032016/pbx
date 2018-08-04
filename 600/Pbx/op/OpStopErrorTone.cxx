/*
 * $Id: OpStopErrorTone.cxx,v 1.2 2007/03/02 04:19:28 lizhijie Exp $
 */

#include "global.h"
#include <cassert>
#include <cpLog.h>
#include "OpStopErrorTone.hxx"
#include "UaHardwareEvent.hxx"
#include "UaDevice.hxx"


///
using namespace Vocal;


OpStopErrorTone::OpStopErrorTone()
{}


///
const Sptr < State >
OpStopErrorTone::process( const Sptr < SipProxyEvent > event )
{
    cpDebug(LOG_DEBUG, "Stop Error Tone" );

    Sptr < UaHardwareEvent > signal =
	new UaHardwareEvent( UaDevice::getDeviceQueue() );
    signal->type = HardwareSignalType;
    //signal->signalOrRequest.signal = DeviceSignalErrorToneStop;
    signal->signalOrRequest.signal = DeviceSignalFastBusyStop;

	signal->setDeviceId( event->getDeviceId() );
	
    UaDevice::getDeviceQueue()->add( signal );

    return 0;
}


///
OpStopErrorTone::~OpStopErrorTone()
{}


///
const char* const
OpStopErrorTone::name() const
{
    return "OpStopErrorTone";
}
