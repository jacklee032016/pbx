/*
 * $Id: OpOnHook.cxx,v 1.3 2007/03/07 18:24:08 lizhijie Exp $
 */

#include "global.h"
#include "OpOnHook.hxx"
#include "UaDeviceEvent.hxx"
#include "UaDevice.hxx"
#include "UaStateMachine.hxx"
#include "UaCallContainer.hxx"
#include "UaConfiguration.hxx"
#include "ResGwDevice.hxx"
#if 0
#include "UaCommandLine.hxx"  
#endif

using namespace Vocal;

OpOnHook::OpOnHook()
{}

OpOnHook::~OpOnHook()
{}


const char* const OpOnHook::name() const
{
	return "OpOnHook";
}


const Sptr <State> OpOnHook::process( const Sptr <SipProxyEvent> event )
{
	cpDebug(LOG_DEBUG, "OpOnHook operation" );

	Sptr <UaDeviceEvent> deviceEvent;
	deviceEvent.dynamicCast( event );
	if ( deviceEvent == 0 )
	{
		return 0;
	}
	if ( deviceEvent->type != DeviceEventHookDown )
	{
		return 0;
	}
	
	cpDebug(LOG_DEBUG, "OnHook" );


	/* added for busy alert in the case of hookup be far end, lizhijie */
	Sptr < UaHardwareEvent > signal = new UaHardwareEvent( UaDevice::getDeviceQueue() );
	signal->setDeviceId( event->getDeviceId() );
	signal->type = HardwareSignalType;
	signal->signalOrRequest.signal = DeviceSignalBusyStop;
	UaDevice::getDeviceQueue()->add( signal );

	Sptr < UaStateMachine > stateMachine;
	stateMachine.dynamicCast( event->getCallInfo()->getFeature() );
	assert( stateMachine != 0 );

	// Remove call info from call container
	Sptr < UaCallContainer > calls;
	cpDebug(LOG_DEBUG, "OnHook" );
	calls.dynamicCast( event->getCallContainer() );
	cpDebug(LOG_DEBUG, "OnHook" );
	assert( calls != 0 );

	calls->deleteCall( *(UaDevice::instance()->getCallId( event->getDeviceId() )) ,event->getDeviceId());

	// Clear call id in device
	UaDevice::instance()->setCallId( 0 ,event->getDeviceId() );

	// release rtp port
	UaDevice::instance()->releaseRtpPort( event->getDeviceId() );

	// if there is a call on call waiting, send that call an onhook evnet
	Sptr < SipCallId > call2Id = UaDevice::instance()->getCallWaitingId( event->getDeviceId() );
	if ( call2Id != 0 )
	{
		//if ( UaConfiguration::instance()->getCallWaitingOn() )
		if ( UaDevice::instance()->getCallWaitingOn(event->getDeviceId()) )
		{
			// send a message to the call on call waiting, notifying
			// it that the active call is completed
			Sptr < UaCallContainer > calls;
			calls.dynamicCast( event->getCallContainer() );
			assert( calls != 0 );

			Sptr < UaCallInfo > call2 = calls->findCall( *call2Id ,event->getDeviceId() );

			if ( call2 == 0 )
			{
			    cpLog( LOG_ERR, "No call info found for the call on call waiting");
			    return 0;
			}
			Sptr < Fifo < Sptr < SipProxyEvent > > > eventQ = deviceEvent->getFifo();
			Sptr < UaDeviceEvent > signal = new UaDeviceEvent( eventQ );
			signal->type = DeviceEventHookDown;
			//signal->callId = call2Id;

			//added by lijie 2005-06-03
			UaDevice::instance()->setCallId( call2Id ,event->getDeviceId() );	

			signal->setDeviceId( event->getDeviceId() );

			eventQ->add( signal );
		}
	}

#if 0	
	if ( !UaCommandLine::instance()->getBoolOpt( "voicemail" ) )
#endif		
	{
	//cout << endl << "Ready" << endl;
	}

	return stateMachine->findState( "StateIdle" );
}

