/*
 * $Id: OpBranchIdle.cxx,v 1.3 2007/03/03 18:39:28 lizhijie Exp $
 */

#include "global.h"
#include "OpBranchIdle.hxx"
#include "ResGwDevice.hxx"

#include "UaDeviceEvent.hxx"
#include "UaStateMachine.hxx"
#include "UaCallInfo.hxx"
#include "UaDevice.hxx"
#include "UaConfiguration.hxx"
#include "SdpHandler.hxx"
#include "Sdp2Media.hxx"
#include "SystemInfo.hxx"
#include "SipSdp.hxx"
#include "SdpHandler.hxx"
#include "RsvpHandler.hxx"
#include "pbx_globals.h"

using namespace Vocal;


using Vocal::SDP::SdpSession;
using Vocal::SDP::SdpMedia;
using Vocal::SDP::MediaAttributes;
using Vocal::SDP::ValueAttribute;
using Vocal::SDP::SdpRtpMapAttribute;


OpBranchIdle::OpBranchIdle()
{
}


OpBranchIdle::~OpBranchIdle()
{
}


const char* const
OpBranchIdle::name() const
{
    return "OpBranchIdle";
}


const Sptr < State >
OpBranchIdle::process( const Sptr < SipProxyEvent > event )
{
    cpDebug(LOG_DEBUG, "OpBranchIdle operation" );

    Sptr < UaDeviceEvent > deviceEvent;
	
    deviceEvent.dynamicCast( event );
    if ( deviceEvent == 0 )
    {
        return 0;
    }

   
    if ( deviceEvent->type != DeviceEventIdle )
    {
        return 0;
    }

    Sptr < UaCallInfo > call;
    call.dynamicCast( event->getCallInfo() );
    assert( call != 0 );

    Sptr < UaHardwareEvent > signal =
	new UaHardwareEvent( UaDevice::getDeviceQueue() );
    signal->type = HardwareSignalType;

      int deviceId =UaDevice::instance()->getDeviceId( *(deviceEvent->callId));

     signal->setDeviceId( deviceId );

     signal->signalOrRequest.signal = DeviceSignalRingStart;
     UaDevice::getDeviceQueue()->add( signal );

    Sptr < UaHardwareEvent > signal2 =
	new UaHardwareEvent( UaDevice::getDeviceQueue() );
    signal2->type = HardwareSignalType;

     signal2->setDeviceId( event->getDeviceId() );

     signal2->signalOrRequest.signal = DeviceSignalLocalRingbackStart;
     UaDevice::getDeviceQueue()->add( signal2 );

    call->setCalleeIdle(true);

    return 0;
}

