/*
 * $Id: OpBranchBusy.cxx,v 1.3 2007/03/03 18:39:28 lizhijie Exp $
 */

#include "global.h"
#include "OpBranchBusy.hxx"
#include "ResGwDevice.hxx"
#include <ByeMsg.hxx>
#include "SipVia.hxx"
#include "UaDeviceEvent.hxx"
#include "UaStateMachine.hxx"
#include "UaCallInfo.hxx"
#include "UaDevice.hxx"
#include "UaConfiguration.hxx"
#include "SdpHandler.hxx"
#include "Sdp2Media.hxx"
#include "SystemInfo.hxx"
#include "SipSdp.hxx"
#include "UaCallContainer.hxx"
#include "UaCallInfo.hxx"
#include "pbx_globals.h"

using namespace Vocal;


OpBranchBusy::OpBranchBusy()
{
}


OpBranchBusy::~OpBranchBusy()
{
}


const char* const
OpBranchBusy::name() const
{
    return "OpBranchBusy";
}


const Sptr < State >
OpBranchBusy::process( const Sptr < SipProxyEvent > event )
{
    cpDebug(LOG_DEBUG, "OpBranchBusy operation" );

 
    	Sptr < UaDeviceEvent > deviceEvent;	
    	deviceEvent.dynamicCast( event );
  
	 if ( deviceEvent == 0 )
    	{
       	 return 0;
    	}
	
	if ( deviceEvent->type != DeviceEventBusy )
    	{
       	 return 0;
    	}

     Sptr < UaHardwareEvent > signal =
	new UaHardwareEvent( UaDevice::getDeviceQueue() );
    signal->type = HardwareSignalType;

     signal->setDeviceId(event->getDeviceId());

     signal->signalOrRequest.signal = DeviceSignalBusyStart;
     UaDevice::getDeviceQueue()->add( signal );

	return 0;

}

