/*
* $Id: Authenticate.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/
#include "Authenticate.hxx"
#include "SipDigest.hxx"
#include "BaseAuthentication.hxx"

using namespace Assist;

bool Assist::authenticateMessage(const StatusMsg& status, SipCommand& command,
                                Data user, Data password)
{
	// need to find out which kind of auth to do
	// should be a checkauth function or something

	SipWwwAuthenticate wwwAuth = status.getWwwAuthenticate();
	SipProxyAuthenticate proxyAuth = status.getProxyAuthenticate();

	if( wwwAuth.encode() != "")
	{
		// do something about the Www-Authenticate: header
		addWwwAuthorization(status, command, user, password);
		return true;
	}

	if( proxyAuth.encode() != "")
	{
		// do something about the Proxy-Authenticate: header
		addProxyAuthorization(status, command, user, password);
		return true;
	}

	// if it reaches here, I was unable to extract information for the
	// purposes of authenticating the message.  return false.
	return false;
}

