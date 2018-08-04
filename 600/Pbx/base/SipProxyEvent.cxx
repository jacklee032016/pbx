/*
 * $Id: SipProxyEvent.cxx,v 1.2 2007/03/02 04:19:27 lizhijie Exp $
 */

#include "global.h"
#include "SipProxyEvent.hxx"
#include "CallContainer.hxx"
#include "SipTransceiver.hxx"


using namespace Vocal;

SipProxyEvent::SipProxyEvent()
    :	myFifo(0),
	myId(0),
	myCallInfo(0),
	myCallContainer(0),
        mySipStack(0),
        myDeviceId(AS_DEVICE_NOT_DEFINED)
{
}


SipProxyEvent::SipProxyEvent(Sptr < Fifo < Sptr < SipProxyEvent > > > fifo)
   :   myFifo(fifo),
       myId(0),
       myCallInfo(0),
       myCallContainer(0),
        mySipStack(0),
        myDeviceId(AS_DEVICE_NOT_DEFINED)
{
}


SipProxyEvent::~SipProxyEvent()
{
}


void
SipProxyEvent::postEvent(const Sptr < SipProxyEvent > newEvent)
{
    assert( newEvent != 0 );
    assert( myFifo != 0 );

    myFifo->add(newEvent);
}



void
SipProxyEvent::postEvent( 
    const Sptr < SipProxyEvent > newEvent,
    const Sptr < Fifo < Sptr < SipProxyEvent > > > newFifo ) const
{
    assert( newEvent != 0 );
    assert( newFifo != 0 );

    newFifo->add(newEvent);
}


void
SipProxyEvent::setSipStack( const Sptr < SipTransceiver > sipStack )
{
    assert( sipStack != 0 );

    mySipStack = sipStack;
}


const Sptr < SipTransceiver >
SipProxyEvent::getSipStack()
{
    assert( mySipStack != 0 );

    return ( mySipStack );
}



void
SipProxyEvent::setCallInfo( 
    const Sptr < CallInfo > callInfo,
    const Sptr < CallContainer > callContainer )
{
    assert( callInfo != 0 );
    assert( callContainer != 0 );

    myCallInfo = callInfo;
    myCallContainer = callContainer;
}



Sptr < CallInfo >
SipProxyEvent::getCallInfo( ) const
{
    return ( myCallInfo );
}



void
SipProxyEvent::removeCallInfo()
{
    myCallInfo = 0;
    myCallContainer = 0;
}



Sptr < Fifo < Sptr < SipProxyEvent > > >
SipProxyEvent::getFifo( ) const
{
    return ( myFifo );
}


Sptr < CallContainer >
SipProxyEvent::getCallContainer() const
{
    return ( myCallContainer );
}

/* added by lizhijie 2005.02.10 */
int SipProxyEvent::getDeviceId( ) const
{
	return ( myDeviceId);
}

void SipProxyEvent::setDeviceId( int _deviceId )
{
	myDeviceId = _deviceId;
}
/* end of added */

