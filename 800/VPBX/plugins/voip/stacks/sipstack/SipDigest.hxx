#ifndef SIPDIGEST_H
#define SIPDIGEST_H

/*
* $Log: SipDigest.hxx,v $
* Revision 1.1.1.1  2006/11/30 16:26:29  lizhijie
* AS800 VPBX
*
* Revision 1.1  2006/06/03 12:06:38  lizhijie
* no message
*
* $Id: SipDigest.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "digest.hxx"
#include "Data.hxx"


namespace Assist
{

///
class SipDigest
{
    public:
        ///
        SipDigest();
        ///
        ~SipDigest();
        ///
        Data form_SIPdigest(const Data& nonce, const Data& user,
                            const Data& pwd, const Data& method,
                            const Data& requestURI,
                            const Data& realm,
                            const Data& qop,
                            const Data& cnonce,
                            const Data& alg,
                            const Data& noncecount);
};
 
} 

#endif
