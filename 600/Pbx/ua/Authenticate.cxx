/*$Id: Authenticate.cxx,v 1.3 2007/03/06 18:56:20 lizhijie Exp $*/

#include "Authenticate.hxx"
#include "SipDigest.hxx"
#include "BaseAuthentication.hxx"

using namespace Vocal;

bool Vocal::authenticateMessage(const StatusMsg& status, 
                                SipCommand& command,
                                Data user,
                                Data password)
{
	// need to find out which kind of auth to do
	// should be a checkauth function or something

	SipWwwAuthenticate wwwAuth = status.getWwwAuthenticate();
	SipProxyAuthenticate proxyAuth = status.getProxyAuthenticate();

	if( wwwAuth.encode() != "")
	{// do something about the Www-Authenticate: header
		addWwwAuthorization(status, command, user, password);
		return true;
	}

	if( proxyAuth.encode() != "")
	{// do something about the Proxy-Authenticate: header
		addProxyAuthorization(status, command, user, password);
		return true;
	}

	// if it reaches here, I was unable to extract information for the
	// purposes of authenticating the message.  return false.
	return false;
}

