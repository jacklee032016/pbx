/*
 * $Id: UaDigitTimerEvent.cxx,v 1.2 2007/03/02 04:19:29 lizhijie Exp $
 */

#include "global.h"
#include <cassert>

#include "UaDigitTimerEvent.hxx"

using namespace Vocal;

///
UaDigitTimerEvent::UaDigitTimerEvent ( const Sptr < Fifo < Sptr < SipProxyEvent > > > outputFifo )
{
    assert( outputFifo != 0 );

    myFifo = outputFifo;
	/* added by lizhijie 2005.02.10 */
	myDeviceId = AS_DEVICE_NOT_DEFINED;
}


///
void
UaDigitTimerEvent::setSipStack( const Sptr < SipTransceiver > stack )
{
    assert( stack != 0 );
    sipStack = stack;
}


///
const Sptr < SipTransceiver >
UaDigitTimerEvent::getSipStack()
{
    assert( sipStack != 0 );
    return sipStack;
}


///
const char* const
UaDigitTimerEvent::name() const
{
    return "Digit Timer Event";
}

///
void
UaDigitTimerEvent::setLoadGenSignalType( LoadGenSigType signal)
{
    lgSignalType = signal;
}

///
LoadGenSigType
UaDigitTimerEvent::getLoadGenSigType()
{
    return lgSignalType;
}


///
UaDigitTimerEvent::~UaDigitTimerEvent()
{}
