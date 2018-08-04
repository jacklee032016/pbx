#ifndef SIPFROM_HXX_
#define SIPFROM_HXX_
/*
* $Log: SipFrom.hxx,v $
* Revision 1.1.1.1  2006/11/30 16:26:29  lizhijie
* AS800 VPBX
*
* Revision 1.1  2006/06/03 12:06:38  lizhijie
* no message
*
* $Id: SipFrom.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include <map>

#include "global.h"
#include "Data.hxx"
#include "BaseUrl.hxx"
#include "VException.hxx"
#include "Sptr.hxx"
#include "SipHeader.hxx"

namespace Assist
{

enum SipFromErrorType
{
    DECODE_FROM_FAILED,
    URL_FAILED,
    USERINFO_ERROR,
    PARSE_ADDPRAM_ERR
    //may need to change this to be more specific
};

class SipTo;

/// Exception handling class SipFromParserException
class SipFromParserException : public VException
{
    public:
        SipFromParserException( const string& msg,
                                const string& file,
                                const int line,
                                const int error = 0 );
        SipFromParserException( const string& msg,
                                const string& file,
                                const int line,
                                SipFromErrorType i)
        : VException( msg, file, line, static_cast < int > (i))
        {
            value = i;
        }
        const SipFromErrorType& getError() const
        {
            return value;
        }
        string getName() const ;
    private:
        SipFromErrorType value;

};


/// data container for From header
class SipFrom : public SipHeader
{
    public:

        typedef map < Data , Data, less < Data > > TokenMapFrom;

        /// Create one with default values
        SipFrom(UrlType uType = SIP_URL);
        ///
        SipFrom(Sptr <BaseUrl> fromurl);
        ///
        SipFrom( const Data& data );
        ///
        SipFrom(const SipFrom& src);

        ///
        explicit SipFrom(const SipTo& to);
        ///
        const bool operator<(const SipFrom&) const;
        ///
        bool operator>(const SipFrom&) const;
        ///
        bool operator ==(const SipFrom&) const ;
        ///
        bool operator != (const SipFrom& aFrom) const
        { return !(*this == aFrom);}
        ///
        const SipFrom& operator =(const SipFrom&);
        ///
        Data encode() const;
        ///
        void setPort(const Data& newport);
        ///
        Data getPortData();
        ///
        int getPort() const;
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
        const Data& getTag() const;
        ///
        void setTag(const Data& item);

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
        Sptr <BaseUrl> getUrl() const;
        ///
        void setUrl(Sptr <BaseUrl> item);
        ///
        void setTokenDetails(const Data& token, const Data& tokenValue);
        ///
        Sptr < TokenMapFrom > getTokenDetails() const;
        ///
        Data getTokenValue(const Data& token) const;

        ///Parser Funtions
        void decode(const Data& data);

	/// method for copying sip headers of any type without knowing which type
	SipHeader* duplicate() const;
	/// compare two headers of (possibly) the same class
	virtual bool compareSipHeader(SipHeader* msg) const;
    private:

        Data displayName;
        Sptr <BaseUrl> fromUrl;
        UrlType urlType; //need this while creating Url, while setting for user
    
        Data tag;
        Data token;
        Data qstring;
        TokenMapFrom tokenMap;
        void parse(const Data& data);
        void parseSipFrom(const Data& data);
        void scanSipFrom(const Data& data);
        void parseNameInfo(const Data& data);
        void parseAddrParam(const Data& data);
        void parseUrl(const Data& data);
        void parseToken(const Data& data, const Data& value);
        void parseAddrParamExt(const Data& newdat);
        void parseTag(const Data& token, const Data& vdata);

        friend class SipMsg;
};
 
}

#endif
