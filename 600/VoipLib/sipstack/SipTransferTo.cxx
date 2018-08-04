/*
 * $Id: SipTransferTo.cxx,v 1.2 2007/03/01 20:04:32 lizhijie Exp $
 */

#include "global.h"
#include "SipTransferTo.hxx"
#include "symbols.hxx"
#include "SipUrl.hxx"
#include "TelUrl.hxx"
    
using namespace Vocal;


#define NOT_FOUND -1
#define FIRST -2
#define FOUND 0

string
SipTransferToParserException::getName( void ) const
{
    return "SipTransferToParserException";
}

SipTransferTo::SipTransferTo(UrlType uType)
    :
    toUrl(),
    displayName()
{
    urlType = uType;
}


SipTransferTo::SipTransferTo(const SipTransferTo& src)
    :
    toUrl(duplicateUrl(src.toUrl)),
    urlType(src.urlType),
    displayName(src.displayName)
{
    if (toUrl.getPtr() != 0)
    {
	if (toUrl->getType() == SIP_URL)
	{
	    Sptr <SipUrl> sipUrl;
	    
	    (sipUrl.dynamicCast(toUrl))->initializeTo();
	}
    }
}


SipTransferTo::SipTransferTo( Sptr <BaseUrl> url)
        :
        toUrl(duplicateUrl(url)),
        displayName()
{
    if (toUrl.getPtr() != 0)
    {
	urlType = toUrl->getType();
    
	if (toUrl->getType() == SIP_URL)
	{
	    Sptr <SipUrl> sipUrl;
	    (sipUrl.dynamicCast(url))->initializeTo();
	}
    }
}


SipTransferTo::SipTransferTo( const Data& data )
        :
        toUrl(),
        displayName()
{
    try
    {
        decode(data);
	if (toUrl.getPtr() != 0)
	{
	    urlType = toUrl->getType();
    
	    if (toUrl->getType() == SIP_URL)
	    {
		Sptr <SipUrl> sipUrl;
		
		(sipUrl.dynamicCast(toUrl))->initializeTo();
	    }
	}
    }
    catch (SipTransferToParserException&)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in Constructor :o( ");
            throw SipTransferToParserException("failed in Decode", __FILE__, __LINE__, DECODE_TRANSFER_TO_FAILED);

        }

    }
}


void
SipTransferTo::scanSipTransferTo( const Data & tmpdata)
{
    Data sipdata;
    Data data = tmpdata;
    int ret = data.match("<", &sipdata, true);
    if (ret == NOT_FOUND)
    {   //it can be URL?
        // check  for the URL with Addrs-params

        if (SipParserMode::sipParserMode())
        {
            Data tmpval;
            int retn = data.match("sip", &tmpval, false);
            if (retn == NOT_FOUND)
            {
                if (SipParserMode::sipParserMode())
                {
                    cpLog(LOG_ERR, "Failed to Decode in Constructor :o( ");
                    throw SipTransferToParserException("failed in Decode", __FILE__, __LINE__, DECODE_TRANSFER_TO_FAILED);

                }

            }
            else if (retn == FIRST)
            {
                if (SipParserMode::sipParserMode())
                {
                    cpLog(LOG_ERR, "Failed to Decode in Constructor :o( ");
                    throw SipTransferToParserException("failed in Decode", __FILE__, __LINE__, DECODE_TRANSFER_TO_FAILED);

                }

            }
            else if (retn == FOUND)
            { // if found then it has the Addrs-params

                toUrl = BaseUrl::decode(data);

            }
        }
        else
        {
            toUrl = BaseUrl::decode(data);
        }
    }
    else if (ret == FIRST)
    {
        sipdata = data;
        Data sivalue;
        parseUrl(sipdata);
    }
    else if (ret == FOUND)
    {
        parseNameInfo(sipdata);
        parseUrl(data);

    }
}


void
SipTransferTo::parseUrl(const Data& data)
{
    Data urlvalue = data;
    Data avalue;
    int retur = urlvalue.match(">", &avalue, true);
    if (retur == NOT_FOUND)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Parse in ParseUrl() :o( ");
            throw SipTransferToParserException("failed in Decode", __FILE__, __LINE__, PARSE_TRANSFER_URL_FAILED);

        }


    }
    else if (retur == FIRST)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Parse in ParseUrl()  :o( ");
            throw SipTransferToParserException("failed in Decode", __FILE__, __LINE__, PARSE_TRANSFER_URL_FAILED);

        }

    }
    else if (retur == FOUND)
    {
        toUrl = BaseUrl::decode(avalue);

    }
}


void
SipTransferTo::parseNameInfo(const Data& data)
{
    Data nameinfo = data;
    setDisplayName(nameinfo);
}


void
SipTransferTo::parse( const Data& todata)
{

    Data data = todata;

    try
    {

        scanSipTransferTo(data);
    }
    catch (SipTransferToParserException exception)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in Parse :o( ");
            throw SipTransferToParserException("failed in Decode", __FILE__, __LINE__, DECODE_TRANSFER_TO_FAILED);

        }

    }
    //everything allright.
}


void
SipTransferTo::decode( const Data& tostr )
{

    try
    {
        parse(tostr);
    }
    catch (SipTransferToParserException exception)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in decode() :o( ");
            throw SipTransferToParserException("failed in Decode", __FILE__, __LINE__, DECODE_TRANSFER_TO_FAILED);

        }



    }
}


SipTransferTo&
SipTransferTo::operator = (const SipTransferTo& srcTransferTo)
{
    if ( &srcTransferTo != this)
    {
        toUrl = duplicateUrl(srcTransferTo.toUrl);
	urlType = srcTransferTo.urlType;
        displayName = srcTransferTo.displayName;
    }
    return (*this);

}


bool 
SipTransferTo::operator ==(const SipTransferTo& srcTransferTo) const
{
    cpDebug(LOG_DEBUG_STACK, "SipTransferTo == operator");
    bool equal = false;
    equal = (toUrl->areEqual(srcTransferTo.toUrl));

    cpDebug(LOG_DEBUG_STACK, "SipTransferTo == operator returning : %d", equal);

    return equal;
}


void 
SipTransferTo::setPortData(const Data& newport)
{
    if (urlType == TEL_URL)
    {
	return;
    }
    if (toUrl.getPtr() == 0)
    {
	toUrl = new SipUrl();
    }
    if (toUrl.getPtr() != 0)
    {
	if (toUrl->getType() == SIP_URL)
	{
	    Sptr<SipUrl> sipUrl;
	    (sipUrl.dynamicCast(toUrl))->setPort(newport);
	}
    }

}


Data
SipTransferTo::getPortData()
{
    Data port;
    
    if (toUrl.getPtr() != 0)
    {
	if (toUrl->getType() == SIP_URL)
	{
	    Sptr<SipUrl> sipUrl;
	    port = (sipUrl.dynamicCast(toUrl))->getPort();
	}
    }
    return port;
}


int
SipTransferTo::getPort()
{
    
    int p = getPortData().convertInt();
    return p;
}


void 
SipTransferTo::setUser(const Data& newuser)
{
    if (toUrl.getPtr() == 0)
    {
	if (urlType == SIP_URL)
	{
	    toUrl = new SipUrl();
	}
	else if (urlType == TEL_URL)
	{
	    toUrl = new TelUrl();
	}
    }
    
    toUrl->setUserValue(newuser);

    //display name defaulted to user.
    if (displayName.length() == 0)
    {
        displayName = newuser;
    }
}


Data 
SipTransferTo::getUser()
{
    if(toUrl != 0)
    {
        return toUrl->getUserValue();
    }
    else
    {
        return "";
    }
}


void 
SipTransferTo::setDisplayName(const Data& name)
{
    displayName = name;
}


Data 
SipTransferTo::getDisplayName()
{
    return displayName;
}


void 
SipTransferTo::setHost(const Data& newhost)
{
    
    if (urlType == TEL_URL)
    {
	return;
    }
    if (toUrl.getPtr() == 0)
    {
	toUrl = new SipUrl();
    }
    if (toUrl.getPtr() != 0)
    {
	if (toUrl->getType() == SIP_URL)
	{
	    Sptr<SipUrl> sipUrl;
	    (sipUrl.dynamicCast(toUrl))->setHost(newhost);
	}
    }
}

    
Data 
SipTransferTo::getHost()
{
    Data host;
    if (toUrl.getPtr() != 0)
    {
	if (toUrl->getType() == SIP_URL)
	{
	    Sptr<SipUrl> sipUrl;
	    host = (sipUrl.dynamicCast(toUrl))->getHost();
	}
    }
    return host;
}


Data 
SipTransferTo::encode() const
{
    Data sipTransferTo;

    sipTransferTo = TRANSFER_TO;
    sipTransferTo += SP;

    if (displayName.length() > 0)
    {
        sipTransferTo += displayName;
    }

    if (toUrl.getPtr() != 0)
    {
	if (toUrl->getType() == SIP_URL)
	{
	    Sptr <SipUrl> sipUrl;
	    sipUrl.dynamicCast(toUrl);
    
	    sipTransferTo += "<";

	    sipUrl->encode();

	    //get before the url param.
	    Data nameaddr;
	    nameaddr = sipUrl->getNameAddr();

	    sipTransferTo += nameaddr;

	    Data userparam;
	    userparam = sipUrl->getUrlParam();

	    if (userparam.length())
	    {
		sipTransferTo += userparam;
	    }
	    sipTransferTo += ">";


	}
    }
    sipTransferTo += CRLF;
    
    return sipTransferTo;
}


Sptr <BaseUrl> 
SipTransferTo::getUrl()
{
    return duplicateUrl(toUrl, true);
}


void 
SipTransferTo::setUrl(Sptr <BaseUrl>  tourl)
{
    toUrl = duplicateUrl(tourl);
    
    if (toUrl.getPtr() != 0)
    {
	if (toUrl->getType() == SIP_URL)
	{
	    Sptr <SipUrl> sipUrl;
    
	    (sipUrl.dynamicCast(toUrl))->initializeTo();
	}
    }
}


SipHeader*
SipTransferTo::duplicate() const
{
    return new SipTransferTo(*this);
}


bool
SipTransferTo::compareSipHeader(SipHeader* msg) const
{
    SipTransferTo* otherMsg = dynamic_cast<SipTransferTo*>(msg);
    if(otherMsg != 0)
    {
	return (*this == *otherMsg);
    }
    else
    {
	return false;
    }
}
