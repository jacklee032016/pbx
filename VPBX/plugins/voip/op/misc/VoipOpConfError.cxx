#include "global.h"
#include "OpConfError.hxx"

using namespace Assist;

const Sptr < State > VoipOpConfError::process( const Sptr < SipProxyEvent > event )
{
    cpDebug(LOG_DEBUG, "OpConfError operation" );
    return 0;
}

