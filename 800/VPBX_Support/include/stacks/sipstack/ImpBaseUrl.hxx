#ifndef ImpBaseUrl_HXX
#define ImpBaseUrl_HXX
/*
* $Log: ImpBaseUrl.hxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:09  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: ImpBaseUrl.hxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#include <string>
#include "global.h"
#include "SipParserMode.hxx"
#include "Data.hxx"
#include "VException.hxx"
#include "BaseUrl.hxx"
#include "Sptr.hxx"


namespace Assist
{

enum ImpBaseUrlErrorType
{
    IMP_DECODE_FAILED,
    IMP_USER_INFO_ERR,
    IMP_MANDATORY_ITEM_MISSING,
    IMP_NOT_VALID_URL_DATA 
};

/// Exception handling class ImpBaseUrlParserException
class ImpBaseUrlParserException : public VException
{
    public:
        ///
        ImpBaseUrlParserException( const string& msg,
                               const string& file,
                               const int line,
                               const int error = 0 );
    
        ///
        ImpBaseUrlParserException( const string& msg,
                               const string& file,
                               const int line,
                               ImpBaseUrlErrorType i)
        : VException( msg, file, line, static_cast < int > (i))
        {
            value = i;
        }
        ///
        ImpBaseUrlErrorType getError() const
        {
            return value;
        }
        ///
        string getName() const
        {
            return "ImpBaseUrlParserException";
        }
    private:
        ImpBaseUrlErrorType value;

};

/// data container for Url header
class ImpBaseUrl : public BaseUrl
{
    public:
	/// Create one with default values
	ImpBaseUrl();
	///
	explicit ImpBaseUrl(const Data& data, const Data type) throw(ImpBaseUrlParserException &) ;
	///
	ImpBaseUrl(const ImpBaseUrl& );
    
	///
	virtual ~ImpBaseUrl();
	///
        bool areEqual(Sptr<BaseUrl>) const;
        ///
        bool isLessThan(Sptr<BaseUrl>) const;
	/// comparison operator is used by others
	virtual bool areEqual(const BaseUrl& other) const;

        ///
	bool operator<(const ImpBaseUrl& srcUrl) const;
	///
	bool operator>(const ImpBaseUrl& srcUrl) const;
    
	///
	bool operator ==(const ImpBaseUrl& srcUrl) const;
    
	///
	ImpBaseUrl& operator =(const ImpBaseUrl& srcUrl);

        ///
        virtual Sptr<BaseUrl> duplicate() const = 0;
	///
	Data getUserValue() const ;  
	///
	bool setUserValue(const Data& newuser , const Data& noUse="") ; 
	///
	const Data& getHost() const ;
	///
	void setHost(const Data& newhost) ;
    
	///
	bool operator != (const ImpBaseUrl& srcUrl) const
        { return !(*this == srcUrl);}
    
	/// return the encoded string version of this. This call should only be
	///used inside the stack and is not part of the API 
	Data encode() const ;
    
        /// get a unique key for a transaction id
        Data getUniqueKey() const;
    
	/// get name and address 
	Data getNameAddr() const;

	///parser functions
	void decode(const Data& data);
	///
	virtual UrlType getType() const = 0;

        ///
        const Data& getSchemeName() const { return schemeName; };
	///
	///Should be removed from the base class as pure virtual
	void setUser(Sptr<BaseUser> newUser) { assert(0); }
	///Should be removed from the base class as pure virtual
	Sptr <BaseUser> getUser() const { assert(0); return 0; };
    protected:
        Data schemeName;
        Data user;
        Data host;
        bool fastDecode( Data myData );
        ///
};
 
}

#endif
