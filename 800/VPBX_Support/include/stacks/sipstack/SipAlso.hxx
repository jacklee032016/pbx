#ifndef SIP_ALSO_HXX_
#define SIP_ALSO_HXX_

/*
* $Log: SipAlso.hxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:09  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: SipAlso.hxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#include "global.h"
#include "Data.hxx"
#include "BaseUrl.hxx"
#include "VException.hxx"
#include "cpLog.h"
#include <map>
#include "Sptr.hxx"
#include "SipHeader.hxx"

namespace Assist
{

enum SipAlsoErrorType
{
    DECODE_ALSO_FAILED,
    PARSE_AURL_FAILED,
    USERINFO_AERR
    //may need to change this to be more specific
};

/// Exception handling class SipAlsoParserException
class SipAlsoParserException : public VException
{
    public:
        SipAlsoParserException( const string& msg,
                                const string& file,
                                const int line,
                                const int error = 0 );
        SipAlsoParserException( const string& msg,
                                const string& file,
                                const int line,
                                SipAlsoErrorType i)
        : VException( msg, file, line, static_cast < int > (i))
        {
            value = i;
        }
        SipAlsoErrorType getError() const
        {
            return value;
        }
        string getName() const ;
    private:
        SipAlsoErrorType value;

};



/// data container for Also header
class SipAlso : public SipHeader
{
    public:

        SipAlso(UrlType uType = SIP_URL);
        ///
        SipAlso( Sptr <BaseUrl> url);
        ///
        SipAlso( const Data& data );
        ///
        SipAlso(const SipAlso&);
        ///
        Data encode() const;
        ///
        SipAlso& operator = (const SipAlso&);
        ///
        void setDisplayName(const Data& name);
        ///
        Data getDisplayName() const;
        ///
        void setHost(const Data& newhost);
        ///
        Data getHost() const;
        ///
        Sptr <BaseUrl> getUrl() const;
        ///
        void setUrl(Sptr <BaseUrl> alsourl);

        ///
        bool operator ==(const SipAlso& srcAlso) const;


	/// method for copying sip headers of any type without knowing which type
	SipHeader* duplicate() const;
	/// compare two headers of (possibly) the same class
	virtual bool compareSipHeader(SipHeader* msg) const;
    private:
        ///
        Sptr <BaseUrl> alsoUrl;
        ///
        UrlType urlType;
        ///
        Data displayName;

        void decode(const Data& data);
        void parse(const Data& data);
        void scanSipAlso(const Data& data);
        void parseNameInfo(const Data& data);
        void parseUrl(const Data& data);
        friend class SipMsg;

};
 
}

#endif
