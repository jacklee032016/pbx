#include "BaseAuthentication.hxx"
#include "SipProxyAuthenticate.hxx"
#include "SipCommand.hxx"

using namespace Assist;

void
addAuthorization(const StatusMsg& errorMsg,
		 SipCommand& cmdMsg,
                 Data username,
		 Data password,
		 bool useProxyAuthenticate)
{

    /// xxx this should be cleaned up
    Data authScheme;
    Data nonce;
    Data realm;
    Data alg;
    Data opaque;
    Data qop;
//    Data user;

    if(!useProxyAuthenticate)
    {
	SipWwwAuthenticate proxyAuth = errorMsg.getWwwAuthenticate();
	
	authScheme = proxyAuth.getAuthScheme();
	nonce = proxyAuth.getTokenValue("nonce");
	realm = proxyAuth.getTokenValue("realm");
	alg = proxyAuth.getTokenValue("algorithm");
	opaque = proxyAuth.getTokenValue("opaque");
	qop = proxyAuth.getTokenValue("qop");
//	user = proxyAuth.getTokenValue("username");
    }
    else
    {
	SipProxyAuthenticate proxyAuth = errorMsg.getProxyAuthenticate();
	
	authScheme = proxyAuth.getAuthScheme();
	nonce = proxyAuth.getTokenValue("nonce");
	realm = proxyAuth.getTokenValue("realm");
	alg = proxyAuth.getTokenValue("algorithm");
	opaque = proxyAuth.getTokenValue("opaque");
	qop = proxyAuth.getTokenValue("qop");
//	user = proxyAuth.getTokenValue("username");
    }

    cpDebug(LOG_DEBUG, " Scheme is : %s", authScheme.logData());

    if ( authScheme == AUTH_DIGEST )
    {
	Data method = cmdMsg.getRequestLine().getMethod();
    
	Sptr< BaseUrl > baseUrl = cmdMsg.getRequestLine().getUrl();
	assert( baseUrl != 0 );
    
	Data reqUri = baseUrl->encode();
    
	Data noncecount;
	Data cnonce;
    
	cpDebug(LOG_DEBUG, "qop: %s", qop.logData() );
	if (qop.length())
	{
	    //cnonce, and noncecount are SHOULD
	    noncecount = "00000001";
	    cnonce = opaque;
            LocalScopeAllocator lo;
	    unsigned int pos = string(qop.getData(lo)).find("auth");
	
	    if  (pos  != string::npos)
	    {
		qop = "auth";
		//the client can also find if this has a auth-int, and
		//if it wants to support it, can set qop to auth-int.
	    }
	    else
	    {
		qop = "";
	    }
	}
    
	cpDebug(LOG_DEBUG, "nonce: %s", nonce.logData() );
	cpDebug(LOG_DEBUG, "realm: %s", realm.logData() );
	cpDebug(LOG_DEBUG, "username: %s", username.logData() );
	cpDebug(LOG_DEBUG, "password: %s", password.logData() );
	cpDebug(LOG_DEBUG, "method: %s", method.logData() );
	cpDebug(LOG_DEBUG, "reqUri: %s", reqUri.logData() );
	cpDebug(LOG_DEBUG, "qop: %s", qop.logData() );
	cpDebug(LOG_DEBUG, "cnonce: %s", cnonce.logData() );
	cpDebug(LOG_DEBUG, "alg: %s", alg.logData() );
	cpDebug(LOG_DEBUG, "noncecount: %s", noncecount.logData() );
	cpDebug(LOG_DEBUG, "opaque: %s", opaque.logData() );
    
	if(!useProxyAuthenticate)
	{
	    cmdMsg.setAuthDigest(nonce, username, password, method, 
                                 realm, reqUri, qop, cnonce, alg,
                                 noncecount, opaque);
	}
	else
	{
	    cmdMsg.setProxyAuthDigest(nonce, username, password, method, 
				      realm, reqUri, qop, cnonce, alg,
				      noncecount, opaque);
	}
    }
    else if (authScheme == AUTH_BASIC)
    {
	cmdMsg.setAuthBasic(username, password);
    }
}


void
addProxyAuthorization(const StatusMsg& errorMsg,
		      SipCommand& cmdMsg,
                      Data username,
		      Data password)
{
    addAuthorization(errorMsg, cmdMsg, username, password, true);
}


void
addWwwAuthorization(const StatusMsg& errorMsg,
		    SipCommand& cmdMsg,
                    Data username,
		    Data password)
{
    addAuthorization(errorMsg, cmdMsg, username, password, false);
}
