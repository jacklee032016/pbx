/*
$Id: OpPbxHookUp.cxx,v 1.1.1.1 2006/11/29 09:50:50 lizhijie Exp $
 */

#include "global.h"
#include "OpPbxHookUp.hxx"

#include "UaDigitTimerEvent.hxx"
#include "DigitCollector.hxx"
#include "UaDevice.hxx"
#include "UaConfiguration.hxx"
#include "UaCallInfo.hxx"
#include "UaStateMachine.hxx"
#include "AssistDevice.hxx"
#include "UaCallContainer.hxx"

using namespace Vocal;


OpPbxHookUp::OpPbxHookUp()
{
}


OpPbxHookUp::~OpPbxHookUp()
{
}


const char* const OpPbxHookUp::name() const
{
    return "OpPbxHookUp";
}


const Sptr < State > OpPbxHookUp::process( const Sptr < SipProxyEvent > event )
{
	cpDebug(LOG_DEBUG, "OpPbxHookUp  operation" );

	Sptr < UaDeviceEvent > deviceEvent;
	deviceEvent.dynamicCast( event );
	if ( deviceEvent == 0 )
	{
		return 0;
	}
	if ( deviceEvent->type != DeviceEventHookUp )
	{
		return 0;
	}

//commentted by lijie	2005-06-03
#if 0
#if 0
	Sptr < DigitCollector > digitCollector = UaDevice::instance()->getDigitCollector(event->getDeviceId() );
	assert( digitCollector != 0 );
	if ( !( digitCollector->isValidDial() ) )
	{
		return 0;
	}

	/* check whether this hardware is busy */
	UaDevice::instance()->setCallId(UaDevice::instance()->getCallId(event->getDeviceId()) ,dev->getDeviceId() );
#endif
	/* notify hardware device stop ring */
	Sptr < UaHardwareEvent > signal = new UaHardwareEvent( UaDevice::getDeviceQueue() );
	signal->type = HardwareSignalType;

	signal->setDeviceId( event->getDeviceId()/*event->getDeviceId() */);
	signal->signalOrRequest.signal = DeviceSignalRingStop;
	UaDevice::getDeviceQueue()->add( signal );

#if 0
	Sptr < UaStateMachine > stateMachine;
	stateMachine.dynamicCast( event->getCallInfo()->getFeature() );
	assert( stateMachine != 0 );

	return stateMachine->findState( "StatePbxInCall" );
#endif
	return 0;
#endif

//added by lijie 2005-06-03
     Sptr < UaCallInfo > call;
      call.dynamicCast( event->getCallInfo() );
      assert( call != 0 );
	
	if(!call->getCalleeIdle())
			return 0;

	int deviceId = UaDevice::instance()->getDeviceId(*(deviceEvent->callId) );

	UaDevice::instance()->setAudioChannel(deviceId, event->getDeviceId());
	
	//remove callid for current virtual device
	UaDevice::instance()->setCallId(0, event->getDeviceId());

	//replace device id 
	Sptr < UaCallContainer > calls;
	calls.dynamicCast( deviceEvent->getCallContainer() );

	calls->replaceCallDeviceId(*(deviceEvent->callId), event->getDeviceId(), deviceId );
	
	
	Sptr < UaStateMachine > stateMachine;
	stateMachine.dynamicCast( event->getCallInfo()->getFeature() );
	assert( stateMachine != 0 );

	return stateMachine->findState( "StateInCall" );
//added ends here		
}

