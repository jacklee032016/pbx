
#include "global.h"
#include <cpLog.h>
#include <InviteMsg.hxx>
#include <SipCSeq.hxx>

#include "OpConvertCw.hxx"
#include "UaDeviceEvent.hxx"
#include "SipContext.hxx"
#include "UaStateMachine.hxx"
#include "UaDevice.hxx"
#include "PbxConfiguration.hxx"
#include "UaCallContainer.hxx"

using namespace Assist;

const Sptr < State > VoipOpConvertCw::process( const Sptr < SipProxyEvent > event )
{
    cpDebug(LOG_DEBUG, "OpConvertCw operation" );

    Sptr < UaDeviceEvent > deviceEvent;
    deviceEvent.dynamicCast( event );
    if ( deviceEvent == 0 )
    {
        return 0;
    }

    if ( deviceEvent->type != DeviceEventFlash )
    {
        return 0;
    }

    Sptr < SipCallId > call2Id = UaDevice::instance()->getCallWaitingId(event->getDeviceId());
    if ( call2Id == 0 )
    {
        // no call on call waiting
        return 0;
    }

    // if there is a call on call waiting, flashhook triggers ua to
    // put current call on hold and answer the call on call waiting
   // if ( PbxConfiguration::instance()->getCallWaitingOn() )
    if ( UaDevice::instance()->getCallWaitingOn(event->getDeviceId()) )
    {
        // send a message to the call on call waiting, notifying
        // it that the call has been picked up
        Sptr < UaCallContainer > calls;
        calls.dynamicCast( event->getCallContainer() );
        assert( calls != 0 );

		Sptr < SipContext > call2 = calls->findCall( *call2Id , event->getDeviceId() );
		
        if ( call2 == 0 )
        {
            cpLog( LOG_ERR, "No call info found for the call on call waiting");
            cpLog( LOG_ERR, "Flash hook ignored" );
            return 0;
        }

	Sptr < Fifo < Sptr < SipProxyEvent > > > eventQ = deviceEvent->getFifo();
	Sptr < UaDeviceEvent > signal = new UaDeviceEvent( eventQ );
	signal->type = DeviceEventFlash;
	//removed by lijie 2005-06-03	
	//signal->callId = call2Id;
	signal->setDeviceId( event->getDeviceId()) ;
	eventQ->add( signal );

	// Remove call info from call container
	calls->deleteCall( *(UaDevice::instance()->getCallId( event->getDeviceId() )) ,event->getDeviceId());

	//modified by lijie 2005-06-03
	// Clear call id in device
	//UaDevice::instance()->setCallId( 0 ,event->getDeviceId() );
	UaDevice::instance()->setCallId( call2Id ,event->getDeviceId() );
    }
    else
    {
        // if call waiting feature off, ignore flashhook
        // this should not happen since the second call can not
        // proceed this far if calling waiting feature is off
        return 0;
    }

    cpDebug(LOG_DEBUG, "activate call on call waiting" );

    Sptr < UaStateMachine > stateMachine;
    stateMachine.dynamicCast( event->getCallInfo()->getFeature() );
    assert( stateMachine != 0 );

    return stateMachine->findState( "StateIdle" );
}
