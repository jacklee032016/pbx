/*
 * $Id: OpStopAudio.cxx,v 1.2 2007/03/02 04:19:28 lizhijie Exp $
 */

#include "global.h"
#include <cassert>
#include <cpLog.h>
#include "OpStopAudio.hxx"
#include "UaHardwareEvent.hxx"
#include "UaDevice.hxx"


///
using namespace Vocal;


OpStopAudio::OpStopAudio()
{}


///

const Sptr < State >
OpStopAudio::process( const Sptr < SipProxyEvent > event )
{
    cpDebug(LOG_DEBUG, "Stop Audio" );

	Sptr < UaHardwareEvent > signalAudio = new UaHardwareEvent( UaDevice::getDeviceQueue() );
	signalAudio->type = HardwareAudioType;
	signalAudio->signalOrRequest.request.type = AudioStop;
	signalAudio->setDeviceId( event->getDeviceId() );
	UaDevice::getDeviceQueue()->add( signalAudio );

	Sptr < UaHardwareEvent > signalBusy = 	new UaHardwareEvent( UaDevice::getDeviceQueue() );
	signalBusy->type = HardwareSignalType;
	signalBusy->signalOrRequest.signal = DeviceSignalBusyStart;
	signalBusy->setDeviceId( event->getDeviceId() );
	UaDevice::getDeviceQueue()->add( signalBusy );

	return 0;
}


///
OpStopAudio::~OpStopAudio()
{}


const char* const
OpStopAudio::name() const
{
    return "OpStopAudio";
}
