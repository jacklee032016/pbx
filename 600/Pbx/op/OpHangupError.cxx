/*
 * $Id: OpHangupError.cxx,v 1.2 2007/03/02 04:19:27 lizhijie Exp $
 */

#include "global.h"
#include "OpHangupError.hxx"


///
using namespace Vocal;


OpHangupError::OpHangupError()
{}


OpHangupError::~OpHangupError()
{}


///

const char* const
OpHangupError::name() const
{
    return "OpHangupError";
}


///
const Sptr < State >
OpHangupError::process( const Sptr < SipProxyEvent > event )
{
    cpDebug(LOG_DEBUG, "OpHangupError operation" );
    // Should we remove this operator if it is doing nothing?
    return 0;
}
