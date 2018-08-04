#ifndef SipReferTo_HXX
#define SipReferTo_HXX
/*
* $Log: SipReferTo.hxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:09  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: SipReferTo.hxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#include "global.h"

#include "Data.hxx"

#include "VException.hxx"
#include "cpLog.h"
#include <map>
#include "BaseUrl.hxx"
#include "Sptr.hxx"
#include "SipHeader.hxx"

namespace Assist
{


enum SipReferToErrorType
{
    DECODE_REFERTO_FAILED,
    PARSE_REFERURL_FAILED

    //may need to change this to be more specific
};

/// Exception handling class SipReferToParserException
class SipReferToParserException : public VException
{
    public:
        SipReferToParserException( const string& msg,
                                   const string& file,
                                   const int line,
                                   const int error = 0 );
        SipReferToParserException( const string& msg,
                                   const string& file,
                                   const int line,
                                   SipReferToErrorType i)
        : VException( msg, file, line, static_cast < int > (i))
        {
            value = i;
        }
        SipReferToErrorType getError() const
        {
            return value;
        }
        string getName() const ;
    private:
        SipReferToErrorType value;

};


/// data container for ReferTo header
class SipReferTo : public SipHeader
{
    public:

        SipReferTo();
        ///
        SipReferTo( const Data& data );
        ///
        SipReferTo(const SipReferTo&);

        ///
        SipReferTo(Sptr <BaseUrl> url);
        ///
        Data encode() const;
        ///
        SipReferTo& operator = (const SipReferTo&);
        ///
        Data getUrlType() const;
        ///
        Data getUrlString() const;

        const Data& getDisplayName() const { return displayName; }
        const Data& getAfterAngleQuote() const { return afterAngleQuote; }
                

        ///
        bool operator ==(const SipReferTo& srcReferTo) const;



	/// method for copying sip headers of any type without knowing which type
	SipHeader* duplicate() const;
	/// compare two headers of (possibly) the same class
	virtual bool compareSipHeader(SipHeader* msg) const;
    private:
        ///
        Data urlType;
        Data address;
        Data displayName;
        Data afterAngleQuote;
        bool angleQuotes;

        void decode(const Data& data);
        void parse(const Data& data);
        void scanSipReferTo(const Data& data);
        void parseUrl(const Data& data);

        friend class SipMsg;
};
 
}

#endif
