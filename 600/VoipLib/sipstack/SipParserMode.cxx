/*
 * $Id: SipParserMode.cxx,v 1.2 2007/03/01 20:04:31 lizhijie Exp $
 */

#include "global.h"
#include "SipParserMode.hxx"

using namespace Vocal;

SipParserMode * SipParserMode::instance_ = 0;

SipParserMode::SipParserMode()
{
    sipParserMode_ = true;
#if 0
    if (getenv("SIP_PARSER_STRICT_MODE"))
    {
        sipParserMode_ = true;
    }
    else
    {
        sipParserMode_ = false;
    }
#endif
}
