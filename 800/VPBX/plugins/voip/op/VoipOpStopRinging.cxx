
#include "global.h"
#include <cassert>
#include <cpLog.h>
#include <SipEvent.hxx>
#include <CancelMsg.hxx>
#include <ByeMsg.hxx>
#include "OpStopRinging.hxx"
#include "UaHardwareEvent.hxx"
#include "UaDevice.hxx"
#include "SipContext.hxx"
#include "UaCallContainer.hxx"
#include "SipMsg.hxx"

using namespace Assist;


const Sptr < State > VoipOpStopRinging::process( const Sptr < SipProxyEvent > event )
{
    cpDebug(LOG_DEBUG, "Stop Ringing" );

    // figure out if the call is going to be cancelled or become active
    bool callCancel = false;


    Sptr < SipEvent > sipEvent;
    sipEvent.dynamicCast( event );
    Sptr < UaDeviceEvent > deviceEvent;
    deviceEvent.dynamicCast( event );
	
    if ( sipEvent != 0 )
    {
        Sptr < SipMsg > sipMsg = sipEvent->getSipMsg();
        assert( sipMsg != 0 );
        Sptr < CancelMsg > msg;
        msg.dynamicCast( sipMsg );
        if ( msg != 0 )
        {
            callCancel = true;
        }

	 Sptr < ByeMsg > byeMsg;
        byeMsg.dynamicCast( sipMsg );
        if ( byeMsg != 0 )
        {
            callCancel = true;
        }
			
    }
    else if(isTimeout(event))
    {
    	 callCancel = true;
    }
    else if(deviceEvent !=0 )
    {
    	if ( deviceEvent->type == DeviceEventBusy||deviceEvent->type == DeviceEventHookDown )
		callCancel = true;
    }

	/* modify bu wuaiwu, 2005/10/19 */
#if 0
     Sptr < UaDeviceEvent > deviceEvent;
     deviceEvent.dynamicCast( event );

	 /* end of added */

    if(deviceEvent !=0 )
    {
    	if(deviceEvent->type == DeviceEventHookUp)
			callCancel = false;
    }
#endif 

    Sptr < UaCallContainer > calls;
    calls.dynamicCast( event->getCallContainer() );
    assert( calls != 0 );

    Sptr < SipContext > call;
    call.dynamicCast( event->getCallInfo() );
    assert( call != 0 );

    Sptr < UaHardwareEvent > signal =
	new UaHardwareEvent( UaDevice::getDeviceQueue() );
    signal->type = HardwareSignalType;

	signal->setDeviceId( event->getDeviceId() );

    SipCallId callId = call->getContact()->getInviteMsg().getCallId();

    if ( UaDevice::instance()->isMyHardware(callId, event->getDeviceId() ) )
    {
	//added by lijie
	if(call->getCalleeIdle())
	{
		 signal->signalOrRequest.signal = DeviceSignalRingStop;
	}
	else
		signal->signalOrRequest.signal = DeviceSignalNULL;
	//modified by lijie 2005-06-03
        if ( callCancel )
        {
		calls->deleteCall( callId, event->getDeviceId() );
		UaDevice::instance()->setCallId( 0 , event->getDeviceId() );
        }	
    }
    else
    {
        signal->signalOrRequest.signal = DeviceSignalStopCallWaitingBeep;
	//modified by lijie 2005-06-03
	if ( callCancel )
        {
   	     calls->deleteCall( callId, event->getDeviceId() );
            UaDevice::instance()->removeCallWaitingId( callId, event->getDeviceId() );
        }	
    }

	if (callCancel)
    		UaDevice::getDeviceQueue()->add( signal );

    return 0;
}
