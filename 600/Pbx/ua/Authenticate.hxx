#ifndef AUTHENTICATE_HXX_
#define AUTHENTICATE_HXX_
/*
 * $Id: Authenticate.hxx,v 1.2 2007/03/02 04:19:29 lizhijie Exp $
 */

#include "StatusMsg.hxx"
#include "SipCommand.hxx"
#include "Data.hxx"

namespace Vocal
{
    
bool authenticateMessage(const StatusMsg& status, 
                         SipCommand& command,
			 Data user,
			 Data password);
}

#endif
