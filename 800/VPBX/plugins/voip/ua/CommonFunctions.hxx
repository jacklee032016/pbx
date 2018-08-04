#ifndef __COMMON_FUNCTIONS_HXX__
#define __COMMON_FUNCTIONS_HXX__
/*
* $Id: CommonFunctions.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "Sptr.hxx"
#include "SipCommand.hxx"
#include "SystemInfo.hxx"

namespace Assist
{
void addSelfInVia(Sptr<SipCommand> cmd, int port=5060, const Data& transport="UDP");
}

#endif

