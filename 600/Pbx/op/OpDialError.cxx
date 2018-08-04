/*
 * $Id: OpDialError.cxx,v 1.2 2007/03/02 04:19:27 lizhijie Exp $
 */

#include "global.h"
#include "OpDialError.hxx"
#include "UaDigitTimerEvent.hxx"
#include "UaStateMachine.hxx"
#include "UaDevice.hxx"
#include "UaCallInfo.hxx"

#include "pbx_globals.h"

///
using namespace Vocal;


OpDialError::OpDialError()
{}


OpDialError::~OpDialError()
{}


///

const char* const
OpDialError::name() const
{
    return "OpDialError";
}


///
const Sptr < State >
OpDialError::process( const Sptr < SipProxyEvent > event )
{
    cpDebug(LOG_DEBUG, "OpDialError operation" );

    Sptr < UaDigitTimerEvent > timerEvent;
    timerEvent.dynamicCast( event );
    if ( timerEvent == 0 )
    {
        return 0;
    }
	
	// We won't enter an error state if we have a valid dial
	Sptr < DigitCollector > digitCollector = UaDevice::instance()->getDigitCollector(event->getDeviceId() );
	assert( digitCollector != 0 );

    if ( ( digitCollector->isValidDial() ) )
    {
        return 0;
    }

    cpDebug(LOG_DEBUG,
           "Invalid number %s",
           UaDevice::instance()->getDigitCollector(event->getDeviceId() )->getDial().c_str() );

    Sptr < UaStateMachine > stateMachine;
    stateMachine.dynamicCast( event->getCallInfo()->getFeature() );
    assert( stateMachine != 0 );

//modified by lijie 2005-06-03
#if 0
	if(event->getDeviceId() >= VIRTUAL_DEVICES_EPOCH)
		return stateMachine->findState( "StateBusy" );
	else
#endif		
	return stateMachine->findState( "StateError" );

}
