/*
* $Log: SipParserMode.cxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:09  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: SipParserMode.cxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#include "global.h"
#include "SipParserMode.hxx"

using namespace Assist;

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

