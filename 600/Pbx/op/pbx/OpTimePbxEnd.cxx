/*
 * $Id: OpTimePbxEnd.cxx,v 1.2 2007/03/02 04:19:28 lizhijie Exp $
 */

#include "global.h"
#include "OpDeviceBusy.hxx"
#include "ResGwDevice.hxx"
#include "UaCallInfo.hxx"
#include "UaDevice.hxx"
#include "UaDeviceEvent.hxx"
#include "UaStateMachine.hxx"
#include "UaCallInfo.hxx"
#include "UaDevice.hxx"
#include "UaConfiguration.hxx"
#include "SdpHandler.hxx"
#include "Sdp2Media.hxx"
#include "UaTimerEvent.hxx"
#include "SystemInfo.hxx"
#include "SipSdp.hxx"
#include "UaCallContainer.hxx"
#include "OpTimePbxEnd.hxx"
#include "pbx_globals.h"
#include <ByeMsg.hxx>
#include "SipVia.hxx"
///
using namespace Vocal;


OpTimePbxEnd::OpTimePbxEnd()
{}


OpTimePbxEnd::~OpTimePbxEnd()
{}


///

const char* const
OpTimePbxEnd::name() const
{
    return "OpTimePbxEnd";
}


///
const Sptr < State >
OpTimePbxEnd::process( const Sptr < SipProxyEvent > event )
{
    cpDebug(LOG_DEBUG, "OpTimePbxEnd operation" );

    if ( isTimeout(event) == false )
        return 0;
	
    Sptr < UaTimerEvent > timerEvent;
    timerEvent.dynamicCast( event );
    if ( timerEvent == 0 )
    {
        return 0;
    }

     Sptr < UaCallInfo > call;
    call.dynamicCast( event->getCallInfo() );
    assert( call != 0 );


  int deviceId = UaDevice::instance()->getDeviceId(*(timerEvent->getCallId()));
	
   if (UaDevice::instance()->isMyHardware(*(timerEvent->getCallId()),deviceId)) 
   {
	if(call->getCalleeIdle())
	{
	   	Sptr < UaHardwareEvent > signalStop = new UaHardwareEvent( UaDevice::getDeviceQueue() );
	    	signalStop->type = HardwareSignalType;
		signalStop->setDeviceId( deviceId );
		UaDevice::getDeviceQueue()->add( signalStop );
		signalStop->signalOrRequest.signal = DeviceSignalRingStop;
		 UaDevice::getDeviceQueue()->add( signalStop );
		call->setCalleeIdle(false);
	}

	Sptr < UaHardwareEvent > signal =
	new UaHardwareEvent( UaDevice::getDeviceQueue() );
    	signal->type = HardwareSignalType;

     signal->setDeviceId( event->getDeviceId() );

     signal->signalOrRequest.signal = DeviceSignalBusyStart;
     UaDevice::getDeviceQueue()->add( signal );

	return 0;	
   }
   else
   {
	 return 0;
   }

}
