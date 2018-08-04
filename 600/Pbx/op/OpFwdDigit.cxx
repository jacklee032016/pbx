/*
 * $Id: OpFwdDigit.cxx,v 1.3 2007/03/03 18:39:27 lizhijie Exp $
 */

#include "global.h"
#include "OpFwdDigit.hxx"
#include "UaDeviceEvent.hxx"
#include "UaCallInfo.hxx"
#include "UaConfiguration.hxx"
#include "InfoMsg.hxx"
#include "UaHardwareEvent.hxx"
#include "UaDevice.hxx"



using namespace Vocal;


OpFwdDigit::OpFwdDigit()
{
}


OpFwdDigit::~OpFwdDigit()
{
}


const char* const
OpFwdDigit::name() const
{
    return "OpFwdDigit";
}


const Sptr<State>
OpFwdDigit::process( const Sptr<SipProxyEvent> event )
{
    cpDebug(LOG_DEBUG,"OpFwdDigit operation");

    Sptr<UaDeviceEvent> deviceEvent;
    deviceEvent.dynamicCast( event );
    if ( deviceEvent == 0 )
    {
        cpDebug(LOG_DEBUG,"Not a deviceEvent");
        return 0;
    }

    // prepare NULL signal
	Sptr<UaHardwareEvent> signal = new UaHardwareEvent( UaDevice::getDeviceQueue() );
	signal->type = HardwareSignalType;
	signal->signalOrRequest.signal = DeviceSignalNULL;

	signal->setDeviceId( event->getDeviceId() );

    // check if signal should be sent to DeviceQueue
    switch( deviceEvent->type )
    {
    case DeviceEventDtmf0:
        signal->signalOrRequest.signal = DeviceSignalDigit0;
        break;
    case DeviceEventDtmf1:
        signal->signalOrRequest.signal = DeviceSignalDigit1;
        break;
    case DeviceEventDtmf2:
        signal->signalOrRequest.signal = DeviceSignalDigit2;
        break;
    case DeviceEventDtmf3:
        signal->signalOrRequest.signal = DeviceSignalDigit3;
        break;
    case DeviceEventDtmf4:
        signal->signalOrRequest.signal = DeviceSignalDigit4;
        break;
    case DeviceEventDtmf5:
        signal->signalOrRequest.signal = DeviceSignalDigit5;
        break;
    case DeviceEventDtmf6:
        signal->signalOrRequest.signal = DeviceSignalDigit6;
        break;
    case DeviceEventDtmf7:
        signal->signalOrRequest.signal = DeviceSignalDigit7;
        break;
    case DeviceEventDtmf8:
        signal->signalOrRequest.signal = DeviceSignalDigit8;
        break;
    case DeviceEventDtmf9:
        signal->signalOrRequest.signal = DeviceSignalDigit9;
        break;
    case DeviceEventDtmfStar:
        signal->signalOrRequest.signal = DeviceSignalDigitStar;
        break;
    case DeviceEventDtmfHash:
        signal->signalOrRequest.signal = DeviceSignalDigitHash;
        break;
    default:
        return 0;
        break;
    }

    // send signal to DeviceQueue
    if( signal->signalOrRequest.signal != DeviceSignalNULL )
    {
        cpDebug(LOG_DEBUG,"Sending DTMF signal %d to DeviceQueue",
              signal->signalOrRequest.signal - DeviceSignalDigit0);
        UaDevice::getDeviceQueue()->add( signal );
//        return 0;
    }
    return 0;
}


