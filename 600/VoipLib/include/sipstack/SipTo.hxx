#ifndef SipTo_HXX
#define SipTo_HXX
/*
 * $Id: SipTo.hxx,v 1.2 2007/03/01 20:04:28 lizhijie Exp $
 */


#include "global.h"
#include "Data.hxx"
#include "BaseUrl.hxx"
#include "VException.hxx"
#include "cpLog.h"
#include <map>
#include "Sptr.hxx"
#include "SipHeader.hxx"

namespace Vocal
{

class SipFrom;    

enum SipToErrorType
{
    DECODE_TO_FAILED,
    PARSE_URL_FAILED,
    USERINFO_ERR,
    PARSE_ADDPRAM_FAILED
    //may need to change this to be more specific
};

/// Exception handling class SipToParserException
class SipToParserException : public VException
{
    public:
        SipToParserException( const string& msg,
                              const string& file,
                              const int line,
                              const int error = 0 );
        SipToParserException( const string& msg,
                              const string& file,
                              const int line,
                              SipToErrorType i)
        : VException( msg, file, line, static_cast < int > (i))
        {
            value = i;
        }
        const SipToErrorType& getError() const
        {
            return value;
        }
        string getName() const ;
    private:
        SipToErrorType value;

};



/// data container for To header
class SipTo : public SipHeader
{
    public:
        typedef map < Data , Data, less < Data > > TokenMapTo;
        /// Create one with default values
        SipTo(UrlType type  = SIP_URL);
        ///
        SipTo( Sptr <BaseUrl> url);
        ///
        SipTo( const Data& data );
        ///
        SipTo(const SipTo&);
        ///
        explicit SipTo(const SipFrom& from);

        ///
        Data encode() const;
        ///
        const SipTo& operator = (const SipTo&);
        ///
        void setPort(int newport);
        ///
        int getPort() const;

        ///
        void setPortData(const Data& newport);
        ///
        Data getPortData() const ;

        ///
        void setUser(const Data& newuser);
        ///
        Data getUser() const;
        ///
        void setDisplayName(const Data& name);
        ///
        const Data& getDisplayName() const;
        ///
        void setHost(const Data& newhost);
        ///
        Data getHost() const;
        ///
        Sptr <BaseUrl> getUrl() const;
        ///
        void setUrl(Sptr <BaseUrl>  tourl);

        const Data& getTag() const
        {
            return tag;
        }

        void setTag(const Data& newtag)
        {
            tag = newtag;
        }

        void setToken(const Data & newtoken)
        {
            token = newtoken;
        }

        Data getToken() const
        {
            return token;
        }

        void setQstring(const Data & newqstring)
        {
            qstring = newqstring ;
        }

        Data getQstring() const
        {
            return qstring;
        }

        ///
        const bool operator<(const SipTo&) const;
        ///
        const bool operator>(const SipTo&) const;

        ///
        const bool operator ==(const SipTo& srcTo) const;
        ///
        const bool operator != (const SipTo& srcTo) const
        { return !(*this == srcTo);}

        void setTokenDetails(const Data& token, const Data& tokenValue);

        Sptr < TokenMapTo > getTokenDetails() const;

        Data getTokenValue(const Data& token) const;

        void decode(const Data& data);
	/// method for copying sip headers of any type without knowing which type
	SipHeader* duplicate() const;

	/// compare two headers of (possibly) the same class
	virtual bool compareSipHeader(SipHeader* msg) const;
private:
    ///
    Sptr <BaseUrl> toUrl;
    ///
    UrlType urlType;
    ///
    Data displayName;
    Data tag;
    Data token;
    Data qstring;
    TokenMapTo tokenMap;
    void parse(const Data& data);
    void parseSipTo(const Data& data);
    void scanSipTo(const Data& data);
    void parseNameInfo(const Data& data);
    void parseAddrParam(const Data& data);
    void parseUrl(const Data& data);
    void parseToken(const Data& data, const Data& value);
    void parseAddrParamExt(const Data& newdat);
    void parseTag(const Data& tok, const Data& vdata);
    
    friend class SipMsg;
    
};
 
} // namespace Vocal

#endif
