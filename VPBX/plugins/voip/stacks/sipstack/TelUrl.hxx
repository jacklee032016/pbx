#ifndef TelUrl_HXX
#define TelUrl_HXX
/*
* $Log: TelUrl.hxx,v $
* Revision 1.1.1.1  2006/11/30 16:26:29  lizhijie
* AS800 VPBX
*
* Revision 1.1  2006/06/03 12:06:38  lizhijie
* no message
*
* $Id: TelUrl.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include <string>
#include "global.h"
#include "SipParserMode.hxx"
#include "Data.hxx"
#include "VException.hxx"
#include "BaseUrl.hxx"
#include "SipTelSubscriberUser.hxx"

namespace Assist
{
    
/// Exception handling class TelUrlParserException

enum TelUrlErrorType
{
    TEL_DECODE_FAILED,
    TEL_NOT_PRESENT,
    TEL_USER_INFO_ERR,
    TEL_MANDATORY_ITEM_MISSING,
    TEL_NOT_VALID_URL_PARMS,
    TEL_NOT_VALID_TRANSORT_PARM,
    TEL_NOT_VALID_USER_PHONE_PARM,
    TEL_NOT_VALID_TTL_PARM,
    TEL_NOT_VALID_URL_DATA 
};

///
class TelUrlParserException : public VException
{
    public:
        ///
        TelUrlParserException( const string& msg,
                               const string& file,
                               const int line,
                               const int error = 0 );
        ///
        TelUrlParserException( const string& msg,
                               const string& file,
                               const int line,
                               TelUrlErrorType i)
        : VException( msg, file, line, static_cast < int > (i))
        {
            value = i;
        }
        ///
        TelUrlErrorType getError() const
        {
            return value;
        }
        ///
        string getName() const ;
    
    private:
        TelUrlErrorType value;

};

/// data container for URLs of type tel:
class TelUrl : public BaseUrl
{
    public:
	/// Create one with default values
	TelUrl();
	///
	TelUrl(const Data& data) throw(TelUrlParserException &) ;
	///
	TelUrl(const TelUrl& );
    
	///
	~TelUrl();
	///
        Sptr<BaseUrl> duplicate() const;
        ///
        bool areEqual(Sptr<BaseUrl>) const;
        ///
        bool isLessThan(Sptr<BaseUrl>) const;
	/// comparison operator is used by others
	virtual bool areEqual(const BaseUrl& other) const;
        ///
	bool operator<(const TelUrl& srcUrl) const;
	///
	bool operator>(const TelUrl& srcUrl) const;
    
	///
	bool operator ==(const TelUrl& srcUrl) const;
    
	///
	TelUrl& operator =(const TelUrl& srcUrl);
	///
	bool setUserValue(const Data& newUser, const Data& dummy = "" );
	///used inside the stack and is not part of the API 
	Data encode() const ;

	///parser functions
	void decode(const Data& data);
    
	///
	Data getUserValue() const;
    
	///
	Sptr <BaseUser> getUser() const;
    
	///
	void setUser(Sptr <BaseUser> newUser);
  
	///
	UrlType getType() const;
        /// get a unique key for a transaction id
        Data getUniqueKey() const;
    
    
	///
    private:
	///
	Data schemeName;
	/// 
	Sptr <SipTelSubscriberUser> user;
    
	///
	bool fastDecode( Data myData );
};
 
} // namespace Assist

#endif
