#ifndef SipTelSubscriberUser_HXX
#define SipTelSubscriberUser_HXX
/*
* $Log: SipTelSubscriberUser.hxx,v $
* Revision 1.1.1.1  2006/11/30 16:26:29  lizhijie
* AS800 VPBX
*
* Revision 1.1  2006/06/03 12:06:38  lizhijie
* no message
*
* $Id: SipTelSubscriberUser.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "global.h"
#include "Data.hxx"
#include "VException.hxx"
#include "symbols.hxx"
#include "BaseUser.hxx"
    
namespace Assist
{
    

///
class SipTelSubscriberUserParserException : public VException
{
public:
    ///
    SipTelSubscriberUserParserException( const string& msg,
                               const string& file,
                               const int line, 
			       const int error = 0)
	:VException(msg, file, line, error)
	{
	}
    ///
    string getName() const;  
};

/// pure virtual class for Url types
class SipTelSubscriberUser : public BaseUser
{
public:
    /// Create one with default values
    SipTelSubscriberUser();
    ///
    ~SipTelSubscriberUser();
    ///
    SipTelSubscriberUser(const Data&) throw(SipTelSubscriberUserParserException&);
    ///
    Sptr<BaseUser> duplicate() const;
    ///parse
    bool  decode(const Data& data);
    ///called by a SipUrl telUser
    Data encode() const;
    ///called by a TelUrl telUser
    Data encodeWithParams() const;
    ///
    bool operator ==(const SipTelSubscriberUser&) const;
    ///
    bool operator < (const SipTelSubscriberUser&) const;
    ///
    bool areEqual(Sptr<BaseUser> baseUser) const;
    ///
    bool isLessThan(Sptr<BaseUser> baseUser) const;
    ///
    UserType getType() const;
    
    ///global phone number sub-component functions.
    ///
    Data getBasePhoneNumber() const;
    ///
    void setBasePhoneNumber(const Data& data);
    ///
    Data getIsdnSubAddress() const;
    ///
    void setIsdnSubAddress(const Data& data);
    ///
    Data getPostDial() const;
    ///
    void setPostDial(const Data& data);
    ///
    Data getPhoneContext() const;
    ///
    void setPhoneContext(const Data& data);
    ///
    Data getServiceProvider() const;
    ///
    void setServiceProvider(const Data& data);
    ///
    Data getFutureExtensionName() const;
    ///
    void setFutureExtensionName(const Data& data);
    ///
    Data getFutureExtensionValue() const;
    ///
    void setFutureExtensionValue(const Data& data);
    ///
    Data getTelParams() const;
    ///call this only if you want along with TelParams.
    Data getGlobalPhoneNumber() const;
    ///
    void setGlobalPhoneNumber(const Data& data);
    ///call this only if you want along with TelParams.
    Data getLocalPhoneNumber() const;
    ///
    void setLocalPhoneNumber(const Data& data);
    ///
    bool getIsGlobalPhoneNumber() const;
private:
    bool isGlobalPhoneNumber;  //if false, this is a localPhoneNumber
    
    mutable bool isGlobalPhoneNumberValid;  //if false, the globalPhoneNumber is not valid.
    //very important to set this flag to false, whenever, the sub-components
    //are being set.
    mutable bool isLocalPhoneNumberValid;
    
    mutable Data globalPhoneNumber;
    mutable Data localPhoneNumber;
    
    //sub-components for globalPhoneNumber/ localPhoneNumber
    Data basePhoneNumber;
    Data isdnSubAddress;
    Data postDial;
    Data phoneContext;
    Data serviceProvider;
    Data futureExtensionName;
    Data futureExtensionValue;
    
    
};

 
}

#endif
