#ifndef BASE_AUTHENTICATION_HXX_
#define BASE_AUTHENTICATION_HXX_
/*
 * $Id: BaseAuthentication.hxx,v 1.2 2007/03/02 04:19:27 lizhijie Exp $
 */

#include "Data.hxx"
#include "SipCommand.hxx"
#include "Sptr.hxx"
#include "StatusMsg.hxx"

using namespace Vocal;

void
addProxyAuthorization(const StatusMsg& errorMsg,
		      SipCommand& cmdMsg,
		      Data username,
		      Data password);

void
addWwwAuthorization(const StatusMsg& errorMsg,
		    SipCommand& cmdMsg,
		    Data username,
		    Data password);
		      

#endif
