/*
 * $Id: OpConfError.cxx,v 1.2 2007/03/02 04:19:27 lizhijie Exp $
 */

#include "global.h"
#include "OpConfError.hxx"


///
using namespace Vocal;


OpConfError::OpConfError()
{}


OpConfError::~OpConfError()
{}


///

const char* const
OpConfError::name() const
{
    return "OpConfError";
}


///
const Sptr < State >
OpConfError::process( const Sptr < SipProxyEvent > event )
{
    cpDebug(LOG_DEBUG, "OpConfError operation" );
    return 0;
}
