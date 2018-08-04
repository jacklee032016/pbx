/*
* $Log: SipAlso.cxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:09  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: SipAlso.cxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#include "global.h"
#include "SipAlso.hxx"
#include "symbols.hxx"
#include "SipParserMode.hxx"
#include "BaseUrl.hxx"
#include "Sptr.hxx"
#include "SipUrl.hxx"
    
using namespace Assist;

string
SipAlsoParserException::getName( void ) const
{
    return "SipAlsoParserException";
}

SipAlso::SipAlso(UrlType uType)
    :
    alsoUrl(),
    displayName()

{
    urlType = uType;
}


SipAlso::SipAlso(const SipAlso& src)
    :
    alsoUrl(duplicateUrl(src.alsoUrl)),
    urlType(src.urlType),
    displayName(src.displayName)
{
}


SipAlso::SipAlso( Sptr <BaseUrl>  url)
    :
    alsoUrl(duplicateUrl(url)),
    displayName()
{
}


SipAlso::SipAlso( const Data& data )
        :
        alsoUrl(),
        displayName()
{


    try
    {
        decode(data);
	if (alsoUrl.getPtr() != 0)
	{
	    urlType = alsoUrl->getType();
	}
    }
    catch (SipAlsoParserException&)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in Constructor :o( ");
            throw SipAlsoParserException("failed in Decode", __FILE__, __LINE__, DECODE_ALSO_FAILED);

        }

    }
}



void
SipAlso::scanSipAlso( const Data & tmpdata)
{
    Data sipdata;
    Data data = tmpdata;
    int ret = data.match("<", &sipdata, true);
    if (ret == NOT_FOUND)
    {
        Data value;
        Data tmpval;
        int retn = data.match("\n", &value, true) ;
        if (retn == NOT_FOUND)
        {
            if (SipParserMode::sipParserMode())
            {
                cpLog(LOG_ERR, "Failed to Decode in Constructor :o( ");
                throw SipAlsoParserException("failed in Decode", __FILE__, __LINE__, DECODE_ALSO_FAILED);

            }

        }
        else if (retn == FIRST)
        {
            if (SipParserMode::sipParserMode())
            {
                cpLog(LOG_ERR, "Failed to Decode in Constructor :o( ");
                throw SipAlsoParserException("failed in Decode", __FILE__, __LINE__, DECODE_ALSO_FAILED);

            }

        }
        else if (retn == FOUND)
        {
            alsoUrl = BaseUrl::decode(value);
        }

    }
    else if (ret == FIRST)
    {
        try
        {
            parseUrl(data);
        }
        catch (SipAlsoParserException&)
        {
            if (SipParserMode::sipParserMode())
            {
                cpLog(LOG_ERR, "Failed to Decode in Scan SIPTO :o( ");
                throw SipAlsoParserException("failed in Decode in scan SipAlso", __FILE__, __LINE__, PARSE_AURL_FAILED);

            }

        }

    }

    else if (ret == FOUND)
    {
        parseNameInfo(sipdata);
        Data urlvalue;
        parseUrl(data);
    }
}




void
SipAlso::parseUrl(const Data& data)
{
    Data urlvalue = data;
    Data avalue;
    int retur = urlvalue.match(">", &avalue, true);
    if (retur == NOT_FOUND)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Parse in ParseUrl() :o( ");
            throw SipAlsoParserException("failed in Decode", __FILE__, __LINE__, PARSE_AURL_FAILED);

        }


    }
    else if (retur == FIRST)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Parse in ParseUrl()  :o( ");
            throw SipAlsoParserException("failed in Decode", __FILE__, __LINE__, PARSE_AURL_FAILED);

        }

    }
    else if (retur == FOUND)
    {
        alsoUrl = BaseUrl::decode(avalue);
    }
}






void
SipAlso::parseNameInfo(const Data& data)
{
    Data newnameinfo;
    Data nameinfo = data;
    int ret = nameinfo.match(":", &newnameinfo, true);
    Data newnameinf;
    ret = nameinfo.match(" ", &newnameinf, true);
    setDisplayName(nameinfo);
}







void
SipAlso::parse( const Data& alsodata)
{

    Data data = alsodata;

    try
    {

        scanSipAlso(data);
    }
    catch (SipAlsoParserException exception)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in Parse :o( ");
            throw SipAlsoParserException("failed in Decode", __FILE__, __LINE__, DECODE_ALSO_FAILED);

        }

    }


    //everything allright.


}


void
SipAlso::decode( const Data& alsostr )
{

    try
    {
        parse(alsostr);
    }
    catch (SipAlsoParserException exception)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in decode() :o( ");
            throw SipAlsoParserException("failed in Decode", __FILE__, __LINE__, DECODE_ALSO_FAILED);

        }



    }
}


SipAlso& SipAlso::operator = (const SipAlso& srcAlso)
{
    if ( &srcAlso != this )
    {
	alsoUrl = duplicateUrl(srcAlso.alsoUrl);
	urlType = srcAlso.urlType;
        displayName = srcAlso.displayName;
    }
    return (*this);
}


bool SipAlso::operator ==(const SipAlso& srcAlso) const
{
    cpDebug(LOG_DEBUG_STACK, "SipAlso ::operator ==");

    bool equal = false;
    if (alsoUrl->areEqual(srcAlso.alsoUrl))
    {
        equal = true;
    }
    if (equal == false)
    {
        cpDebug(LOG_DEBUG_STACK, " url match failed" );

	if (getUrl().getPtr() != 0)
	{
	    cpDebug(LOG_DEBUG_STACK, "this.url = %s" , getUrl()->encode().logData());
	}
#ifdef __linux__
	if (srcAlso.getUrl().getPtr() != 0)
	{
	    cpDebug(LOG_DEBUG_STACK, "other.url = %s" ,
		  srcAlso.getUrl()->encode().logData() );
	}
#endif
    }
    return equal;
}

void SipAlso::setDisplayName(const Data& name)
{
    displayName = name;
}

Data SipAlso::getDisplayName() const
{
    return displayName;
}

void SipAlso::setHost(const Data& newhost)
{
    if (urlType == TEL_URL)
    {
	return;
    }
    if (alsoUrl.getPtr() == 0)
    {
	alsoUrl = new SipUrl();
    }
    if (alsoUrl.getPtr() != 0)
    {
    	if (alsoUrl->getType() == SIP_URL)
	{
	    Sptr<SipUrl> sipUrl;
	    sipUrl.dynamicCast(alsoUrl);
	    sipUrl->setHost(newhost);
	}
    } 
}

Data SipAlso::getHost() const
{
    Data host;
    if (alsoUrl.getPtr() != 0)
    {
    	if (alsoUrl->getType() == SIP_URL)
	{
	    Sptr<SipUrl> sipUrl;
	    sipUrl.dynamicCast(alsoUrl);
	    host = sipUrl->getHost();
	}
    } 
    return host;
}


Data SipAlso::encode() const
{
    Data sipalso;

    sipalso = "Also:";
    sipalso += SP;

    if (displayName.length() > 0)
    {
        sipalso += displayName;
    }

    if (alsoUrl.getPtr() != 0)
    {
	alsoUrl->encode();
	    
	//get before the url param.
	Data nameaddr;
	
	if (alsoUrl->getType() == SIP_URL)
	{
	    Sptr<SipUrl> sipUrl;
	    sipUrl.dynamicCast(alsoUrl);
	    
	    sipalso += "<";
	    
	    nameaddr = sipUrl->getNameAddr();
	    
	    Data test = nameaddr;
	    
	    sipalso += nameaddr;
	    
	    Data userparam;
	    userparam = sipUrl->getUrlParam();
	    
	    
	    if (userparam.length())
	    {
		sipalso += userparam;
	    }
	    
	    sipalso += ">";
	}
    }
	
    sipalso += CRLF;
    
    return sipalso;
}
   
    
///
Sptr <BaseUrl> 
SipAlso::getUrl() const
{
    return duplicateUrl(alsoUrl, true);
}

///
void SipAlso::setUrl(Sptr <BaseUrl> alsourl)
{
    alsoUrl = duplicateUrl(alsourl);
}


SipHeader*
SipAlso::duplicate() const
{
    return new SipAlso(*this);
}


bool
SipAlso::compareSipHeader(SipHeader* msg) const
{
    SipAlso* otherMsg = dynamic_cast<SipAlso*>(msg);
    if(otherMsg != 0)
    {
	return (*this == *otherMsg);
    }
    else
    {
	return false;
    }
}
