#ifndef __AUTHENTICATE_HXX__
#define __AUTHENTICATE_HXX__
/*
* $Id: Authenticate.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "StatusMsg.hxx"
#include "SipCommand.hxx"
#include "Data.hxx"

namespace Assist
{

bool authenticateMessage(const StatusMsg& status, SipCommand& command, Data user, Data password);

}

#endif

