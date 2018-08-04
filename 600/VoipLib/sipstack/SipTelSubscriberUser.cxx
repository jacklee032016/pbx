/*
 * $Id: SipTelSubscriberUser.cxx,v 1.2 2007/03/01 20:04:32 lizhijie Exp $
 */

#include "global.h"
#include "BaseUser.hxx"
#include "SipTelSubscriberUser.hxx"
#include "Data.hxx"
    
using namespace Vocal;

string
SipTelSubscriberUserParserException::getName( void ) const
{
    return "SipTelSubscriberUserParserException";
}       
  
SipTelSubscriberUser::SipTelSubscriberUser()
    :BaseUser(),
     isGlobalPhoneNumber(false)
{
}
    
    
SipTelSubscriberUser::~SipTelSubscriberUser()
{
}

    
SipTelSubscriberUser::SipTelSubscriberUser(const Data& data) 
    throw(SipTelSubscriberUserParserException&)
    :BaseUser(),
     isGlobalPhoneNumber(false),
     isGlobalPhoneNumberValid(false),
     isLocalPhoneNumberValid(false),
     globalPhoneNumber(),
     localPhoneNumber(),
     basePhoneNumber(),
     isdnSubAddress(),
     postDial(),
     phoneContext(),
     serviceProvider(),
     futureExtensionName(),
     futureExtensionValue()
{
    if  (decode(data)  == false)
    {
	throw SipTelSubscriberUserParserException("failed in decode of telSubscriberUser", __FILE__, __LINE__);
    }
}
    
Sptr<BaseUser> 
SipTelSubscriberUser::duplicate() const
{
    return new SipTelSubscriberUser(*this);
}
    
bool
SipTelSubscriberUser::decode(const Data& data)
{
    //telephone-subscriber = global-phone-number/local-phone-number
    Data phoneData = data;
    bool parseFailed;
    
    phoneData.parse("+", &parseFailed);
    
    if (!parseFailed)
    {
	isGlobalPhoneNumber = true;
    }
    
    bool matchFailed;
    
    basePhoneNumber = phoneData.parse(";", &matchFailed); 
    if (matchFailed)
    {
	basePhoneNumber = phoneData;
	//no isdn, post-dial, area-specifier, service-provider, 
	//future-extension
    }
    else
    {
	//parseSubcomponents.
	 bool done = false;
	 bool matchedParam = false;

	 char  matchedChar;
    
	 while(!done)
	 {
	     // look for an equal sign
	     Data key = phoneData.matchChar("=", &matchedChar);
	     Data value;

	     if(matchedChar == '=')
	     {
		 matchedParam = true;
    
		 // find the value
		 value = phoneData.matchChar(";", &matchedChar);
		 if(matchedChar != ';')
		 {
		     // did not match, so this must be the last parameter
		     value = phoneData;
		     phoneData.erase();
		 }
		 if (key == "isub")
		 {
		     isdnSubAddress = value;
		 }
		 else if (key == "postd")
		 {
		     postDial = value;
		 }
		 else if (key == "phone-context")
		 {
		     phoneContext = value;
		 }
		 else if (key == "tsp")
		 {
		     serviceProvider = value;
		 }
		 else if (matchedParam)
		 {
		     //this must be future-extension.
		     futureExtensionName = key;
		     futureExtensionValue = value;
		 }
		 matchedParam = false;
	     }
	     else
	     {
		 done = true;
	     }
	 }
    }
    return true; //this should be based on parsing global/local correctly.
}
    
    
Data 
SipTelSubscriberUser::getGlobalPhoneNumber() const
{
    if (isGlobalPhoneNumberValid)
    {
        return globalPhoneNumber;
    }
    else
    {

	Data globalPhoneInfo;
    
	if (basePhoneNumber.length())
	{
	    globalPhoneInfo = "+";
	    globalPhoneInfo += basePhoneNumber;
	    if (isdnSubAddress.length())
	    {
		globalPhoneInfo += SEMICOLON;
		globalPhoneInfo += "isub=";
		globalPhoneInfo += isdnSubAddress;
	    }
	    if (postDial.length())
	    {
		globalPhoneInfo += SEMICOLON;
		globalPhoneInfo += "postd=";
		globalPhoneInfo += postDial;
	    }
	    if (phoneContext.length())
	    {
		globalPhoneInfo += SEMICOLON;
		globalPhoneInfo += "phone-context";
		globalPhoneInfo += phoneContext; 
	    }
	    if (serviceProvider.length())
	    {
		globalPhoneInfo += SEMICOLON;
		globalPhoneInfo += "tsp";
		globalPhoneInfo += serviceProvider;
	    }
	    if ( (futureExtensionName.length()  ) && 
		 (futureExtensionValue.length() )
		)
	    {
		globalPhoneInfo += SEMICOLON;
		globalPhoneInfo += futureExtensionName;
		globalPhoneInfo += EQUAL;
		globalPhoneInfo += futureExtensionValue;
	    }
	    
	    globalPhoneNumber = globalPhoneInfo;
	    isGlobalPhoneNumberValid = true;
	    
	}
	return globalPhoneInfo;
    }
}

    
UserType
SipTelSubscriberUser::getType() const
{
    return TEL_USER;
}
    
bool SipTelSubscriberUser::isLessThan(Sptr<BaseUser> baseUser) const
{
    if (baseUser->getType() == TEL_USER)
    {
        Sptr<SipTelSubscriberUser> newUser;
        newUser.dynamicCast(baseUser);
        return ( *(this) < *(newUser) );
    }
    else
    {
	return false;
    }
}
    
    
bool SipTelSubscriberUser::areEqual(Sptr<BaseUser> baseUser) const
{
    if (baseUser->getType() == TEL_USER)
    {
        Sptr<SipTelSubscriberUser> newUser;
        newUser.dynamicCast(baseUser);
        return ( *(this) == *(newUser) );
    }
    else
    {
	return false;
    }
}

    
bool
SipTelSubscriberUser::operator == (const SipTelSubscriberUser& src) const
{
    if (isGlobalPhoneNumber != src.isGlobalPhoneNumber)
    {
	return false;
    }
    return ( (basePhoneNumber == src.basePhoneNumber) &&
	     (isdnSubAddress == src.isdnSubAddress) &&
	     (postDial == src.postDial) &&
	     (phoneContext == src.phoneContext) &&
	     (serviceProvider == src. serviceProvider) &&
	     (futureExtensionName == src.futureExtensionName) &&
	     (futureExtensionValue == src.futureExtensionValue) );
}
    
bool 
SipTelSubscriberUser::operator < (const SipTelSubscriberUser& src) const
{
    if ( (basePhoneNumber < src.basePhoneNumber) ||
	 (isdnSubAddress < src.isdnSubAddress) ||
	 (postDial < src.postDial) ||
	 (phoneContext < src.phoneContext) ||
	 (serviceProvider < src.serviceProvider) ||
	 (futureExtensionName < src.futureExtensionName ) ||
	 (futureExtensionValue < src.futureExtensionValue)
       )
    {
	return true;
    }
    else
    {
	return false;
    }
}

        
Data
SipTelSubscriberUser::getTelParams() const
{
    Data otherParams;
    if (isdnSubAddress.length())
    {
	otherParams += SEMICOLON;
	otherParams += "isub=";
	otherParams += isdnSubAddress;
    }
    if (postDial.length())
    {
	otherParams += SEMICOLON;
	otherParams += "postd=";
	otherParams += postDial;
    }
    if (phoneContext.length())
    {
	otherParams += SEMICOLON;
	otherParams += "phone-context=";
	otherParams += phoneContext; 
    }
    if (serviceProvider.length())
    {
	otherParams += SEMICOLON;
	otherParams += "tsp=";
	otherParams += serviceProvider;
    }
    if ( (futureExtensionName.length()  ) && 
	 (futureExtensionValue.length() )
	)
    {
	otherParams += SEMICOLON;
	otherParams += futureExtensionName;
	otherParams += EQUAL;
	otherParams += futureExtensionValue;
    }
    return otherParams;
}

    
Data 
SipTelSubscriberUser::getLocalPhoneNumber() const
{
    if (isLocalPhoneNumberValid)
    {
        return localPhoneNumber;
    }
    else
    {

	Data localPhoneInfo;
    
	if (basePhoneNumber.length())
	{
	    localPhoneInfo += basePhoneNumber;
	    if (isdnSubAddress.length())
	    {
		localPhoneInfo += SEMICOLON;
	        localPhoneInfo += "isdn=";
		localPhoneInfo += isdnSubAddress;
	    }
	    if (postDial.length())
	    {
		localPhoneInfo += SEMICOLON;
		localPhoneInfo += "postd=";
		localPhoneInfo += postDial;
	    }
	    if (phoneContext.length())
	    {
		localPhoneInfo += SEMICOLON;
		localPhoneInfo += "phone-context=";
		localPhoneInfo += phoneContext; 
	    }
	    if (serviceProvider.length())
	    {
		localPhoneInfo += SEMICOLON;
	        localPhoneInfo += "tsp=";
		localPhoneInfo += serviceProvider;
	    }
	    if ( (futureExtensionName.length()  ) && 
		 (futureExtensionValue.length() )
		)
	    {
		localPhoneInfo += SEMICOLON;
		localPhoneInfo += futureExtensionName;
		localPhoneInfo += EQUAL;
		localPhoneInfo += futureExtensionValue;
	    }
	    
	    localPhoneNumber = localPhoneInfo;
	    isLocalPhoneNumberValid = true;
	    
	}
	return localPhoneInfo;
    }
}    
    
    
    
//set and get functions for sub-components of global phone number.
Data
SipTelSubscriberUser::getBasePhoneNumber() const
{
    return basePhoneNumber;
}
    

void
SipTelSubscriberUser::setBasePhoneNumber(const Data& data)
{
    basePhoneNumber = data;
    isGlobalPhoneNumberValid = false;
    isLocalPhoneNumberValid = false;
}    
    
    
Data
SipTelSubscriberUser::getIsdnSubAddress() const
{
    return isdnSubAddress;
}
    
    
void
SipTelSubscriberUser::setIsdnSubAddress(const Data& data)
{
    isdnSubAddress = data;
    isGlobalPhoneNumberValid = false;
    isLocalPhoneNumberValid = false; 
}    
    
    
Data
SipTelSubscriberUser::getPostDial() const
{
    return postDial;
}
    
    
void
SipTelSubscriberUser::setPostDial(const Data& data)
{
    postDial = data;
    isGlobalPhoneNumberValid = false;
    isLocalPhoneNumberValid = false;
}        
    
    
Data
SipTelSubscriberUser::getPhoneContext() const
{
    return phoneContext;
}
    
    
void
SipTelSubscriberUser::setPhoneContext(const Data& data)
{
    phoneContext = data;
    isGlobalPhoneNumberValid = false;
    isLocalPhoneNumberValid = false;
}        
    
    
Data
SipTelSubscriberUser::getServiceProvider() const
{
    return serviceProvider;
}
    
    
void
SipTelSubscriberUser::setServiceProvider(const Data& data)
{
    serviceProvider = data;
    isGlobalPhoneNumberValid = false;
    isLocalPhoneNumberValid = false;
}    
    
    
Data
SipTelSubscriberUser::getFutureExtensionName() const
{
    return futureExtensionName;
}
    
    
void
SipTelSubscriberUser::setFutureExtensionName(const Data& data)
{
    futureExtensionName = data;
    isGlobalPhoneNumberValid = false;
    isLocalPhoneNumberValid = false;
}    
    
    
Data
SipTelSubscriberUser::getFutureExtensionValue() const
{
    return futureExtensionValue;
}
    
    
void
SipTelSubscriberUser::setFutureExtensionValue(const Data& data)
{
    futureExtensionValue = data;
    isGlobalPhoneNumber = false;
    isLocalPhoneNumberValid = false;
    
}    
    

Data
SipTelSubscriberUser::encode() const //this returns with no TelParams.
{
    Data encUser;
    if (isGlobalPhoneNumber)
    {
	encUser = "+";
    }
    encUser += basePhoneNumber;
    
    return encUser;
}
    
    
Data
SipTelSubscriberUser::encodeWithParams() const
{
    Data encUser;
    if (isGlobalPhoneNumber)
    {
	if (isGlobalPhoneNumberValid)
	{
	    encUser = globalPhoneNumber;
	}
	else
	{
	    encUser = getGlobalPhoneNumber();
	}
    }
    else //localPhoneNumber
    {
	encUser = getLocalPhoneNumber();
    }
    return encUser;
}
