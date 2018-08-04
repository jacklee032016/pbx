/*
 * $Id: OpBlindXferred.cxx,v 1.2 2007/03/02 04:19:27 lizhijie Exp $
 */

#include "global.h"
#include <SipEvent.hxx>
#include <ByeMsg.hxx>
#include <StatusMsg.hxx>
#include <TransferMsg.hxx>

#include "OpBlindXferred.hxx"
#include "UaHardwareEvent.hxx"
#include "UaDevice.hxx"
#include "UaCallInfo.hxx"

///
using namespace Vocal;


OpBlindXferred::OpBlindXferred()
{}


OpBlindXferred::~OpBlindXferred()
{}


///

const char* const
OpBlindXferred::name() const
{
    return "OpBlindXferred";
}


///
const Sptr < State >
OpBlindXferred::process( const Sptr < SipProxyEvent > event )
{
    cpDebug(LOG_DEBUG, "OpBlindXferred operation" );

    Sptr < SipEvent > sipEvent;
    sipEvent.dynamicCast( event );
    if ( sipEvent == 0 )
    {
        return 0;
    }
    Sptr < SipMsg > sipMsg = sipEvent->getSipMsg();
    assert( sipMsg != 0 );

    Sptr < UaCallInfo > call;
    call.dynamicCast( event->getCallInfo() );
    assert( call != 0 );


    //check for 180/183 message
    Sptr < StatusMsg > msg;
    msg.dynamicCast( sipMsg );
    if ( msg == 0 )
    {
        return 0;
    }

    Sptr < Contact > contact = call->getContact();
    assert( contact != 0 );

    if ( msg->getStatusLine().getStatusCode() != 180 &&
            msg->getStatusLine().getStatusCode() != 183 )
    {
        return 0;
    }

    cpDebug(LOG_DEBUG, "Blind Xferred" );
    contact->update( *msg );

    //send the 200 for transfer msg
    Sptr < TransferMsg > xferMsg = call->getXferMsg();

    // hack to work around extra 180 from Marshal
    //assert( xferMsg != 0 );
    if ( xferMsg == 0 )
    {
        cpLog( LOG_ERR, "Extra 180 from Marshal, ignore!" );
        return 0;
    }

    StatusMsg status(*xferMsg, 200/*OK*/);
    sipEvent->getSipStack()->sendReply( status );
    call->setXferMsg(0);

    //turn on ringback
    Sptr < UaHardwareEvent > signal = new UaHardwareEvent( UaDevice::getDeviceQueue() );
    signal->type = HardwareSignalType;
    signal->signalOrRequest.signal = DeviceSignalLocalRingbackStart;
    UaDevice::getDeviceQueue()->add( signal );

    return 0;
}
