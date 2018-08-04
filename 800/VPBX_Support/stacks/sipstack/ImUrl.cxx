/*
* $Log: ImUrl.cxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:09  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: ImUrl.cxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/


#include "ImUrl.hxx"

using namespace Assist;

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
