#ifndef SipUrl_HXX
#define SipUrl_HXX
/*
* $Log: SipUrl.hxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:09  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: SipUrl.hxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#include <string>
#include "global.h"
#include "SipParserMode.hxx"
#include "Data.hxx"
#include "VException.hxx"
#include "BaseUrl.hxx"
#include "BaseUser.hxx"
#include "Sptr.hxx"
#include "EmbeddedObj.hxx"
#include "NetworkAddress.h"

/// Exception handling class

namespace Assist
{
    
enum SipUrlErrorType
{
    DECODE_FAILED,
    SIP_NOT_PRESENT,
    USER_INFO_ERR,
    MANDATORY_ITEM_MISSING,
    NOT_VALID_URL_PARMS,
    NOT_VALID_TRANSORT_PARM,
    NOT_VALID_USER_PHONE_PARM,
    NOT_VALID_TTL_PARM,
    NOT_VALID_URL_DATA 
};

///
class SipUrlParserException : public VException
{
    public:
        ///
        SipUrlParserException( const string& msg,
                               const string& file,
                               const int line,
                               const int error = 0 );
    
        ///
        SipUrlParserException( const string& msg,
                               const string& file,
                               const int line,
                               SipUrlErrorType i)
        : VException( msg, file, line, static_cast < int > (i))
        {
            value = i;
        }
        ///
        SipUrlErrorType getError() const
        {
            return value;
        }
        ///
        string getName() const ;
    private:
        SipUrlErrorType value;

};

/// data container for URLs of type sip:
class SipUrl : public BaseUrl
{
    public:
	/// Create one with default values. must specify loose router
	SipUrl(bool lr=false);
	///
	explicit SipUrl(const Data& data) throw(SipUrlParserException &) ;
	///
	SipUrl(const SipUrl& );
    
	///
	~SipUrl();
	///
        bool areEqual(Sptr<BaseUrl>) const;
        ///
        bool isLessThan(Sptr<BaseUrl>) const;
	/// comparison operator is used by others
	virtual bool areEqual(const BaseUrl& other) const;

        ///
	bool operator<(const SipUrl& srcUrl) const;
	///
	bool operator>(const SipUrl& srcUrl) const;
    
	///
	bool operator ==(const SipUrl& srcUrl) const;
    
	///
	SipUrl& operator =(const SipUrl& srcUrl);

        ///
        Sptr<BaseUrl> duplicate() const;
	///
	Data getUserValue() const ;  
	///
	bool setUserValue(const Data& newuser, const Data& newUserParam = "") ; 
	///
	void setUser(Sptr<BaseUser> newUser);
	///
	Sptr <BaseUser> getUser() const;
	///
        Sptr<EmbeddedObj> getEscObj() {return EscObj;}
        ///
        const Data& getHost() const ;
        ///
        void setEscObj(Sptr<EmbeddedObj> newescobj);
        ///
	void setHost(const Data& newhost) ;
	///
	const Data& getPort() const ;
    
	///
	void setPort(const Data& newport) ;
    
	///
	const Data& getPasswd() const ;
    
	///
	void setPasswd(const Data& newpasswd) ;
    
	///
	const Data& getTransportParam() const ;
    
	///
	void setTransportParam(const Data& newtransport) ;
    
	///
	const Data& getMaddrParam() const ;
    
	///
	void setMaddrParam(const Data& newmaddr) ;

        ///
        bool isLooseRouterPresent() const;
        
	///
	bool operator != (const SipUrl& srcUrl) const
        { return !(*this == srcUrl);}
    

	///
	const Data& getTtlData() const ;
    
	///
	void setTtlParam(const Data& newttl) throw(SipUrlParserException&);
    
	///
	const Data& getTtlParam() const;
    
	///        
	const Data& getUserParam() const ;
    
	///
	void setUserParam(const Data& newuserParm) ;
    
	///
	const Data& getMethodParam() const ;
    
	///
	void setMethodParam(const Data& newmethodParm);
    
	///
	const Data& getOtherParam() const ;
    
	///
	void setOtherParam(const Data& newotherParm);
    
	///
	void setOtherName(const Data& newOtherName) ;
    
	///
	const Data& getOtherName() const;
    
	///
	const Data& getHeaders() const ;
    
	///
	void setHeaders(const Data& newheader) ;
    
    
	/// return the encoded string version of this. This call should only be
	///used inside the stack and is not part of the API 
	Data encode() const ;
    
	///
	Data getNameAddr() const;

        /// get a unique key for a transaction id
        Data getUniqueKey() const;
    
	///
	Data getUrlParam() const;
    
	/// 
	void initializeFrom();
    
	///
	void initializeTo();
        
	/// get name and address without the port if it is the default
	Data getNameAddrNoPort() const;

	///parser functions
	void decode(const Data& data);
	///
	UrlType getType() const;
	///

        NetworkAddress asNetworkAddress() const;
    private:
    
        Data schemeName;
        Sptr<BaseUser> user;
        Data passwd;
        Data host;
        Data port;
        // URL Parameters
        Data transportParam;
        Data ttlParam;
        Data maddrParam;
        Data userParam;
        Data methodParam;
        Data otherParam;
        Data otherName;
        Data headers;
        bool lrParam;
        bool ipv6Addr;
        Sptr<EmbeddedObj> EscObj;
        ///
        bool fastDecode( Data myData );
        ///
        bool parseIpv6Address(Data& inData);

};

 
} 

#endif
