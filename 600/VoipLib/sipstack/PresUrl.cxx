/*
 * $Id: PresUrl.cxx,v 1.2 2007/03/01 20:04:31 lizhijie Exp $
 */

#include "global.h"
#include <cassert>
#include "PresUrl.hxx"

using namespace Vocal;

PresUrl::PresUrl(const PresUrl& src)
        :ImpBaseUrl(src)
{
}


PresUrl::PresUrl( const Data& data ) throw(ImpBaseUrlParserException&)
        :ImpBaseUrl(data, "pres")
{
}

PresUrl::PresUrl()
        :ImpBaseUrl()
{
    schemeName = "pres";
}


Sptr<BaseUrl> PresUrl::duplicate() const
{
    return new PresUrl(*this);
}
