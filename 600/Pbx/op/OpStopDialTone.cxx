/*
 * $Id: OpStopDialTone.cxx,v 1.2 2007/03/02 04:19:28 lizhijie Exp $
 */

#include "global.h"
#include <cassert>
#include <cpLog.h>
#include "OpStopDialTone.hxx"
#include "UaHardwareEvent.hxx"
#include "UaDevice.hxx"
#include "UaDigitTimerEvent.hxx"

///
using namespace Vocal;


OpStopDialTone::OpStopDialTone()
{}


///

const Sptr < State >
OpStopDialTone::process( const Sptr < SipProxyEvent > event )
{
    cpDebug(LOG_DEBUG, "OpStopDialTone operation" );

    Sptr < UaDeviceEvent > deviceEvent;
    deviceEvent.dynamicCast( event );
    if ( deviceEvent == 0 )
    {
        Sptr < UaDigitTimerEvent > timerEvent;
        timerEvent.dynamicCast( event );
        if ( timerEvent == 0 )
        {
            return 0;
        }
    }
    else
    {
        switch ( deviceEvent->type )
        {
            case DeviceEventHookDown:
            case DeviceEventDtmfStar:
            case DeviceEventDtmfHash:
            case DeviceEventDtmf0:
            case DeviceEventDtmf1:
            case DeviceEventDtmf2:
            case DeviceEventDtmf3:
            case DeviceEventDtmf4:
            case DeviceEventDtmf5:
            case DeviceEventDtmf6:
            case DeviceEventDtmf7:
            case DeviceEventDtmf8:
            case DeviceEventDtmf9:
        {}

            break;
            default:
            {
                // for any other cases, don't stop dial tone.
                return 0;
            }
        }
    }
    cpDebug(LOG_DEBUG, "Stop Dial Tone" );

    // stop dial tone on hardware
    Sptr < UaHardwareEvent > signal =
	new UaHardwareEvent( UaDevice::getDeviceQueue() );
    signal->type = HardwareSignalType;
    signal->signalOrRequest.signal = DeviceSignalDialToneStop;

	signal->setDeviceId( event->getDeviceId() );
	
    UaDevice::getDeviceQueue()->add( signal );

    return 0;
}


///
OpStopDialTone::~OpStopDialTone()
{}


///
const char* const
OpStopDialTone::name() const
{
    return "OpStopDialTone";
}
