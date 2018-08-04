/*
 * $Id: OpSuspendAudio.cxx,v 1.2 2007/03/02 04:19:28 lizhijie Exp $
 */

#include "global.h"
#include <cassert>
#include <cpLog.h>
#include <SipSdp.hxx>
#include "OpSuspendAudio.hxx"
#include "UaDevice.hxx"
#include "UaHardwareEvent.hxx"
#include "UaCallInfo.hxx"


///
using namespace Vocal;


OpSuspendAudio::OpSuspendAudio()
{}


///
const Sptr < State >
OpSuspendAudio::process( const Sptr < SipProxyEvent > event )
{
    cpDebug(LOG_DEBUG, "Suspend Audio operation" );

    Sptr < UaHardwareEvent > signal =
	new UaHardwareEvent( UaDevice::getDeviceQueue() );
    signal->type = HardwareAudioType;
    signal->signalOrRequest.request.type = AudioSuspend;

    UaDevice::getDeviceQueue()->add( signal );

    return 0;
}


///
OpSuspendAudio::~OpSuspendAudio()
{}


///
const char* const
OpSuspendAudio::name() const
{
    return "OpSuspendAudio";
}
