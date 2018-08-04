/*
 * $Id: OpBlindXferring.cxx,v 1.2 2007/03/02 04:19:27 lizhijie Exp $
 */

#include "global.h"
#include "OpBlindXferring.hxx"


///
using namespace Vocal;


OpBlindXferring::OpBlindXferring()
{}


OpBlindXferring::~OpBlindXferring()
{}


///

const char* const
OpBlindXferring::name() const
{
    return "OpBlindXferring";
}


///
const Sptr < State >
OpBlindXferring::process( const Sptr < SipProxyEvent > event )
{
    cpDebug(LOG_DEBUG, "OpBlindXferring operation" );

    // Optional: Check if it is an OnHook event

    // Send TRANSFER message

    return 0;
}
