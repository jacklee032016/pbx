#ifndef SIPDIGEST_H
#define SIPDIGEST_H
/*
 * $Id: SipDigest.hxx,v 1.2 2007/03/01 20:04:28 lizhijie Exp $
 */

#include "digest.hxx"
#include "Data.hxx"


namespace Vocal
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
 
} // namespace Vocal

#endif
