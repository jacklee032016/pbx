/*
 * $Id: OpStartTimer.cxx,v 1.2 2007/03/02 04:19:28 lizhijie Exp $
 */

#include "global.h"
#include "OpStartTimer.hxx"
#include "UaTimerEvent.hxx"
#include "UaConfiguration.hxx"

///
using namespace Vocal;


OpStartTimer::OpStartTimer()
{}


OpStartTimer::~OpStartTimer()
{}


///

const char* const
OpStartTimer::name() const
{
    return "OpStartTimer";
}


///
const Sptr < State >
OpStartTimer::process( const Sptr < SipProxyEvent > event )
{
    cpDebug(LOG_DEBUG, "OpStartTimer operation; device Id is %d" , event->getDeviceId() );

    if ( ! setTimer( event, UaConfiguration::instance()->getUaTimeout() ) )
    {
        cpDebug(LOG_DEBUG, "Ua Timer not started" );
    }
    else
        cpDebug(LOG_DEBUG, "Ua Timer started" );

    return 0;
}

