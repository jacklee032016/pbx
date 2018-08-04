/*
 * $Id: OpConsultXferring.cxx,v 1.2 2007/03/02 04:19:27 lizhijie Exp $
 */

#include "global.h"
#include "OpConsultXferring.hxx"


///
using namespace Vocal;


OpConsultXferring::OpConsultXferring()
{}


OpConsultXferring::~OpConsultXferring()
{}


///

const char* const
OpConsultXferring::name() const
{
    return "OpConsultXferring";
}


///
const Sptr < State >
OpConsultXferring::process( const Sptr < SipProxyEvent > event )
{
    cpDebug(LOG_DEBUG, "OpConsultXferring operation" );
}
