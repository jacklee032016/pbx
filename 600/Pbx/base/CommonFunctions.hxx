#ifndef COMMONFUNCTIONS_HXX_
#define COMMONFUNCTIONS_HXX_
/*
 * $Id: CommonFunctions.hxx,v 1.3 2007/03/03 18:39:27 lizhijie Exp $
 */

#include "Sptr.hxx"
#include "SipCommand.hxx"
#include "SystemInfo.hxx"

namespace Vocal
{

void addSelfInVia(Sptr<SipCommand> cmd, 
		  int port=5060, 
		  const Data& transport="UDP");
}

#endif

