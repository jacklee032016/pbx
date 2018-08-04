/*
 * $Id: OpCancelTimer.cxx,v 1.3 2007/03/07 18:24:08 lizhijie Exp $
 */

#include "global.h"
#include "OpCancelTimer.hxx"


///
using namespace Vocal;


using namespace Vocal;


OpCancelTimer::OpCancelTimer()
{}


OpCancelTimer::~OpCancelTimer()
{}


///

const char* const
OpCancelTimer::name() const
{
    return "OpCancelTimer";
}


///
const Sptr < State >
OpCancelTimer::process( const Sptr < SipProxyEvent > event )
{
    cpDebug(LOG_DEBUG, "OpCancelTimer operation" );

    cancelTimer(event);
    return 0;
}

