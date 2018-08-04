/*
 * $Id: SipRecordRoute.cxx,v 1.2 2007/03/01 20:04:31 lizhijie Exp $
 */

#include "global.h"
#include "SipRecordRoute.hxx"
#include "SipParserMode.hxx"
#include "SipUrl.hxx"
#include "symbols.hxx"
#include "cpLog.h"

using namespace Vocal;


string
SipRecordRouteParserException::getName( void ) const
{
    return "SipRecordRouteParserException";
}


SipRecordRoute::SipRecordRoute(UrlType uType) : url()
{
    urlType = uType;
}


SipRecordRoute::SipRecordRoute( const SipRecordRoute& src )
    : url(duplicateUrl(src.url)),
      urlType(src.urlType),
      displayname(src.displayname)
{
}


SipRecordRoute::SipRecordRoute(const Data& newData) : url()
{
    try
    {
        decode(newData);
	if (url.getPtr() != 0)
	{
	    urlType = url->getType();
	}
    }
    catch (SipRecordRouteParserException&)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in Constructor of Record ROute :( ");
            throw SipRecordRouteParserException(
                "failed to decode the Record-Route string :(",
                __FILE__,
                __LINE__, DECODE_RECORDROUTE_FAILED);
        }
    }
}


void
SipRecordRoute::decode( const Data& rrstr )
{

    try
    {
        parse(rrstr);
    }
    catch (SipRecordRouteParserException exception)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in decode() of Record ROute :( ");
            throw SipRecordRouteParserException(
                "failed to decode the Record-Route string :(",
                __FILE__,
                __LINE__, DECODE_RECORDROUTE_FAILED);
        }

    }
}


void
SipRecordRoute::parse( const Data & tmpdata)
{
    Data sipdata;
    Data data = tmpdata;
    int ret = data.match("<", &sipdata, true);
    if (ret == NOT_FOUND)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in Parse() of Record ROute :( ");
            throw SipRecordRouteParserException(
                "failed to decode the Record-Route string :(",
                __FILE__,
                __LINE__, DECODE_RECORDROUTE_FAILED);
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
SipRecordRoute::parseUrl(const Data & tmpurl)
{
    Data gdata = tmpurl;
    Data urlvalue;
    int retn = gdata.match(">", &urlvalue, true);
    if (retn == NOT_FOUND)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in ParseUrl() of Record ROute :( ");
            throw SipRecordRouteParserException(
                "failed to decode the Record-Route string :(",
                __FILE__,
                __LINE__, URL_PARSE_FAIL);
        }
    }
    else if (retn == FIRST)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in ParseUrl() of Record ROute :( ");
            throw SipRecordRouteParserException(
                "failed to decode the Record-Route string :(",
                __FILE__,
                __LINE__, URL_PARSE_FAIL);
        }

    }
    else if (retn == FOUND)
    {

        url = BaseUrl::decode(urlvalue);

    }

}


Sptr <BaseUrl> 
SipRecordRoute::getUrl() const
{
    return duplicateUrl(url,true);
}


void
SipRecordRoute::setUrl( Sptr <BaseUrl> newUrl )
{
    url = duplicateUrl(newUrl);
}

    
Data
SipRecordRoute::encode() const
{
    Data data;
    Data disname = getDisplayName();
    if (disname.length() > 0)
    {
        data += disname;
    }
    
    if (url.getPtr() != 0)
    {
	if (url->getType() == SIP_URL)
	{
	    Sptr <SipUrl> sipUrl;
	    sipUrl.dynamicCast(url);
    
	    data += "<";
	    sipUrl->encode();
	    Data name = sipUrl->getNameAddr();
	    data += name;

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
    
	    Data recRouteMaddr = sipUrl->getMaddrParam();
	    if ( recRouteMaddr.length() > 0)
	    {
		data += ";";
		data += "maddr=";
		data += recRouteMaddr;
	    }
    
            if (sipUrl->isLooseRouterPresent())
            {
                data += ";lr";
            }
            
	    data += ">";
	}
    }

    return data;
}


const SipRecordRoute& 
SipRecordRoute::operator=(const SipRecordRoute& src)
{
    if ( &src != this )
    {
	url = duplicateUrl(src.url);
        displayname = src.displayname;
    }
    return *this;
}


bool 
SipRecordRoute::operator == (const SipRecordRoute& src) const
{
    bool equal = false;
    
    if ( (url.getPtr() != 0) && (src.url.getPtr() != 0) )
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
    equal = (equal && 
	     (displayname == src.displayname)
	);
    return equal;
}


SipHeader*
SipRecordRoute::duplicate() const
{
    return new SipRecordRoute(*this);
}


bool
SipRecordRoute::compareSipHeader(SipHeader* msg) const
{
    SipRecordRoute* otherMsg = dynamic_cast<SipRecordRoute*>(msg);
    if(otherMsg != 0)
    {
	return (*this == *otherMsg);
    }
    else
    {
	return false;
    }
}
