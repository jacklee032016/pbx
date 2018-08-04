/*
 * $Id: ImpBaseUrl.cxx,v 1.2 2007/03/01 20:04:31 lizhijie Exp $
 */

#include "global.h"
#include <cassert>
#include "symbols.hxx"
//#include "HostMatch.hxx"
#include "cpLog.h"
#include "Data.hxx"
#include "ImpBaseUrl.hxx"

using namespace Vocal;

ImpBaseUrl::ImpBaseUrl(const ImpBaseUrl& src)
        :BaseUrl(),
        schemeName(src.schemeName),
        user(src.user),
        host(src.host)
{
}


ImpBaseUrl::ImpBaseUrl( const Data& data , const Data sName) throw(ImpBaseUrlParserException&)
        :BaseUrl(),
        schemeName(sName),
        user(),
        host()
{
    try
    {
        decode(data);
    }
    catch (ImpBaseUrlParserException&)
    {

        if (SipParserMode::sipParserMode())
        {
            throw ImpBaseUrlParserException("failed in Decode", __FILE__, __LINE__, IMP_DECODE_FAILED);
        }
    }
}

ImpBaseUrl::ImpBaseUrl()
        :BaseUrl(),
        schemeName(),
        user(),
        host()
{
}



ImpBaseUrl::~ImpBaseUrl()
{
}

bool ImpBaseUrl::areEqual(Sptr<BaseUrl> baseUrl) const
{
    return areEqual(*baseUrl);
}
    
    
bool ImpBaseUrl::isLessThan(Sptr<BaseUrl> baseUrl) const
{
    if (baseUrl->getType() == getType())
    {
        Sptr<ImpBaseUrl> newUrl;
        newUrl.dynamicCast(baseUrl);
        return ( *(this) < *(newUrl) );
    }
    else
    {
	return false;
    }
}

bool
ImpBaseUrl::areEqual(const BaseUrl& baseUrl) const
{
    if(baseUrl.getType() == getType())
    {
        const ImpBaseUrl& newUrl = dynamic_cast<const ImpBaseUrl&> (baseUrl);
        return ( *(this) == newUrl );
    }
    else
    {
        return false;
    }
}
 
    
    
ImpBaseUrl& ImpBaseUrl::operator =(const ImpBaseUrl& srcUrl)
{ 
    if (&srcUrl != this) 
    {
        schemeName = srcUrl.schemeName;
	user = srcUrl.user;
	host = srcUrl.host;
    }
    return (*this);
}

bool ImpBaseUrl::operator<(const ImpBaseUrl& srcUrl) const
{
    //we won't compare hosts in this.
    if (user < srcUrl.user)
    {
        return true;
    }
    else
    {
	return false;
    }

}

bool ImpBaseUrl::operator>(const ImpBaseUrl& srcUrl) const
{
    //compare reverse.
    return ( srcUrl < (*this) );
}
    
    
bool ImpBaseUrl::operator ==(const ImpBaseUrl& srcUrl) const
{
    bool equal = false;
    cpDebug(LOG_DEBUG_STACK, "Url operator ==  function");
   
    cpDebug(LOG_DEBUG_STACK, "this.schemeName= %s" , schemeName.logData());
    cpDebug(LOG_DEBUG_STACK, "other.schemeName= %s", srcUrl.schemeName.logData());

    cpDebug(LOG_DEBUG_STACK, "this.user= %s" , user.logData());
    cpDebug(LOG_DEBUG_STACK, "other.user= %s", srcUrl.user.logData());
    
    cpDebug(LOG_DEBUG_STACK, "this.host= %s" , host.logData());
    cpDebug(LOG_DEBUG_STACK, "other.host= %s", srcUrl.host.logData());
    
    equal =  ((isEqualNoCase(schemeName, srcUrl.schemeName)) &&
	     (isEqualNoCase(user, srcUrl.user)) &&
             (isEqualNoCase(host, srcUrl.host) ) ); 

    
    cpDebug(LOG_DEBUG_STACK, "ImpBaseUrl comparison: , returning: %s", (equal == true ? "true" : "false" ));
    return equal;
    
}
    

void
ImpBaseUrl::decode( const Data& urlstr )
{
    if (fastDecode(urlstr) == false) {
            cpLog(LOG_ERR, "Failed in Decode (%s) URL", schemeName.logData());
            throw ImpBaseUrlParserException("Failed in Decode URL", 
					__FILE__, __LINE__, IMP_NOT_VALID_URL_DATA);
    }
    cpDebug(LOG_DEBUG_STACK, "completed decoding in the ImpBaseUrl");
}    // ImpBaseUrl::decode

bool
ImpBaseUrl::fastDecode( Data myData )
{
    bool parseFailed = true;
    
    Data urlType = myData.parse(":", &parseFailed);

    if(parseFailed)
    {
	cpDebug(LOG_DEBUG, "parse failed: could not find ':' separating URL type from body");
	return false;
    }

    if  (!isEqualNoCase(urlType, schemeName) )
    {
	cpDebug(LOG_DEBUG, "parse failed: scheme not %s", schemeName.logData());
	return false;
    }
    
    // now, either user or host, depending

    char matchedChar;

    Data tmpData;
    tmpData = myData.matchChar("@", &matchedChar);

    if(matchedChar == '@')
    {
	// this really is a user
	user = tmpData;
	// the next thing must be a host
	host = myData.matchChar(";", &matchedChar);
	if(matchedChar == ';')
	{
            cpLog(LOG_ERR, "Failed to parse (%s) URL", schemeName.logData());
            return false;
	}
	else
	{
	    //no separator, so the rest has to be a host
	    host = myData;
	    myData.erase();
	}
    }
    else
    {
        cpLog(LOG_ERR, "Failed to parse (%s) URL", schemeName.logData());
        return false;
    }

    return true;
}


///
Data
ImpBaseUrl::encode() const
{
    Data imurl;

    if (schemeName.length())
    {
	imurl = schemeName;
    }
    imurl+= COLON;

    // userinfo = user 
    if (user.length() != 0)
    {
        imurl += user;
        imurl += "@";
    }

    imurl += host;

    return imurl;
}    // ImpBaseUrl::encode


Data
ImpBaseUrl::getNameAddr() const
{
    Data nameaddr;
    if (schemeName.length())
    {
	nameaddr = schemeName;
	nameaddr += COLON;
    }
    
    if (user.length())
    {
	nameaddr += user;
        nameaddr += "@";
    }
    nameaddr += host;
    return nameaddr;
}


Data
ImpBaseUrl::getUniqueKey() const
{
    return getNameAddr();
}




Data
ImpBaseUrl::getUserValue() const
{
    return user;
}

bool 
ImpBaseUrl::setUserValue(const Data& newUser, const Data& noUse)
{
    user = newUser;
    return true;
    
}

    
const Data&
ImpBaseUrl:: getHost() const
{
    return host;
}

    
void
ImpBaseUrl::setHost(const Data& newhost)
{
    host = newhost;
}
