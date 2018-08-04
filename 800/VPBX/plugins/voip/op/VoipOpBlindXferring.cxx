
#include "global.h"
#include "OpBlindXferring.hxx"


const Sptr < State > VoipOpBlindXferring::process( const Sptr < SipProxyEvent > event )
{
    cpDebug(LOG_DEBUG, "OpBlindXferring operation" );

    // Optional: Check if it is an OnHook event

    // Send TRANSFER message

    return 0;
}

