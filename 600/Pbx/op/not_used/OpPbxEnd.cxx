/*
$Id: OpPbxEnd.cxx,v 1.1.1.1 2006/11/29 09:50:50 lizhijie Exp $
 */

#include "global.h"
#include "OpPbxEnd.hxx"
#include "UaCallContainer.hxx"

#include "UaDigitTimerEvent.hxx"
#include "DigitCollector.hxx"
#include "UaDevice.hxx"
#include "UaConfiguration.hxx"
#include "UaCallInfo.hxx"
#include "UaStateMachine.hxx"

#include "AssistDevice.hxx"

using namespace Vocal;


OpPbxEnd::OpPbxEnd()
{
}


OpPbxEnd::~OpPbxEnd()
{
}


const char* const OpPbxEnd::name() const
{
    return "OpPbxEnd";
}


const Sptr < State > OpPbxEnd::process( const Sptr < SipProxyEvent > event )
{
	cpDebug(LOG_DEBUG, "OpPbxEnd  operation" );

	Sptr < UaDeviceEvent > uaEvent;
	uaEvent.dynamicCast( event );
	if ( uaEvent == 0 )
	{
		cpDebug(LOG_DEBUG, "This is not a UaDeviceEvent process in OpPbxEnd");
		return 0;
	}
	if ( uaEvent->type != DeviceEventHookUp /*&&deviceEvent->type != DeviceEventFlash*/ )
	{
		return 0;
	}


	Sptr < UaCallInfo > call;
	call.dynamicCast( event->getCallInfo() );
	assert( call != 0 );

	int virtual_device_id = UaDevice::instance()->getPbxDeviceId(*(uaEvent->callId) );
	
	/* get audio info from virtual device and begin to transfer audio */
	AssistDevice *pbx = UaDevice::instance()->getAssistDevice(virtual_device_id);
	AssistDevice *hardware = UaDevice::instance()->getAssistDevice(uaEvent->getDeviceId() );
	assert(pbx!=0);
	assert(hardware!=0);
	hardware->setAudioStack(pbx->getAudioStack());
	
	/* remove or told virtual stop audio and return to usable status */
	pbx->setAudioStack(0);
	pbx->setCallId(0);
//	pbx->setStatus()

	/* notify PBX device to stop remote ringback and set status to free */
	Sptr < UaHardwareEvent > pbxSignal = new UaHardwareEvent( event->getFifo() );

	pbxSignal->setDeviceId( virtual_device_id );
	pbxSignal->type = HardwareTimeoutType;
//	pbxEvent->signalOrRequest.signal = DeviceSignalBusyStop;
	pbxSignal->signalOrRequest.signal = DeviceSignalRemoteRingbackStop;
	event->getFifo()->add(pbxSignal);

	/* remove the callinfo about virtual device and create a new callInfo */
	Sptr < UaCallContainer > calls;
	calls.dynamicCast( uaEvent->getCallContainer() );

	calls->replaceCallDeviceId(*(uaEvent->callId), virtual_device_id, uaEvent->getDeviceId() );
#if 0
	Sptr <UaCallInfo> oldCallInfo = calls->findCall(*(uaEvent->callId), virtual_device_id);

	Sptr <Contact> contact = oldCallInfo->getContact(  );
	if ( contact == 0 )
	{
		cpLog( LOG_ERR, "No contact data in old CallInfo " );
//		return ;
	}
#endif
	Sptr < UaStateMachine > mystateMachine;
	mystateMachine.dynamicCast( uaEvent->getCallInfo()->getFeature() );
	assert( mystateMachine != 0 );
#if 0
	calls->deleteCall( *(uaEvent->callId), virtual_device_id);
	
	Sptr <UaCallInfo> newCallInfo = calls->newCall( *(uaEvent->callId) , uaEvent->getDeviceId() );
	assert( newCallInfo != 0 );
	newCallInfo->setFeature( mystateMachine );
	newCallInfo->setContact( contact );
#endif
	/* put this new callinfo into StateInCall */
//	return call->setState( mystateMachine->findState( "StateInCall" ) );
	return  mystateMachine->findState( "StateInCall" ) ;
#if 0	
	Sptr < UaHardwareEvent > signal = new UaHardwareEvent( UaDevice::getDeviceQueue() );
	signal->type = HardwareSignalType;

	signal->setDeviceId( event->getDeviceId()/*event->getDeviceId() */);
	signal->signalOrRequest.signal = DeviceSignalRingStop;
	UaDevice::getDeviceQueue()->add( signal );
	return 0;
#endif
}

