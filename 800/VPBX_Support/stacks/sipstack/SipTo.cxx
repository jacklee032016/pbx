/*
* $Log: SipTo.cxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:10  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: SipTo.cxx,v 1.1.1.1 2006/11/30 16:27:10 lizhijie Exp $
*/

#include "global.h"
#include "SipTo.hxx"
#include "symbols.hxx"
#include "SipUrl.hxx"
#include "TelUrl.hxx"
#include "NullUrl.hxx"    
#include "SipFrom.hxx"
    
using namespace Assist;


string
SipToParserException::getName( void ) const
{
    return "SipToParserException";
}

SipTo::SipTo(UrlType type)
    :
    toUrl(),
    tokenMap()
{
    urlType = type;
}

    
SipTo::SipTo(const SipTo& src)
    :
    toUrl(duplicateUrl(src.toUrl)),
    urlType(src.urlType),
    displayName(src.displayName),
    tag(src.tag),
    token(src.token),
    qstring(src.qstring),
    tokenMap(src.tokenMap)
{
    if (toUrl != 0)
    {
	if (toUrl->getType() == SIP_URL)
	{
	    Sptr <SipUrl> sipUrl;
	   
	    (sipUrl.dynamicCast(toUrl))->initializeTo();
	}
    }
}


SipTo::SipTo( Sptr <BaseUrl> url)
    :
    toUrl(duplicateUrl(url)),
    displayName(),
    tag(),
    token(),
    qstring(),
    tokenMap()
{
    if (toUrl != 0)
    {
	if (toUrl->getType() == SIP_URL)
	{
	    Sptr <SipUrl> sipUrl;
    
	    (sipUrl.dynamicCast(toUrl))->initializeTo();
	}
    }
}


SipTo::SipTo( const Data& data )
    :
    toUrl(),
    tokenMap()
{
    try
    {
        decode(data);
       
	if (toUrl != 0)
	{
	    urlType = toUrl->getType(); //to keep the two in sync
    
	    if (toUrl->getType() == SIP_URL)
	    {
		Sptr <SipUrl> sipUrl;
		(sipUrl.dynamicCast(toUrl))->initializeTo();
	    }
	}
    }
    catch (SipToParserException&)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in Constructor :o( ");
            throw SipToParserException("failed in Decode", __FILE__, __LINE__, DECODE_TO_FAILED);

        }

    }
}

SipTo::SipTo(const SipFrom& src) 
    :
        toUrl(src.getUrl()),
        urlType(toUrl->getType()),
        displayName(src.getDisplayName()),
        tag(src.getTag()),
        token(src.getToken()),
        qstring(src.getQstring()),
        tokenMap(*src.getTokenDetails())
{
    if (toUrl != 0)
    {
	if (toUrl->getType() == SIP_URL)
	{
	    Sptr <SipUrl> sipUrl;
	    sipUrl.dynamicCast(toUrl);
	    sipUrl->initializeTo();
	}
    }
}



void
SipTo::scanSipTo( const Data & tmpdata)
{
    Data sipdata;
    Data data = tmpdata;
    int ret = data.match("<", &sipdata, true);
    if (ret == NOT_FOUND)
    {
        // this must be of the form
        // To: sip:xxxx@xxxx.xx.com

        Data value;
        Data tmpval;
        int retn = data.match(";", &value, true) ;
        if (retn == NOT_FOUND)
        {  
            // in this case, it must be of the form
            // To: sip:xxxx@xxxx.xx.com
            // without optional parameters
            tmpval = value;
            toUrl = BaseUrl::decode(data);
        }
        else if (retn == FIRST)
        {
            // in this case, this is some sort of problem.
            if (SipParserMode::sipParserMode())
            {
                cpLog(LOG_ERR, "Failed to Decode in Constructor :o( ");
                throw SipToParserException("failed in Decode", __FILE__, __LINE__, DECODE_TO_FAILED);

            }

        }
        else if (retn == FOUND)
        { // if found then it has the Addrs-params
            tmpval = value;
            //remove spaces since of form <url>SP;tag= (pingtel bug)
            tmpval.removeSpaces();
            toUrl = BaseUrl::decode(tmpval);
            //remove spaces since of form <url>SP;tag= (pingtel bug)
            value.removeSpaces();
            value = data;
            parseAddrParam(value);
        }

    }
    else if (ret == FIRST)
    {
        try
        {
            parseUrl(data);
        }
        catch (SipToParserException&)
        {
            if (SipParserMode::sipParserMode())
            {
                cpLog(LOG_ERR, "Failed to Decode in Scan SIPTO :o( ");
                throw SipToParserException("failed in Decode in scan SipTo", __FILE__, __LINE__, PARSE_URL_FAILED);

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
SipTo::parseUrl(const Data& data)
{
    Data urlvalue = data;
    Data avalue;
    int retur = urlvalue.match(">", &avalue, true);
    if (retur == NOT_FOUND)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Parse in ParseUrl() :o( ");
            throw SipToParserException("failed in Decode", __FILE__, __LINE__, PARSE_URL_FAILED);

        }


    }
    else if (retur == FIRST)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Parse in ParseUrl()  :o( ");
            throw SipToParserException("failed in Decode", __FILE__, __LINE__, PARSE_URL_FAILED);

        }

    }
    else if (retur == FOUND)
    {
        //Remove Space for Pingel
        avalue.removeSpaces();
        toUrl = BaseUrl::decode(avalue);

        Data te = urlvalue;
        Data fik;
        //remove spaces since of form <url>SP;tag= (pingtel bug)
        te.removeSpaces();
        int rety = te.match(";", &fik, true);
        if (rety == FOUND)
        {
        }

        else if (rety == FIRST)
        {
            parseAddrParam(te);
        }
        else if (rety == NOT_FOUND)
        {
        }
    }
}


void
SipTo::parseNameInfo(const Data& data)
{
    Data newnameinfo;
    Data nameinfo = data;
    //nameinfo.removeSpaces();  // !jf! to handle 7960 problem
    setDisplayName(nameinfo);

}


void
SipTo::parseAddrParam(const Data& data)
{
    Data addrparm = data;
    Data parm;
    while (1)
    {
        int ret = addrparm.match(";", &parm, true);
        if (ret == NOT_FOUND)
        {
            parseAddrParamExt(addrparm);
            break;
        }
        else if (ret == FIRST)
        {}

        else if (ret == FOUND)
        {
            parseAddrParamExt(parm);
        }
    }
}


void
SipTo::parseAddrParamExt(const Data& newdat)
{
    Data checkdata = newdat;
    Data parmvalue;
    int retn = checkdata.match("=", &parmvalue, true);
    if (retn == NOT_FOUND)
    {
        tokenMap[checkdata] = "";
    }
    else if (retn == FIRST)
    {
    }

    else if (retn == FOUND)
    {
        parmvalue.removeSpaces();
        parseTag(parmvalue, checkdata);
        
    }
}


// create value given the token and the value as the two parameters
void
SipTo::parseTag(const Data& intoken, const Data& vdata)
{
    if (intoken == TAG_CHECK)
    {
        tag = vdata;
        tag.removeSpaces();
    }
    else
    {
        Data copy = intoken;
        Data copy2 = vdata;
        copy.removeSpaces();
        copy2.removeSpaces();
        tokenMap[copy] = copy2;
    }
}


void
SipTo::parse( const Data& todata)
{
    Data data = todata;

    try
    {

        scanSipTo(data);
    }
    catch (SipToParserException&)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in Parse :o( ");
            throw SipToParserException("failed in Decode", __FILE__, __LINE__, DECODE_TO_FAILED);

        }

    }
    //everything allright.
}


void
SipTo::decode( const Data& tostr )
{
    try
    {
        parse(tostr);
    }
    catch (SipToParserException&)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in decode() :o( ");
            throw SipToParserException("failed in Decode", __FILE__, __LINE__, DECODE_TO_FAILED);

        }
    }
}


const SipTo& 
SipTo::operator = (const SipTo& srcTo)
{
    if (&srcTo != this)
    {
        toUrl = duplicateUrl(srcTo.toUrl);
        displayName = srcTo.displayName;
        tag = srcTo.tag;
        token = srcTo.token;
        qstring = srcTo.qstring;
        tokenMap = srcTo.tokenMap;
    }
    return (*this);
}


const bool 
SipTo::operator<(const SipTo& srcTo) const
{
    bool less;
    
    if ( (toUrl != 0) && (srcTo.toUrl != 0) )
    {
	less = ( toUrl->isLessThan(srcTo.toUrl));
    }
    else if ( (toUrl == 0) && (srcTo.toUrl != 0) )
    {
	less = true;
    }
    else
    {
	less = false;
    }
    if ( less ||
        (tag < srcTo.tag) ||
        (token < srcTo.token) ||
        (qstring < srcTo.qstring)
       )
    {
        return true;
    }
    return false;
}


const bool 
SipTo::operator>(const SipTo& srcTo) const
{
    return (srcTo < *this);
}


const bool 
SipTo::operator ==(const SipTo& srcTo) const
{
    cpDebug(LOG_DEBUG_STACK, "SipTo ::operator ==");

    bool equal = false;
    if ( (toUrl != 0) && (srcTo.toUrl != 0) )
    {
	equal = ( toUrl->areEqual(srcTo.toUrl));
    }
    else if ( (toUrl == 0) && (srcTo.toUrl == 0) )
    {
	equal = true;
    }
    else 
    {
	equal = false;
    }

    if (equal == false)
    {
        cpDebug(LOG_DEBUG_STACK, " url match failed" );

	if (toUrl != 0)
	{
	    cpDebug(LOG_DEBUG_STACK, "this.url = %s" , toUrl->encode().logData()
		);
	}
#ifdef __linux__
	if (srcTo.toUrl != 0)
	{
	    cpDebug(LOG_DEBUG_STACK, "other.url = %s" ,
		  srcTo.toUrl->encode().logData() );
	}
#endif
    }

    equal = equal && (tag == srcTo.tag);
    cpDebug(LOG_DEBUG_STACK, " tags equal is : %d", equal);
    cpDebug(LOG_DEBUG_STACK, "this.tag = %s", getTag().logData());
    cpDebug(LOG_DEBUG_STACK, "other.Tag = %s", srcTo.getTag().logData());
    equal = (equal && (tokenMap == srcTo.tokenMap));
    cpDebug(LOG_DEBUG_STACK, " tokenmap equal: %d", equal);
    cpDebug(LOG_DEBUG_STACK, " to operator == final result: %d", equal);
    return equal;
}

void
SipTo::setPort(int portNum)
{
    setPortData(Data(portNum));
}

void 
SipTo::setPortData(const Data& newport)
{
    if (urlType  == TEL_URL)
    {
	return;
    }
    if (toUrl == 0)
    {
	toUrl = new SipUrl();
    }
    if (toUrl != 0)
    {
	if (toUrl->getType() == SIP_URL)
	{
	    Sptr <SipUrl> sipUrl;
	    (sipUrl.dynamicCast(toUrl))->setPort(newport);
	}
    }
}


Data
SipTo::getPortData() const
{
    Data toUrlport;
    if (toUrl != 0)
    {
	if (toUrl->getType() == SIP_URL)
	{
	    Sptr <SipUrl> sipUrl;
	    toUrlport = (sipUrl.dynamicCast(toUrl))->getPort();
	}
    }
    return toUrlport;
}
    
    
void 
SipTo::setTokenDetails(const Data& intoken, const Data& tokenValue)
{
    tokenMap[intoken] = tokenValue;
}


Sptr < SipTo::TokenMapTo >
SipTo::getTokenDetails() const
{
    Sptr < SipTo::TokenMapTo > dupMap = new TokenMapTo(tokenMap) ;

    return dupMap;
}


Data 
SipTo::getTokenValue(const Data& intoken) const
{
    Data ret;
    TokenMapTo::const_iterator check = tokenMap.find(intoken);
    ;

    if (check != tokenMap.end())
    {
        ret = check -> second;
    }

    return ret;

}


int
SipTo::getPort() const
{
    int p = getPortData().convertInt();
    return p;
}


void 
SipTo::setUser(const Data& newuser)
{
    if (toUrl == 0)
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
    if (toUrl != 0)
    {
	toUrl->setUserValue(newuser);
    }
    //display name defaulted to user.
    if (displayName.length() == 0)
    {
        displayName = newuser;
    }
}


Data 
SipTo::getUser() const
{
    Data toUser;
    if (toUrl != 0)
    {
	toUser = toUrl->getUserValue();
    }
    return toUser;
}


void 
SipTo::setDisplayName(const Data& name)
{
    displayName = name;
}

const Data& 
SipTo::getDisplayName() const
{
    return displayName;
}

    
void 
SipTo::setHost(const Data& newhost)
{
    if (urlType == TEL_URL)
    {
	//the user might set this to TelUrl, and then call setHost.
	//so, then , we should not create a SipUrl object.
	return;
    }
    
    if (toUrl == 0)
    {
	toUrl = new SipUrl();
    }
    
    if (toUrl != 0)
    {

    	if (toUrl->getType() == SIP_URL)
	{
	    Sptr <SipUrl> sipUrl;
    
	    (sipUrl.dynamicCast(toUrl))->setHost(newhost);
	}
    }
}
    

Data 
SipTo::getHost() const
{
    Data toHost;
    if (toUrl != 0)
    {
	if (toUrl->getType() == SIP_URL)
	{
	    Sptr<SipUrl> sipUrl;
    
	    toHost = (sipUrl.dynamicCast(toUrl))->getHost();
	}
    }
    return toHost;
}


Data 
SipTo::encode() const
{
    Data sipTo;
    
    if (toUrl != 0)
    {
	sipTo = TO;
	sipTo += SP;
	
	if (displayName.length())
	{
	    sipTo += displayName;
	}
	sipTo += "<";
	    
	sipTo += toUrl->encode();
    
    	sipTo+= ">";

	Data tagparam = getTag();
	
        if (tagparam.length() > 0)
        {
            sipTo += ";";
            sipTo += "tag=";
            sipTo += tagparam;
        }
        TokenMapTo::const_iterator i = tokenMap.begin();
        if (i != tokenMap.end())
        {
            sipTo += ";";
        }
        while (i != tokenMap.end())
        {
            Data token1 = i->first;
            Data tokenValue1 = i->second;
            sipTo += token1;
            if (tokenValue1.length() > 0)
            {
                sipTo += "=";
                sipTo += tokenValue1;
            }
	
            ++i;
	
            if ( i != tokenMap.end())
            {
                sipTo += ";";
            }
        }
        
        sipTo += CRLF;
    }
    
    return sipTo;
}


Sptr<BaseUrl> 
SipTo::getUrl() const
{
    return duplicateUrl(toUrl, true);
}


void 
SipTo::setUrl(Sptr<BaseUrl> tourl)
{
    toUrl = duplicateUrl(tourl);
}


SipHeader*
SipTo::duplicate() const
{
    return new SipTo(*this);
}


bool
SipTo::compareSipHeader(SipHeader* msg) const
{
    SipTo* otherMsg = dynamic_cast<SipTo*>(msg);
    if(otherMsg != 0)
    {
	return (*this == *otherMsg);
    }
    else
    {
	return false;
    }
}
