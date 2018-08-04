/*
$Id: OpPbxDialOK.cxx,v 1.1.1.1 2006/11/29 09:50:50 lizhijie Exp $
 */

#include "global.h"
#include "OpPbxDialOK.hxx"

#include "UaDigitTimerEvent.hxx"
#include "DigitCollector.hxx"
#include "UaDevice.hxx"
#include "UaConfiguration.hxx"
#include "UaCallInfo.hxx"
#include "UaStateMachine.hxx"
#include "UaHardwareEvent.hxx"
#include "UaCallContainer.hxx"
#include "ResGwDevice.hxx"
#include "AssistDeviceInfo.hxx"

using namespace Vocal;

OpPbxDialOK::OpPbxDialOK()
{
}

OpPbxDialOK::~OpPbxDialOK()
{
}

const char* const OpPbxDialOK::name() const
{
	return "OpPbxDialOK";
}

const Sptr < State > OpPbxDialOK::process( const Sptr < SipProxyEvent > event )
{
	cpDebug(LOG_DEBUG, "OpPbxDialOK  operation" );

	Sptr < UaDigitTimerEvent > timerEvent;
	timerEvent.dynamicCast( event );
	if ( timerEvent == 0 )
	{
		return 0;
	}
//modified by lijie	2005-06-03
#if 0

	Sptr < DigitCollector > digitCollector =	UaDevice::instance()->getDigitCollector(event->getDeviceId() );
	assert( digitCollector != 0 );
	if ( !( digitCollector->isValidDial() ) )
	{
		return 0;
	}

#if 0
	if(digitCollector->getDialMethod() != INTERNAL_IP_DIAL)
	{
		cpDebug(LOG_DEBUG, "PBX dial error");
		return 0;
	}
#endif

	/* following code should be enhanced for mutex access */
	string callee = digitCollector->getDial();
	AssistDevice *dev = digitCollector->checkHardwareForLocalDial(callee);
	if( dev == 0)
		return 0;
	
	cpDebug(LOG_DEBUG, "Pbx Dialing complete" );
	cpDebug(LOG_DEBUG, "Dial internal branch phone number %s", callee.c_str() );
	cout << endl;

	/* check whether this hardware is busy */
	UaDevice::instance()->setCallId(UaDevice::instance()->getCallId(event->getDeviceId()) ,dev->getDeviceId() );

	/* first notify virtual pbx begin to play ringback tone on audioStack*/
	Sptr < UaHardwareEvent > signalToPbx = new UaHardwareEvent( UaDevice::getDeviceQueue() );
	signalToPbx->type = HardwareSignalType;
	signalToPbx->setDeviceId( event->getDeviceId() );
	signalToPbx->signalOrRequest.signal = DeviceSignalLocalRingbackStart;
	UaDevice::getDeviceQueue()->add( signalToPbx );

	/* second, notify hardware device ringing now */
	Sptr < UaHardwareEvent > signalToHardware = new UaHardwareEvent( UaDevice::getDeviceQueue() );
	signalToHardware->type = HardwareSignalType;
	signalToHardware->setDeviceId( dev->getDeviceId() );
	signalToHardware->signalOrRequest.signal = DeviceSignalRingStart;
	UaDevice::getDeviceQueue()->add( signalToHardware );


	Sptr < UaStateMachine > stateMachine;
	stateMachine.dynamicCast( event->getCallInfo()->getFeature() );
	assert( stateMachine != 0 );

	return stateMachine->findState( "StatePbxRinging" );
#else
	Sptr < DigitCollector > digitCollector = UaDevice::instance()->getDigitCollector(event->getDeviceId() );
	assert( digitCollector != 0 );

	
	if (!( digitCollector->isValidPbxDial() ) )
	{
		return 0;
	}

	Sptr < SipCallId > activeId;	
	Sptr < UaCallInfo > activeCall;
	Sptr < UaHardwareEvent > signal;
	// Get call container
	Sptr < UaCallContainer > calls;
	calls.dynamicCast( event->getCallContainer() );
	assert( calls != 0 );

	
	/* following code should be enhanced for mutex access */
	/*modify by wuaiwu, 2005/08/12 */
	string callee = digitCollector->getDial();
	if (DigitCollector::checkDialMethod(callee)==INTERNAL_IP_DIAL)
	{
		callee = UaConfiguration::instance()->getLocalCountry()+UaConfiguration::instance()->getLocalDistrict()+\
			UaConfiguration::instance()->getLocalDevice()+callee;
	}
	cpDebug(LOG_DEBUG, "Pbx Dialing complete" );
	cpDebug(LOG_DEBUG, "Dial internal branch phone number %s", callee.c_str() );
	//cout << endl;

	AssistDeviceInfo *dev =  (AssistDeviceInfo *)UaDevice::instance()->getAssistDeviceByPhoneNumber( callee.c_str(), *(timerEvent->callId));

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
			 UaDevice::instance()->addCallWaitingId( callId, dev->getDeviceId() );
			 break;
		case HARDWARE_AVAILABLE:
			UaDevice::instance()->setCallId( callId, dev->getDeviceId() );
			break;
		default:
			break;
			//assert(0);
	}

	Sptr < UaStateMachine > stateMachine;
	stateMachine.dynamicCast( event->getCallInfo()->getFeature() );
	assert( stateMachine != 0 );

	return stateMachine->findState( "StatePbxRinging");

#endif
}

