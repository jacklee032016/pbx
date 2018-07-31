#ifndef SipDiversion_HXX
#define SipDiversion_HXX
/*
* $Log: SipDiversion.hxx,v $
* Revision 1.1.1.1  2006/11/30 16:26:29  lizhijie
* AS800 VPBX
*
* Revision 1.1  2006/06/03 12:06:38  lizhijie
* no message
*
* $Id: SipDiversion.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include <map>

#include "global.h"
#include "BaseUrl.hxx"
#include "Data.hxx"
#include "SipHeader.hxx"
#include "Sptr.hxx"
#include "VException.hxx"


namespace Assist
{

class BaseUrl;
enum SipDiversionErrorType
{
    DECODE_DIVERSION_FAILED,
    PARSE_RURL_FAILED,
    LIMIT_ERR,
    COUNT_FAILED,
    REASON_FAILED,
    DISPLAY_FAILED
    //may need to change this to be more specific
};

/// Exception handling class SipDiversionParserException
class SipDiversionParserException : public VException
{
    public:
        ///
        SipDiversionParserException( const string& msg,
                                    const string& file,
                                    const int line,
                                    const int error = 0 );
        ///
        SipDiversionParserException( const string& msg,
                                    const string& file,
                                    const int line,
                                    SipDiversionErrorType i)
        : VException( msg, file, line, static_cast < int > (i))
        {
            value = i;
        }
        ///
        SipDiversionErrorType getError() const
        {
            return value;
        }
        ///
        string getName() const ;
    private:
        ///
        SipDiversionErrorType value;

};




/// data container for Diversion header

class SipDiversion : public SipHeader
{
    public:

        typedef map < Data , Data > TokenMapDiversion ;

        /// Create one with default values
        SipDiversion(UrlType uType = SIP_URL);
        ///
        ~SipDiversion();
        ///
        SipDiversion( Sptr<BaseUrl> url);
        ///
        SipDiversion(const Data& data );
        ///
        SipDiversion(const SipDiversion&);

        ///
        Data encode() const;
        ///
        SipDiversion& operator = (const SipDiversion&);
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

        Sptr<BaseUrl> getUrl() const;
        ///
        void setUrl(Sptr <BaseUrl> tourl);
        ///
        bool operator ==(const SipDiversion& srcDiversion) const;
        //
        bool operator< (const SipDiversion& src) const;


        //
        void setDisplayName(const Data& name);
        //
        void parseToken(const Data& data);

        ///
        void parseTag( Data& data);

        ///
        Sptr < TokenMapDiversion > getTokenDetails();

        ///
        void setTag(const Data& newtag) ;

        ///
        Data getTag() const;

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
    //  void setDextensionDetails(const Data& token, const Data& tokenValue);

        ///

        void print();
	/// method for copying sip headers of any type without knowing which type
	SipHeader* duplicate() const;
	/// compare two headers of (possibly) the same class
	virtual bool compareSipHeader(SipHeader* msg) const;
    private:
        ///
        Sptr<BaseUrl> rurl;
        UrlType urlType;
        Data rcounter;
        Data rlimit;
        Data rreason;
        Data displayName;
        Data tag;
        TokenMapDiversion tokenMap;
    //  TokenMapDiversion divextension;
        void decode(const Data& data);
        void scanSipDiversion( const Data & tmpdata);
        void parse( const Data& reidrectdata);
        void parseNameInfo(const Data& data);
        void parseUrl(const Data&);
        void parseLimit(const Data& data);
        void parseCounter(const Data& data);
        void parseReason(const Data& data);

};
 
}

#endif
