/*
 * $Id: SipRoute.cxx,v 1.2 2007/03/01 20:04:31 lizhijie Exp $
 */

#include "global.h"
#include "SipRecordRoute.hxx"
#include "SipRoute.hxx"
#include "SipParserMode.hxx"
#include "SipUrl.hxx"

using namespace Vocal;


string
SipRouteParserException::getName( void ) const
{
    return "SipRouteParserException";
}


SipRoute::SipRoute(UrlType uType)  : url()
{
    urlType = uType;
}


SipRoute::SipRoute( const Data& srcData ) : url()
{
    try
    {
        decode(srcData);
	if (url != 0)
	{
	    urlType = url->getType();
	}
    }
    catch (SipRouteParserException&)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in Constructor of ROute :( ");
            throw SipRouteParserException(
                "failed to decode the Route string :(",
                __FILE__,
                __LINE__, DECODE_ROUTE_FAILED);
        }


    }
}


SipRoute::SipRoute( const SipRoute& src )
        : url(duplicateUrl(src.url)), 
	  urlType(src.urlType),
	  displayname(src.displayname)
{
}

SipRoute::SipRoute( const SipRecordRoute& rr) : 
    url(duplicateUrl(rr.url)),
    urlType(rr.urlType),
    displayname(rr.displayname)
{
}


void
SipRoute::decode( const Data& rrstr )
{

    try
    {
        parse(rrstr);
    }
    catch (SipRouteParserException&)
    {

        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in Constructor of ROute :( ");
            throw SipRouteParserException(
                "failed to decode the Route string :(",
                __FILE__,
                __LINE__, DECODE_ROUTE_FAILED);
        }

    }
}


void
SipRoute::parse( const Data & tmpdata)
{
    Data sipdata;
    Data data = tmpdata;
    int ret = data.match("<", &sipdata, true);
    if (ret == NOT_FOUND)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in Parse() of Record ROute :( ");
            throw SipRouteParserException(
                "failed to parse the Record-Route string :(",
                __FILE__,
                __LINE__, DECODE_ROUTE_FAILED);
        }
    }
    else if (ret == FIRST)
    {
        // which is fine
        parseUrl(data);
    }
    else if (ret == FOUND)
    {
        // this is also fine becos name-addrs :dispaly-name <addr-spec>
        setDisplayName(sipdata);
        parseUrl(data);
    }
}


void
SipRoute::parseUrl(const Data & tmpurl)
{
    Data gdata = tmpurl;
    Data urlvalue;
    int retn = gdata.match(">", &urlvalue, true);
    if (retn == NOT_FOUND)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in ParseUrl() of  ROute :( ");
            throw SipRouteParserException(
                "failed to decode the Route string :(",
                __FILE__,
                __LINE__, ROUTE_URL_PARSE_FAIL);
        }
        //Exceptions
    }

    else if (retn == FIRST)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in ParseUrl() of  ROute :( ");
            throw SipRouteParserException(
                "failed to decode the Route string :(",
                __FILE__,
                __LINE__, ROUTE_URL_PARSE_FAIL);
        }
        //EXCEPTions
    }

    else if (retn == FOUND)
    {

        url = BaseUrl::decode(urlvalue);
    }

}


Sptr <BaseUrl> SipRoute::getUrl() const
{
    return duplicateUrl(url, true);
}


void SipRoute::setUrl( Sptr <BaseUrl> srcurl )
{
    url = duplicateUrl(srcurl);
}


Data SipRoute::encode() const
{

    Data data;
    if (url.getPtr() != 0)
    {
	if (url->getType() == SIP_URL)
	{
	    Sptr <SipUrl> sipUrl;
	    sipUrl.dynamicCast(url);
    
	    Data urlData = sipUrl->getNameAddr();

	    if ( (displayname.length()) || (urlData.length()) )
	    {
		data += displayname;
		
		data = "<";
		data += urlData;

#if 1
		Data transportParam = sipUrl->getTransportParam();
                if ((transportParam.length() > 0) &&
                    (transportParam == Data("tcp")))
                {
                    data += SEMICOLON;
                    data += SipUrlParamTransport;
                    data += transportParam;
                }
#endif

		Data maddr = sipUrl->getMaddrParam();
		if (maddr.length() > 0)
		{
		    data += ";";
		    data += "maddr=";
		    data += maddr;
		}
                if (sipUrl->isLooseRouterPresent())
                {
                    data += ";lr";
                }

		data += ">";
	    }
	}
    }
    return data;
}


const SipRoute& 
SipRoute::operator =(const SipRoute& src)
{
    if ( &src != this )
    {
	url = duplicateUrl(src.url);
	urlType = src.urlType;
        displayname = src.displayname;
    }

    return *this;
}


bool 
SipRoute::operator ==(const SipRoute& src) const
{
    bool equal = false;
    if ( (url.getPtr() != 0)  && (src.url.getPtr() != 0) )
    {
	equal = ( url->areEqual(src.url));
    }
    else if ( (url.getPtr() == 0) && (src.url.getPtr() == 0) )
    {
	equal = true;
    }
    else
    {
	equal = false;
    }
    return   ( equal  &&
	     (  displayname == src.displayname )
	     );
}


SipHeader*
SipRoute::duplicate() const
{
    return new SipRoute(*this);
}


bool
SipRoute::compareSipHeader(SipHeader* msg) const
{
    SipRoute* otherMsg = dynamic_cast<SipRoute*>(msg);
    if(otherMsg != 0)
    {
	return (*this == *otherMsg);
    }
    else
    {
	return false;
    }
}

ostream&
Vocal::operator<<(ostream& s, const SipRoute& route)
{
    s << route.encode();
    return s;
}
