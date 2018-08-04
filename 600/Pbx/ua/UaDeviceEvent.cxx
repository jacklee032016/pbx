/*
 * $Id: UaDeviceEvent.cxx,v 1.2 2007/03/02 04:19:29 lizhijie Exp $
 */

#include "global.h"
#include <cassert>

#include "UaDeviceEvent.hxx"

using namespace Vocal;

///
UaDeviceEvent::UaDeviceEvent ( const Sptr < Fifo < Sptr < SipProxyEvent > > > outputFifo )
{
    assert( outputFifo != 0 );

    myFifo = outputFifo;	
    type = DeviceEventUndefined;
	/* added by lizhijie 2005.02.10 */
	myDeviceId = AS_DEVICE_NOT_DEFINED;
}


///
void
UaDeviceEvent::setSipStack( const Sptr < SipTransceiver > stack )
{
    assert( stack != 0 );
    sipStack = stack;
}


///
const Sptr < SipTransceiver >
UaDeviceEvent::getSipStack()
{
    assert( sipStack != 0 );
    return sipStack;
}


///
const char* const
UaDeviceEvent::name() const
{
    return "User Agent Device Event";
}


///
UaDeviceEvent::~UaDeviceEvent()
{}
