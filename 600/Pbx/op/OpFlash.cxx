/*
 * $Id: OpFlash.cxx,v 1.2 2007/03/02 04:19:27 lizhijie Exp $
 */

#include "global.h"
#include "OpFlash.hxx"


///
using namespace Vocal;


OpFlash::OpFlash()
{}


OpFlash::~OpFlash()
{}


///

const char* const
OpFlash::name() const
{
    return "OpFlash";
}


///
const Sptr < State >
OpFlash::process( const Sptr < SipProxyEvent > event )
{
    cpDebug(LOG_DEBUG, "OpFlash operation" );
}
