#include "global.h"
#include <cassert>
#include <cpLog.h>
#include <SipSdp.hxx>
#include "OpSuspendAudio.hxx"
#include "UaDevice.hxx"
#include "UaHardwareEvent.hxx"
#include "SipContext.hxx"

using namespace Assist;

const Sptr < State > VoipOpSuspendAudio::process( const Sptr < SipProxyEvent > event )
{
    cpDebug(LOG_DEBUG, "Suspend Audio operation" );

    Sptr < UaHardwareEvent > signal =
	new UaHardwareEvent( UaDevice::getDeviceQueue() );
    signal->type = HardwareAudioType;
    signal->signalOrRequest.request.type = AudioSuspend;

    UaDevice::getDeviceQueue()->add( signal );

    return 0;
}

