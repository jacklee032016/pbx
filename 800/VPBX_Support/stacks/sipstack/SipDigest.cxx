/*
* $Log: SipDigest.cxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:09  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: SipDigest.cxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#include "global.h"
#include "SipDigest.hxx"
#include "cpLog.h"
#include "support.hxx"

#define HASHLEN 16

using namespace Assist;

SipDigest::SipDigest()
{
}


SipDigest::~SipDigest()
{
}


Data 
SipDigest::form_SIPdigest( const Data& nonce, 
                           const Data& user,
			   const Data& pwd, 
                           const Data& method,
			   const Data& requestURI,
			   const Data& realm,
			   const Data& qop,
			   const Data& cnonce,
			   const Data& alg,
			   const Data& noncecount)
{
    HASHHEX HA1;
    HASHHEX HA2 = "";
    HASHHEX response;

    char algstr[1024];
    char userstr[1024];
    char realmstr[1024];
    char pwdstr[1024];
    char noncestr[1024];
    char cnoncestr[1024];
    char noncecountstr[1024];
    char qopstr[1024];
    char methodstr[1024];
    char reqURIstr[1024];

    alg.getData(algstr,1024);
    user.getData(userstr,1024);
    realm.getData(realmstr,1024);
    pwd.getData(pwdstr,1024);
    nonce.getData(noncestr,1024);
    cnonce.getData(cnoncestr,1024);
    noncecount.getData(noncecountstr,1024);
    qop.getData(qopstr,1024);
    method.getData(methodstr,1024);
    requestURI.getData(reqURIstr,1024);

    DigestCalcHA1(algstr, userstr, realmstr, pwdstr, noncestr, cnoncestr, HA1);
    DigestCalcResponse(HA1, 
                       noncestr, 
                       noncecountstr, 
                       cnoncestr, 
                       qopstr, 
                       methodstr, 
                       reqURIstr, 
                       HA2, 
                       response);

    cpDebug(LOG_DEBUG_STACK, "Response = %s\n", response);

    return response;
}

