/*
 * $Id: ImUrl.cxx,v 1.2 2007/03/01 20:04:31 lizhijie Exp $
 */

#include "ImUrl.hxx"

using namespace Vocal;

ImUrl::ImUrl(const ImUrl& src)
        :ImpBaseUrl(src)
{
}


ImUrl::ImUrl( const Data& data ) throw(ImpBaseUrlParserException&)
        :ImpBaseUrl(data, "im")
{
}

ImUrl::ImUrl()
        :ImpBaseUrl()
{
    schemeName = "im";
}


Sptr<BaseUrl> ImUrl::duplicate() const
{
    return new ImUrl(*this);
}
