/*
 * $Id: OpDropTalk.cxx,v 1.3 2007/03/03 18:39:27 lizhijie Exp $
 */

#include "global.h"

#include "ByeMsg.hxx"
#include "AckMsg.hxx"
#include "OpDropTalk.hxx"
#include "UaDeviceEvent.hxx"
#include "UaDevice.hxx"
#include "UaCallInfo.hxx"
#include "UaConfiguration.hxx"
#include "UaStateMachine.hxx"
#include "UaCallContainer.hxx"
#include "UaConfiguration.hxx"
#include "ResGwDevice.hxx"
#include "SystemInfo.hxx"
#include "SdpHandler.hxx"
#if 0
#include "UaCommandLine.hxx"  
#endif
#include "SipVia.hxx"

#ifdef HAS_RSVPLIB
#include "SuaRsvp.hxx"
#endif

#ifndef NO_POLICY
#include "PepAgent.hxx"
#endif

#if __CDR_OPEN__

#include "SimulateDB.hxx"
using namespace SDB;

#endif

using namespace Vocal;


OpDropTalk::OpDropTalk()
{
}


OpDropTalk::~OpDropTalk()
{
}


const char* const
OpDropTalk::name() const
{
    return "OpDropTalk";
}


const Sptr < State >
OpDropTalk::process( const Sptr < SipProxyEvent > event )
{
    cpDebug(LOG_DEBUG, "OpDropTalk operation" );

    Sptr < UaDeviceEvent > deviceEvent;
    deviceEvent.dynamicCast( event );
    if ( deviceEvent == 0 )
    {
        return 0;
    }

    if ( deviceEvent->type != DeviceEventDropTalk)
    {
        return 0;
    }

	/* modify by wuaiwu */

	event->setErrorInfo(ERR_LINEDROP);
	Sptr < UaStateMachine > stateMachine;
	stateMachine.dynamicCast( event->getCallInfo()->getFeature() );
	assert( stateMachine != 0 );
	
	return stateMachine->findState("StateError");
#if 0
    Sptr < UaHardwareEvent > signal;
    signal = new UaHardwareEvent( UaDevice::getDeviceQueue() );
    signal->type = HardwareSignalType;
    signal->setDeviceId(event->getDeviceId());
    signal->signalOrRequest.signal = DeviceSignalBusyStart;
    signal->setErrorInfo(ERR_LINEDROP);
    UaDevice::getDeviceQueue()->add( signal );

    Sptr < Fifo < Sptr < SipProxyEvent > > > eventQ = deviceEvent->getFifo();
    Sptr < UaDeviceEvent > sig= new UaDeviceEvent( eventQ );
   sig->type = DeviceEventHookDown;
   sig->setDeviceId( event->getDeviceId() );
   eventQ->add( sig );
#endif 
    return 0;
}


