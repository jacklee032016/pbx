/*
* $Log: TelUrl.cxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:10  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: TelUrl.cxx,v 1.1.1.1 2006/11/30 16:27:10 lizhijie Exp $
*/

#include "global.h"
#include "TelUrl.hxx"
#include "symbols.hxx"
#include "cpLog.h"
#include <cassert>

using namespace Assist;


    
string
TelUrlParserException::getName( void ) const
{
    return "TelUrlParserException";
}    


TelUrl::TelUrl(const TelUrl& src)
        :
        BaseUrl(src),
        schemeName(src.schemeName),
        user(src.user)
{
}


TelUrl::TelUrl( const Data& data ) throw(TelUrlParserException&)
        :
        BaseUrl(),
        schemeName(),
        user()
{
    try
    {
        decode(data);
    }
    catch (TelUrlParserException&)
    {

        if (SipParserMode::sipParserMode())
        {
            throw TelUrlParserException("failed in Decode", __FILE__, __LINE__, TEL_DECODE_FAILED);
        }
    }
}

    
TelUrl::TelUrl()
        :
        BaseUrl(),
        schemeName(),
        user()
{
}


TelUrl::~TelUrl()
{
}

Sptr<BaseUrl> TelUrl::duplicate() const
{
    return new TelUrl(*this);
}    
    
bool TelUrl::areEqual(Sptr<BaseUrl> baseUrl) const
{
    return areEqual(*baseUrl);
}
    
    
bool TelUrl::isLessThan(Sptr<BaseUrl> baseUrl) const
{
    if (baseUrl->getType() == TEL_URL)
    {
        Sptr<TelUrl> newUrl;
        newUrl.dynamicCast(baseUrl);
        return ( *(this) < *(newUrl) );
    }
    else
    {
	return false;
    }
}
    

// comparison operator is used by others
bool 
TelUrl::areEqual(const BaseUrl& baseUrl) const
{
    if(baseUrl.getType() == SIP_URL)
    {
        const TelUrl& newUrl = dynamic_cast<const TelUrl&> (baseUrl);
        return ( *(this) == newUrl );
    }
    else
    {
        return false;
    }
}



TelUrl& TelUrl::operator =(const TelUrl& srcUrl)
{
    if (this != &srcUrl)
    {
	schemeName = srcUrl.schemeName;
	user = srcUrl.user;
    }
    return (*this);
}

    
bool TelUrl::operator<(const TelUrl& srcUrl) const
{
    if (schemeName < srcUrl.schemeName) 
    {
	return true;
    }
    else if ( (user.getPtr() != 0) && (srcUrl.user.getPtr() != 0) )
    {
        return ( (*user) < (*(srcUrl.user)) );
      
    }
    else if ( (user.getPtr() == 0) && (srcUrl.user.getPtr() != 0) )
    {
        return true;
    }
    return false;

}

    
bool 
TelUrl::setUserValue(const Data& data, const Data& dummy)
{
    if (user.getPtr() == 0)
    {
	user = new SipTelSubscriberUser();
    }
    return user->decode(data);
}
    
    
bool TelUrl::operator>(const TelUrl& srcUrl) const
{
    return ( srcUrl < *this );
}
    
    
bool TelUrl::operator ==(const TelUrl& srcUrl) const
{
    bool equal = false;
    cpDebug(LOG_DEBUG_STACK, "Url operator ==  function");
    if (user.getPtr() != 0)
    {
	cpDebug(LOG_DEBUG_STACK, "this.user= %s", user->encodeWithParams().logData());
    }
    if (srcUrl.user.getPtr() != 0)
    {
	cpDebug(LOG_DEBUG_STACK, "other.user= %s", srcUrl.user->encodeWithParams().logData());
    }
    
    cpDebug(LOG_DEBUG_STACK, "this.schemeName = " , schemeName.logData());
    cpDebug(LOG_DEBUG_STACK, "other.schemeName= " , srcUrl.schemeName.logData());

    equal = (isEqualNoCase(schemeName, srcUrl.schemeName) );
    
    if ( (user.getPtr() != 0) && (srcUrl.user.getPtr() != 0) )
    {
	equal = equal && ( (*user) == ( *(srcUrl.user) ) );
    }
    else if ( (user.getPtr() == 0) && (srcUrl.user.getPtr() == 0) )
    {
	equal = equal && true;
    }
    
    
    cpDebug(LOG_DEBUG_STACK, "TelUrl comparison: , returning: %s", (equal == true ? "true" : "false" ));
    return equal;
}

    
void
TelUrl::decode( const Data& urlstr )
{
    if (fastDecode(urlstr) == false) {
            cpLog(LOG_ERR, "Failed in Decode (TEL_URL)");
            throw TelUrlParserException("Failed in Decode (TEL_URL)", 
					__FILE__, __LINE__, TEL_NOT_VALID_URL_DATA);
    }
    cpDebug(LOG_DEBUG_STACK, "completed decoding in the TelUrl");
}    // TelUrl::decode

    
bool
TelUrl::fastDecode( Data myData )
{
    // new decoder, hopefully faster

    bool parseFailed;
    
    Data urlType = myData.parse(":", &parseFailed);

    if(parseFailed)
    {
	cpDebug(LOG_DEBUG, "parse failed: could not find ':' separating URL type from body");
	return false;
    }

    if (!isEqualNoCase(urlType, "tel") )
    {
	cpDebug(LOG_DEBUG, "parse failed: scheme not TEL");
	return false;
    }
    //set it.
    schemeName = urlType;
    
    // now, pass the remaining to SipTelSubscriber.
    if (myData.length())
    {
	user = new SipTelSubscriberUser(myData);
    }
    if (user.getPtr() != 0)
    {
	return true;
    }
    else
    {
	return false;
    }
}


//
Data
TelUrl::encode() const
{
    // TEL-URL
    Data telUrl;

    if (schemeName.length())
    {
	telUrl = schemeName;
    }
   
    telUrl+= COLON;

    // userinfo = user [ ":" password ]
    if (user.getPtr() != 0)
    {
	Data userValue = user->encodeWithParams();
	if (userValue.length() > 0)
	{
	    telUrl += userValue;
	}
    }
   
    return telUrl;
}    // TelUrl::encode


Data
TelUrl::getUserValue() const
{
    Data userValue;
    if (user.getPtr() != 0)
    {
	userValue = user->encode();
    }
    return userValue;
}
    
   
    
void
TelUrl::setUser(Sptr <BaseUser> newUser)
{
    Sptr <SipTelSubscriberUser> telUser;
    
    telUser.dynamicCast(newUser);
    user = telUser;
}
    

Sptr <BaseUser> 
TelUrl::getUser() const
{
    const Sptr<BaseUser> baseUser = user;
    return baseUser;
}


UrlType
TelUrl::getType() const
{
    return TEL_URL;
}


Data 
TelUrl::getUniqueKey() const
{
    return getUserValue();
}    
    
//write more accessory functions to access SipTelSubscriberUser functions.
