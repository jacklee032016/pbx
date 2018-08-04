/*
* $Log: PresUrl.cxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:09  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: PresUrl.cxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/


#include "global.h"
#include <cassert>
#include "PresUrl.hxx"

using namespace Assist;

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
