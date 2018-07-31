#ifndef __BASE_AUTHENTICATION_HXX__
#define __BASE_AUTHENTICATION_HXX__

#include "Data.hxx"
#include "SipCommand.hxx"
#include "Sptr.hxx"
#include "StatusMsg.hxx"

using namespace Assist;

void addProxyAuthorization(const StatusMsg& errorMsg, SipCommand& cmdMsg, Data username, Data password);

void addWwwAuthorization(const StatusMsg& errorMsg, SipCommand& cmdMsg, Data username, Data password);
		      

#endif

