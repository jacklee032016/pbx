#ifndef SipReferredBy_HXX
#define SipReferredBy_HXX
/*
* $Log: SipReferredBy.hxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:09  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: SipReferredBy.hxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#include "global.h"

#include <map>

#include "Data.hxx"
#include "VException.hxx"
#include "BaseUrl.hxx"
#include "Sptr.hxx"
#include "SipHeader.hxx"
#include "Mutex.hxx"

namespace Assist
{


enum SipReferredByErrorType
{
    DECODE_REFERREDBY_FAILED,
    PARSE_REFERRERURL_FAILED,
    PARSE_REFERENCEDURL_FAILED

    //may need to change this to be more specific
};

/// Exception handling class SipReferredByParserException
class SipReferredByParserException : public VException
{
    public:
        SipReferredByParserException( const string& msg,
                                      const string& file,
                                      const int line,
                                      const int error = 0 );
        SipReferredByParserException( const string& msg,
                                      const string& file,
                                      const int line,
                                      SipReferredByErrorType i)
        : VException( msg, file, line, static_cast < int > (i))
        {
            value = i;
        }
        SipReferredByErrorType getError() const
        {
            return value;
        }
        string getName() const ;
    private:
        SipReferredByErrorType value;

};



/// data container for ReferredBy header
class SipReferredBy : public SipHeader
{
    public:

        /// Create one with default values
        typedef map < Data , Data > ReferredByPgpMap;


        SipReferredBy(UrlType uType = SIP_URL);

        ///
        ~SipReferredBy();
        ///
        SipReferredBy( const Data& data );
        ///
        SipReferredBy(const SipReferredBy&);
        ///
        Data encode() const;
        ///
        SipReferredBy& operator = (const SipReferredBy&);

        ///
        bool operator ==(const SipReferredBy& srcReferredBy) const;

        ///
        Data getReferencedUrl() const;
        ///

        Sptr <BaseUrl> getReferrerUrl() const;
        ///
        void setDisplayName(const Data& name);
        ///
        void parseNameInfo(const Data& data);


        void setReferrerUrl(Sptr <BaseUrl> sipurl) ;

        void setReferencedUrl(const Data& data) ;

        ///
        Data getAuthScheme() const
        {
            return authScheme;
        }

        ///
        void setTokenDetails(const Data& token, const Data& tokenValue);

        ///
        Sptr < ReferredByPgpMap > getTokenDetails();

        ///
        Data getTokenValue(const Data& token);

        ///
        void scanAuthTokens( const Data& data);

	/// method for copying sip headers of any type without knowing which type
	SipHeader* duplicate() const;
	/// compare two headers of (possibly) the same class
	virtual bool compareSipHeader(SipHeader* msg) const;
    private:
        ///
        Sptr <BaseUrl> referrerUrl;
        UrlType urlType;
        Data referencedUrl;
        Data displayName;
        Data address;

        ReferredByPgpMap pgpMap;
        Data authScheme;
        //Data basicCookie;

        mutable Threads::Mutex tokenMutex;
        void decode(const Data& data);
        void parse(const Data& data);
        void scanReferrerUrl(const Data& data);
        void scanReferencedUrl(const Data& data);
        void parseUrl(const Data& data);

        friend class SipMsg;
};
 
}

#endif
