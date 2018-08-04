/*
 * $Id: SipReferTo.cxx,v 1.2 2007/03/01 20:04:31 lizhijie Exp $
 */

#include "global.h"
#include "SipParserMode.hxx"
#include "SipReferTo.hxx"
#include "symbols.hxx"
#include "SipUrl.hxx"
#include "BaseUrl.hxx"
#include "Sptr.hxx"

using namespace Vocal;

string
SipReferToParserException::getName( void ) const
{
    return "SipReferToParserException";
}

SipReferTo::SipReferTo()
        :
        urlType(),
        address(),
        displayName(),
        afterAngleQuote(),
        angleQuotes(false)
{
}


SipReferTo::SipReferTo(const SipReferTo& src)
        :
        urlType(src.urlType),
        address(src.address),
        displayName(src.displayName),
        afterAngleQuote(src.afterAngleQuote),
        angleQuotes(src.angleQuotes)
{
}


SipReferTo::SipReferTo( const Data& data )
        :
        urlType(),
        address(),
        displayName(),
        afterAngleQuote(),
        angleQuotes(false)
{
    decode(data);
/*
    try
    {
        decode(data);
    }
    catch (SipReferToParserException&)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decodein Constructor :o( ");
            throw SipReferToParserException("failed in Decode", __FILE__, __LINE__, DECODE_REFERTO_FAILED);
        }
    }
*/
}


SipReferTo::SipReferTo(Sptr <BaseUrl> url)
{
    if (url.getPtr() != 0)
    {
        if (url->getType() == SIP_URL)
	{
	    Sptr <SipUrl> sipUrl;
	    sipUrl.dynamicCast(url);
	    Data x = sipUrl->getNameAddr();
	    Data sipdata;
	    int ret = x.match(":", &sipdata, true);
	    if ( ret == FOUND)
	    {
		urlType = sipdata;
		address = x;
	    }
	}
    }
}

void
SipReferTo::scanSipReferTo( const Data & tmpdata)
{
    Data sipdata;
    Data data;
    Data sdata = tmpdata;
    int ret;
    ret = sdata.match("<", &displayName, true);
    
    if (ret == FOUND || ret == FIRST)
    {
        angleQuotes = true;
    }
    ret = sdata.match(":", &sipdata, true);
    if (ret == NOT_FOUND)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in Constructor :o( ");
            throw SipReferToParserException("failed in Decode", __FILE__, __LINE__, DECODE_REFERTO_FAILED);
        }
    }
    else if (ret == FOUND)
    {
        Data urldata;
        urlType = sipdata;
        int retn = sdata.match("//", &urldata, true);
        if (retn == FIRST)
        {
            Data urData;
            if (angleQuotes)
            {
                if (sdata.match(">", &address, true) == FOUND)
                {
                    if (sdata.match(CRLF, &urData, true) == FOUND)
                    {
                        afterAngleQuote = urData;
                    }
                    else
                    {
                        afterAngleQuote = sdata;
                    }
                }
                else
                {
                    throw SipReferToParserException("failed in Decode in scan SipAlso", __FILE__, __LINE__, PARSE_REFERURL_FAILED);
                }
            }
            else
            {
                int retl = sdata.match(CRLF, &urData, true);
                if ( retl == FOUND)
                {
                    address = urData;
                }
                else if ( retl == NOT_FOUND)
                {
                    address = sdata;
                }
            }
        }
        else
        {
            sipdata.removeSpaces();
            if ( sipdata == "sip")
            {
                Data urData;
                if (angleQuotes)
                {
                    if (sdata.match(">", &address, true) == FOUND)
                    {
                        if (sdata.match(CRLF, &urData, true) == FOUND)
                        {
                            afterAngleQuote = urData;
                        }
                        else
                        {
                            afterAngleQuote = sdata;
                        }
                    }
                    else
                    {
                        throw SipReferToParserException("failed in Decode in scan SipAlso", __FILE__, __LINE__, PARSE_REFERURL_FAILED);
                    }
                }
                else
                {
                    int retl = sdata.match(CRLF, &urData, true);
                    if ( retl == FOUND)
                    {
                        address = urData;
                    }
                    else if ( retl == NOT_FOUND)
                    {
                        address = sdata;
                    }
                }
            }
            else
            {
                if (SipParserMode::sipParserMode())
                {
                    cpLog(LOG_ERR, "Failed to Decode in Scan SIPTO :o( ");
                    throw SipReferToParserException("failed in Decode in scan SipAlso", __FILE__, __LINE__, PARSE_REFERURL_FAILED);
                }
            }
        }
    }
    else if (ret == FIRST)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in Scan SIPTO :o( ");
            throw SipReferToParserException("failed in Decode in scan SipAlso", __FILE__, __LINE__, PARSE_REFERURL_FAILED);
        }
    }
}


void
SipReferTo::parse( const Data& refertodata)
{
    Data data = refertodata;
    scanSipReferTo(data);
/*
    try
    {
        scanSipReferTo(data);
    }
    catch (SipReferToParserException exception)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in Parse :o( ");
            throw SipReferToParserException("failed in Decode", __FILE__, __LINE__, DECODE_REFERTO_FAILED);
        }
    }
*/
}

void
SipReferTo::decode( const Data& alsostr )
{
    parse(alsostr);
/*
    try
    {
        parse(alsostr);
    }
    catch (SipReferToParserException exception)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in decode() :o( ");
            throw SipReferToParserException("failed in Decode", __FILE__, __LINE__, DECODE_REFERTO_FAILED);
        }
    }
*/
}


SipReferTo& SipReferTo::operator = (const SipReferTo& srcReferTo)
{
    if ( &srcReferTo != this )
    {
        urlType = srcReferTo.urlType;
        address = srcReferTo.address;
        displayName = srcReferTo.displayName;
        afterAngleQuote = srcReferTo.afterAngleQuote;
        angleQuotes = srcReferTo.angleQuotes;
    }
    return (*this);
}


bool SipReferTo::operator ==(const SipReferTo& srcReferTo) const
{
    cpDebug(LOG_DEBUG_STACK, "SipAlso ::operator ==");

    bool equal = false;
    if (urlType == srcReferTo.urlType)
    {
        equal = true;

        if ( address != srcReferTo.address)
        {
            equal = false;
        }
        
        if ( afterAngleQuote != srcReferTo.afterAngleQuote)
        {
            equal = false;
        }
    }
    if (equal == false)
    {
        cpDebug(LOG_DEBUG_STACK, " url match failed" );

    }
    return equal;
}

Data SipReferTo::getUrlString() const
{
    if (( urlType.length() == 0) && ( address.length() == 0))
    {
        Data temp;
        return temp;
    }
    Data sipalso;

    sipalso += urlType;
    if ( urlType == "sip")
    {
        sipalso += ":";
    }
    else
    {
        sipalso += "://";
    }
    sipalso += address;
    return sipalso;
}    


Data SipReferTo::encode() const
{
    if (( urlType.length() == 0) && ( address.length() == 0))
    {
        Data temp;
        return temp;
    }
    Data sipalso;

    sipalso = REFER_TO;
    sipalso += ":";
    sipalso += SP;
    
    if (angleQuotes)
    {
        sipalso += displayName;
        sipalso += "<";
    }

    sipalso += urlType;
    if ( urlType == "sip")
    {
        sipalso += ":";
    }
    else
    {
        sipalso += "://";
    }
    sipalso += address;
    if (angleQuotes)
    {
        sipalso += ">";
        sipalso += afterAngleQuote;
    }
    
    sipalso += CRLF;

    return sipalso;
}


///

Data SipReferTo::getUrlType() const
{
    return urlType;
}








SipHeader*
SipReferTo::duplicate() const
{
    return new SipReferTo(*this);
}


bool
SipReferTo::compareSipHeader(SipHeader* msg) const
{
    SipReferTo* otherMsg = dynamic_cast<SipReferTo*>(msg);
    if(otherMsg != 0)
    {
	return (*this == *otherMsg);
    }
    else
    {
	return false;
    }
}
