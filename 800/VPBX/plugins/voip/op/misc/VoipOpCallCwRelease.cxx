
#include "global.h"
#include "OpCwRing.hxx"
#include "UaHardwareEvent.hxx"
#include "SipContext.hxx"
#include "UaDevice.hxx"
#include "UaStateMachine.hxx"
#include "PbxConfiguration.hxx"

using namespace Assist;

/* process _CallRelease for Call Wait when in RINGING */
const Sptr < State > VoipOpCwRing::process( const Sptr < SipProxyEvent > event )
{
	if(checkCallMsg(event) != CALL_RELEASE )
	{
		return PBX_STATE_CONTINUE;
	}


	Sptr < UaDeviceEvent > deviceEvent;
    deviceEvent.dynamicCast( event );
    if ( deviceEvent == 0 )
    {
        return 0;
    }
	
    if ( deviceEvent->type != DeviceEventHookDown )
    {
        return 0;
    }
	
    Sptr <SipContext> call;
    call.dynamicCast( event->getCallInfo() );
    assert( call != 0 );

    if(call->getCalleeIdle())
    {
    	Sptr <InviteMsg> msg = call->getRingInvite();
    	assert( msg != 0 );

	StatusMsg status( *msg, SIP_STATUS_CODE_CERROR_BUSY_HERE );

	deviceEvent->getSipStack()->sendReply( status );

   	Sptr < UaStateMachine > stateMachine;
    	stateMachine.dynamicCast( event->getCallInfo()->getFeature() );
   	 assert( stateMachine != 0 );

	return stateMachine->findState( "StateIdle" );
	
    }

    cpDebug(LOG_DEBUG, "Convert call waiting beep to actual ringing" );

    //for now, no support of 183 for the second call

    Sptr < SipCallId > call2Id = UaDevice::instance()->getCallWaitingId(event->getDeviceId());
    if ( call2Id != 0 )
    {
        Sptr < SipCallId > callId = deviceEvent->callId;
        if ( *callId == *call2Id )
        {
			//grab the hardware and remove itself from call waiting list
			UaDevice::instance()->setCallId( call2Id , event->getDeviceId()  );
			UaDevice::instance()->removeCallWaitingId( *call2Id, event->getDeviceId() );

            // stop cal waiting beep
            Sptr < UaHardwareEvent > signalCwbeep = new UaHardwareEvent( UaDevice::getDeviceQueue() );
            signalCwbeep->type = HardwareSignalType;
            signalCwbeep->signalOrRequest.signal = DeviceSignalStopCallWaitingBeep;

			signalCwbeep->setDeviceId( event->getDeviceId() );
			
            UaDevice::getDeviceQueue()->add( signalCwbeep );

            // ring the phone
            Sptr < UaHardwareEvent > signalRing = new UaHardwareEvent( UaDevice::getDeviceQueue() );
            signalRing->type = HardwareSignalType;
	      //modified by lijie     
	    #if 0
	     signalRing->signalOrRequest.signal = DeviceSignalRingStart;
	     #else
	      signalRing->signalOrRequest.signal = DeviceSignalQueryStatus;
	     #endif
	     signalRing->setDeviceId( event->getDeviceId() );
			
            UaDevice::getDeviceQueue()->add( signalRing );

        }
        else
            cpLog( LOG_ERR, "The call on call waiting is not this call" );
    }
    else
        cpLog( LOG_ERR, "No call on cal waiting" );

    return 0;
}

