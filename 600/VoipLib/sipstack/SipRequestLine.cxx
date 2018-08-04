/*
 * $Id: SipRequestLine.cxx,v 1.2 2007/03/01 20:04:31 lizhijie Exp $
 */

#include "global.h"
#include "SipRequestLine.hxx"
#include "symbols.hxx"
#include "SystemInfo.hxx"
#include "cpLog.h"
#include "SipMethod.hxx"

using namespace Vocal;

#if 0
static const Data UnknownSipMethod(UNKNOWN_METHOD.getData());
static const Data DefaultSipVersion(DEFAULT_VERSION.getData());
static const Data DefaultSipProtocol(DEFAULT_PROTOCOL.getData());
#endif

string
SipRequestLineParserException::getName( void ) const
{
    return "SipRequestLineParserException";
}


SipRequestLine::SipRequestLine(Method amethod, Sptr <BaseUrl> url)
{
    //initialize the default port, and over write if present in the url.

    // set the default for protocol and version
    protocol = Data(DEFAULT_PROTOCOL);
    version = Data(DEFAULT_VERSION);

    requestUrl = url;
    method = methodToData(amethod);
}


SipRequestLine::SipRequestLine(const Data& data, Sptr<BaseUrl> url)
{
    method = data;
    requestUrl = url;
}
    
    
SipRequestLine::SipRequestLine(UrlType type)
        : method(UNKNOWN_METHOD),
        version(DEFAULT_VERSION),
        protocol(DEFAULT_PROTOCOL)
{
    urlType = type;
}


SipRequestLine::SipRequestLine(const SipRequestLine& reqLine)
    : requestUrl(duplicateUrl(reqLine.requestUrl)),
      urlType(reqLine.urlType),
      method(reqLine.method),
      version(reqLine.version),
      protocol(reqLine.protocol)
{
}


SipRequestLine::SipRequestLine( const Data& srcData )
{
    Data fdata = srcData;
    try
    {
        decode(fdata);
	if (requestUrl.getPtr() != 0)
	{
	    urlType = requestUrl->getType();  //to keep the two in sync.
	}
    }
    catch (SipRequestLineParserException&)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in Constructor of Requestline :( ");
            throw SipRequestLineParserException(
                "failed to decode the Requestline string :(",
                __FILE__,
                __LINE__, DECODE_REQUESTLINE_FAILED);
        }
    }
}


void
SipRequestLine::decode(const Data& data) throw (SipRequestLineParserException&)
{
    Data nData = data;

    try
    {
        scanSipRequestLine(nData);
    }
    catch (SipRequestLineParserException exception)
    {
        cpLog(LOG_ERR, "Failed to Decode in Constructor of Requestline :( ");
        throw SipRequestLineParserException(
            "failed to decode the RequestLine string",
            __FILE__,
            __LINE__, DECODE_REQUESTLINE_FAILED);
    }
}


void
SipRequestLine::scanSipRequestLine(const Data &tmpdata) 
    throw (SipRequestLineParserException&)
{
    Data reqdata = tmpdata;
    Data reqvalue;
    int ret = reqdata.match(" ", &reqvalue, true);
    if (ret == FOUND)
    {
        setMethod(reqvalue);
        Data reqsdata = reqdata;
        Data reqsvalue;
        int retn = reqsdata.match(" ", &reqsvalue, true);
        if (retn == FOUND)
        {
            try
            {
                if (requestUrl.getPtr() == 0)
		{
		    requestUrl = BaseUrl::decode(reqsvalue);
		}
		else
		{
		    requestUrl->decode(reqsvalue);
		}
            }
            catch (SipUrlParserException e)
            {
                cpLog(LOG_ERR, "Failed to decode URL of Requestline");
                throw SipRequestLineParserException(
                    "failed to decode the RequestLine URL",
                    __FILE__,
                    __LINE__, DECODE_REQUESTLINE_FAILED);
            }

            Data finaldata = reqsdata;
            Data finalvalue;
            parseVersion(finaldata);
        }
        else if (retn == NOT_FOUND)
        {
            cpLog(LOG_ERR, 
                  "Failed to Decode in scanSipRequestline() of Requestline");
            throw SipRequestLineParserException(
                "failed to decode the RequestLine string",
                __FILE__,
                __LINE__, DECODE_REQUESTLINE_FAILED);
        }
        else if (retn == FIRST)
        {
            cpLog(LOG_ERR, 
                  "Failed to Decode in scanSipRequestline() of Requestline :( ");
            throw SipRequestLineParserException(
                "failed to decode the RequestLine string",
                __FILE__,
                __LINE__, DECODE_REQUESTLINE_FAILED);
        }
    }
    else if (ret == NOT_FOUND)
    {
        cpLog(LOG_ERR, "Failed to Decode in scanSipRequestline() of Requestline :( ");
        throw SipRequestLineParserException(
            "failed to decode the RequestLine string",
            __FILE__,
            __LINE__, DECODE_REQUESTLINE_FAILED);
    }
    else if (ret == FIRST)
    {
        cpLog(LOG_ERR, "Failed to Decode in scanSipRequestline() of Requestline :( ");
        throw SipRequestLineParserException(
            "failed to decode the RequestLine string",
            __FILE__,
            __LINE__, DECODE_REQUESTLINE_FAILED);
    }
}


const SipRequestLine& 
SipRequestLine::operator=(const SipRequestLine& src)
{
    if (&src != this)
    {
        requestUrl = duplicateUrl(src.requestUrl);
        method = src.method;
        version = src.version;
        protocol = src.protocol;
    }

    return *this;
}


bool  
SipRequestLine::operator==(const SipRequestLine& src) const 
{
    bool equal = false;
    if ( (requestUrl.getPtr() != 0) && (src.requestUrl.getPtr() != 0) )
    {
	equal = ( requestUrl->areEqual(src.requestUrl));
    }
    else if ( (requestUrl.getPtr() == 0) && (src.requestUrl.getPtr() == 0) )
    {
	equal = true;
    }
    else
    {
	equal = false;
    }
    return  (equal && 
             (  method == src.method ) && 
             (  version == src.version ) && 
             (  protocol == src.protocol ) 
        );
}


Sptr <BaseUrl> 
SipRequestLine::getUrl() const
{
    return duplicateUrl(requestUrl, true);
}


void 
SipRequestLine::setUrl(Sptr <BaseUrl> url)
{
    requestUrl = duplicateUrl(url);
}


Data 
SipRequestLine::getMethod() const
{
    return method;
}


void 
SipRequestLine::setMethod(const Data& newmethod)
{
     method = newmethod;
}

    
Data 
SipRequestLine::encode() const
{
    Data siprequest;

    if (requestUrl.getPtr() != 0)
    {
	siprequest = method;
	siprequest += SP;
	siprequest += requestUrl->encode();
        if (requestUrl->isLooseRouterPresent())
        {
            siprequest += ";lr";
        }
	siprequest += SP;
	siprequest += protocol;
	siprequest += "/";
	siprequest += version;
	siprequest += CRLF;
    }
    return siprequest;
}


void 
SipRequestLine::setProtocol(const Data& newprotocol)
{
    protocol = newprotocol;

}


void 
SipRequestLine::parseVersion(const Data& newver)
{
    Data finaldata = newver;
    Data finvalue;
    int test = finaldata.match("/", &finvalue, true);
    if (test == FIRST)
    {
        //EXp
    }

    if (test == FOUND)
    {
        setVersion(finaldata);
        setProtocol(finvalue);
    }

    if (test == NOT_FOUND)
    {
        cpLog(LOG_ERR, "Failed to Decode version in Request line");
        throw SipRequestLineParserException(
            "failed to decode the RequestLine string",
            __FILE__,
            __LINE__, DECODE_REQUESTLINE_FAILED);
    }


}


void
SipRequestLine::setVersion(const Data& newversion)
{
    version = newversion;
}


Data 
SipRequestLine::getProtocol() const
{
    return protocol;
}


Data 
SipRequestLine::getVersion() const
{

    return version;
}


void 
SipRequestLine::setTransportParam(const Data& transport)
{
    //Valid values for transport "TCP" or "UDP" 
    assert((transport == "TCP") || (transport == "UDP"));
    Sptr<BaseUrl> bUrl = getUrl();
    Sptr<SipUrl> sUrl;
    sUrl.dynamicCast(bUrl);
    if(sUrl != 0)
    {
        if(transport == "TCP")
        {
            sUrl->setTransportParam("tcp");
        }
        else
        {   
            sUrl->setTransportParam("");
        }
        setUrl(bUrl);
    }
}

Data 
SipRequestLine::getTransportParam() const
{
    Data retVal;
    Sptr<BaseUrl> bUrl = getUrl();
    Sptr<SipUrl> sUrl;
    sUrl.dynamicCast(bUrl);
    if(sUrl != 0)
    {
        retVal = sUrl->getTransportParam();
    }
    return retVal;
}

Data
SipRequestLine::getHost() const
{
    Sptr<SipUrl> sUrl;
    sUrl.dynamicCast(requestUrl);
    assert(sUrl != 0);
    return sUrl->getHost();
}


void 
SipRequestLine::setHost(const Data& host)
{
    Sptr<SipUrl> sUrl;
    sUrl.dynamicCast(requestUrl);
    if(sUrl != 0)
    {
        sUrl->setHost(host);
    }
}


void 
SipRequestLine::setPort(const Data& port)
{
    Sptr<SipUrl> sUrl;
    sUrl.dynamicCast(requestUrl);
    if(sUrl != 0)
    {
        sUrl->setPort(port);
    }
}
