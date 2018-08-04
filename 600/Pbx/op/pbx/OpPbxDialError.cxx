/*
 * $Id: OpPbxDialError.cxx,v 1.2 2007/03/02 04:19:28 lizhijie Exp $
 */

#include "global.h"
#include "OpPbxDialError.hxx"
#include "UaDigitTimerEvent.hxx"
#include "UaStateMachine.hxx"
#include "UaDevice.hxx"
#include "UaCallInfo.hxx"
#include "AssistDeviceInfo.hxx"
#include "pbx_globals.h"
#include "ResGwDevice.hxx"
#include "UaConfiguration.hxx"
#include "UaCallContainer.hxx"

///
using namespace Vocal;


OpPbxDialError::OpPbxDialError()
{}

OpPbxDialError::~OpPbxDialError()
{}

const char* const OpPbxDialError::name() const
{
	return "OpPbxDialError";
}

/* only check the pbx dial rule, eg phone number about line and local dial , not include url */
const Sptr < State > OpPbxDialError::process( const Sptr < SipProxyEvent > event )
{
	cpDebug(LOG_DEBUG, "OpPbxDialError operation" );

	Sptr < UaDigitTimerEvent > timerEvent;
	timerEvent.dynamicCast( event );
//modified by lijie	2005-06-03
#if 0	
#if 0
	Sptr < UaDeviceEvent > deviceEvent;
	deviceEvent.dynamicCast( event );
	
	/* OpDialError should be enhanced here too */
	if ( deviceEvent == 0 && timerEvent == 0 )
	{
		return 0;
	}
#endif
	if ( timerEvent == 0 )
	{
		return 0;
	}
	
	Sptr < DigitCollector > digitCollector = UaDevice::instance()->getDigitCollector(event->getDeviceId() );
	assert( digitCollector != 0 );

	if ( ( digitCollector->isValidPbxDial() ) )
	{
		return 0;
	}

	cpDebug(LOG_DEBUG,
           "Invalid number %s for PBX branch call",
           UaDevice::instance()->getDigitCollector(event->getDeviceId() )->getDial().c_str() );

	Sptr < UaHardwareEvent > signal = new UaHardwareEvent( UaDevice::getDeviceQueue() );
	signal->type = HardwareSignalType;
	signal->setDeviceId( event->getDeviceId());
	signal->signalOrRequest.signal = DeviceSignalBusyStart;
	UaDevice::getDeviceQueue()->add( signal );

#if 0
	Sptr < UaStateMachine > stateMachine;
	stateMachine.dynamicCast( event->getCallInfo()->getFeature() );
	assert( stateMachine != 0 );
	
	return stateMachine->findState( "StateBusy" );
#endif
	return 0;
#else
	if ( timerEvent == 0 )
	{
		return 0;
	}
	
	Sptr < DigitCollector > digitCollector = UaDevice::instance()->getDigitCollector(event->getDeviceId() );
	assert( digitCollector != 0 );

	
	if (( digitCollector->isValidPbxDial() ) )
	{
		return 0;
	}

	Sptr < UaHardwareEvent > signal;
	Sptr < SipCallId > activeId;	
	Sptr < UaCallInfo > activeCall;
	// Get call container
	Sptr < UaCallContainer > calls;
	calls.dynamicCast( event->getCallContainer() );
	assert( calls != 0 );
	
	string localExchage = UaConfiguration::instance()->getLocalExchange();
	AssistDeviceInfo *dev =  (AssistDeviceInfo *)UaDevice::instance()->getAssistDeviceByPhoneNumber( localExchage.c_str(), *(timerEvent->callId));


	if(dev == 0)
	{
		signal = new UaHardwareEvent( UaDevice::getDeviceQueue() );
    		signal->type = HardwareSignalType;
		signal->setDeviceId(event->getDeviceId());
		signal->signalOrRequest.signal = DeviceSignalBusyStart;
     		UaDevice::getDeviceQueue()->add( signal );
		return 0;
	}


	Sptr < SipCallId > callId = new SipCallId( *(timerEvent->callId) );
	HardwareStatusType hdwStatus = UaDevice::instance()->getHardwareStatus(dev->getDeviceId());
	
	switch(hdwStatus)
	{	
		case HARDWARE_UNAVAILABLE:
	    		signal = new UaHardwareEvent( UaDevice::getDeviceQueue() );
    			signal->type = HardwareSignalType;
			signal->setDeviceId(event->getDeviceId());
			signal->signalOrRequest.signal = DeviceSignalBusyStart;
     			UaDevice::getDeviceQueue()->add( signal );
			return 0;
		case HARDWARE_CALLWAITING_ALLOWED:	
			activeId = UaDevice::instance()->getCallId( dev->getDeviceId() );
			/// allow call waiting only if the current call is talking to one party
			if ( activeId != 0 )
			{
				activeCall = calls->findCall(*activeId , dev->getDeviceId() );
				if ( activeCall != 0 )
				{
					if ( activeCall->getContactList()->size() > 1 )
					{
						signal = new UaHardwareEvent( UaDevice::getDeviceQueue() );
    						signal->type = HardwareSignalType;
						signal->setDeviceId(event->getDeviceId());
						signal->signalOrRequest.signal = DeviceSignalBusyStart;
     						UaDevice::getDeviceQueue()->add( signal );
						return 0;
					}
				}
			}
			else
			{
			 	UaDevice::instance()->addCallWaitingId( callId, dev->getDeviceId() );
			}
			 break;
		case HARDWARE_AVAILABLE:
			UaDevice::instance()->setCallId( callId, dev->getDeviceId() );
			break;
		default:
			assert(0);
	}

	Sptr < UaStateMachine > stateMachine;
	stateMachine.dynamicCast( event->getCallInfo()->getFeature() );
	assert( stateMachine != 0 );

	return stateMachine->findState( "StatePbxRinging");
#endif
}
