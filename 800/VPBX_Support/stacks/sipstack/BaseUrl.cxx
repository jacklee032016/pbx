/*
* $Log: BaseUrl.cxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:09  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: BaseUrl.cxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#include "global.h"
#include "BaseUrl.hxx"
#include "symbols.hxx"
#include "cpLog.h"
#include "Data.hxx"
#include "SipUrl.hxx"
#include "TelUrl.hxx"
#include "ImUrl.hxx"
#include "PresUrl.hxx"
#include "NullUrl.hxx"
#include "UnknownUrl.hxx"

using namespace Assist;

BaseUrl::BaseUrl()
{
}
    
    
BaseUrl::~BaseUrl()
{
}
    
Sptr<BaseUrl>
BaseUrl::decode(const Data& data)
{
    Sptr <BaseUrl> newUrl;
    //parse the first line here.if this is sip, invoke SipUrl::decode,
    //else if tel, invoke TelUrl::decode.
    //return the created object.
    Data urlType;
    Data myData = data;
    
    int ret = myData.match(":", &urlType, false);
    if ( (ret == NOT_FOUND) || (ret == FIRST) )
    {
	cpDebug(LOG_DEBUG, "parse failed: could not find ':' separating URL type from body");
        cpDebug(LOG_DEBUG, "bad data in BaseUrl::decode: <%s>", data.logData());
        newUrl = new UnknownUrl(data);
    }
    else if ( isEqualNoCase(urlType, "sip") )
    {

	newUrl = new SipUrl(data);
    }
    else if ( isEqualNoCase(urlType, "tel") )
    {
	newUrl = new TelUrl(data);
    }
    else if ( isEqualNoCase(urlType, "im") )
    {
	newUrl = new ImUrl(data);
    }
    else if ( isEqualNoCase(urlType, "pres") )
    {
	newUrl = new PresUrl(data);
    }
    else      
    {
	cpDebug(LOG_DEBUG, "parse failed: scheme not SIP or TEL: %s", data.logData());
//	newUrl = new NullUrl;
        newUrl = new UnknownUrl(data);
    }
    return newUrl;
}
    

#if 0
bool
operator==(const BaseUrl& left, const BaseUrl& right)
{
    if(left.getType() == right.getType())
    {
	
    }
}
#endif

ostream& 
BaseUrl::dump(ostream& s) const
{
    s << encode();
    return s;
}


ostream&
Assist::operator<<(ostream& s, const Sptr<BaseUrl>& url)
{
    if (url == 0)
    {
        s << "NULL";
        return s;
    }
    else
    {
        return url->dump(s);
    }
}


Sptr<BaseUrl> 
Assist::duplicateUrl(const Sptr<BaseUrl> original, bool returnNullUrl)
{
    static Sptr<BaseUrl> null = new NullUrl;

    if(original != 0 && original->getType() != NULL_URL)
    {
        return original->duplicate();
    }
    else
    {
	if(returnNullUrl)
	{
	    return null;
	}
	else
	{
	    return 0;
	}
    }
}
