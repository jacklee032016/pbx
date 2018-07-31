#ifndef SipRedirect_HXX
#define SipRedirect_HXX
/*
* $Log: SipRedirect.hxx,v $
* Revision 1.1.1.1  2006/11/30 16:26:29  lizhijie
* AS800 VPBX
*
* Revision 1.1  2006/06/03 12:06:38  lizhijie
* no message
*
* $Id: SipRedirect.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include <map>

#include "global.h"
#include "Data.hxx"
#include "SipUrl.hxx"
#include "VException.hxx"
#include "Sptr.hxx"


namespace Assist
{


class SipUrl;
enum SipRedirectErrorType
{
    DECODE_REDIRECT_FAILED,
    PARSE_RURL_FAILED,
    LIMIT_ERR,
    COUNT_FAILED,
    REASON_FAILED,
    DISPLAY_FAILED
    //may need to change this to be more specific
};

/// Exception handling class
class SipRedirectParserException : public VException
{
    public:
        ///
        SipRedirectParserException( const string& msg,
                                    const string& file,
                                    const int line,
                                    const int error = 0 );
        ///
        SipRedirectParserException( const string& msg,
                                    const string& file,
                                    const int line,
                                    SipRedirectErrorType i)
        : VException( msg, file, line, static_cast < int > (i))
        {
            value = i;
        }
        ///
        SipRedirectErrorType getError() const
        {
            return value;
        }
        ///
        string getName() const ;
    private:
        ///
        SipRedirectErrorType value;

};




/// data container for Redirect header
class SipRedirect
{
    public:

        typedef map < Data , Data > TokenMapRedirect ;

        /// Create one with default values
        SipRedirect();
        ///
        ~SipRedirect();
        ///
        SipRedirect( const SipUrl& url);
        ///
        SipRedirect(const Data& data );
        ///
        SipRedirect(const SipRedirect&);
        ///
        Data encode();
        ///
        SipRedirect& operator = (const SipRedirect&);
        ///
        void setReason(const Data& reason);
        ///
        Data getReason() const;
        ///
        void setHost(const Data& newhost);
        ///
        Data getHost() const;

        ///
        void setCounter(const Data& count);
        ///
        Data getCounter() const ;

        ///
        void setLimit(const Data& newuser);
        ///
        Data getLimit() const;
        ///

        SipUrl getUrl() const;
        ///
        void setUrl(const SipUrl& tourl);
        ///
        bool operator ==(const SipRedirect& srcRedirect) const;
        //
        bool operator< (const SipRedirect& src) const;


        //
        void setDisplayName(const Data& name);
        //
        void parseToken(const Data& data);

        ///
        void parseTag( Data& data);

        ///
        Sptr < SipRedirect::TokenMapRedirect > getTokenDetails();

        ///
        void setTag(const Data& newtag) ;

        ///
        Data SipRedirect::getTag() const;

        ///
        Data isToken(const Data& sdata );

        ///
        bool isCounter(const Data& sdata);

        ///
        bool isLimit(const Data& sdata);

        ///
        bool isReason(const Data& sdata);

        ///
        bool isEmpty(const Data& sdata);

        ///
        void setTokenDetails(const Data& token, const Data& tokenValue);


        ///

        void print();
    private:
        ///
        SipUrl rurl;
        Data rcounter;
        Data rlimit;
        Data rreason;
        Data displayName;
        Data tag;
        TokenMapRedirect tokenMap;
        void decode(const Data& data);
        void scanSipRedirect( const Data & tmpdata);
        void parse( const Data& reidrectdata);
        void parseNameInfo(const Data& data);
        void parseUrl(const Data&);
        void parseLimit(const Data& data);
        void parseCounter(const Data& data);
        void parseReason(const Data& data);

};
 
}

#endif
