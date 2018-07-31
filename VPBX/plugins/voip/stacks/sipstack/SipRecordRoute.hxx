#ifndef SipRecordRoute_HXX
#define SipRecordRoute_HXX
/*
* $Log: SipRecordRoute.hxx,v $
* Revision 1.1.1.1  2006/11/30 16:26:29  lizhijie
* AS800 VPBX
*
* Revision 1.1  2006/06/03 12:06:38  lizhijie
* no message
*
* $Id: SipRecordRoute.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "global.h"

#include "Data.hxx"
#include "BaseUrl.hxx"
#include "SipHeader.hxx"
#include "VException.hxx"

namespace Assist
{


enum SipRecordRouteErrorType
{
    DECODE_RECORDROUTE_FAILED,
    URL_PARSE_FAIL

    //may need to change this to be more specific
};

/// Exception handling class SipRecordRouteParserException
class SipRecordRouteParserException : public VException
{
    public:
        SipRecordRouteParserException( const string& msg,
                                       const string& file,
                                       const int line,
                                       const int error = 0 );
        SipRecordRouteParserException( const string& msg,
                                       const string& file,
                                       const int line,
                                       SipRecordRouteErrorType i)
        : VException( msg, file, line, static_cast < int > (i))
        {
            value = i;
        }
        const SipRecordRouteErrorType& getError() const
        {
            return value;
        }
        string getName() const ;
    private:
        SipRecordRouteErrorType value;

};


/// data container for RecordRoute header
class SipRecordRoute : public SipHeader
{
    public:
        /// Create one with default values
        SipRecordRoute(UrlType uType = SIP_URL);

        SipRecordRoute( const SipRecordRoute& src);

        SipRecordRoute(const Data & data);

        ///
        Sptr <BaseUrl> getUrl() const;

        ///
        void setUrl(Sptr <BaseUrl> url) ;
        void parse(const Data & tmpdata);
        void parseUrl(const Data & data);
        void setDisplayName(const Data& newdisplayname)
        {
            displayname = newdisplayname;
        }
        Data getDisplayName() const
        {
            return displayname;
        }
        /*** return the encoded string version of this. This call should only be
             used inside the stack and is not part of the API */
        Data encode() const;
        ///
        const SipRecordRoute& operator =(const SipRecordRoute& src);
        ///
        bool operator ==( const SipRecordRoute& src) const;

	/// method for copying sip headers of any type without knowing which type
	SipHeader* duplicate() const;
	/// compare two headers of (possibly) the same class
	virtual bool compareSipHeader(SipHeader* msg) const;
    private:

        Sptr <BaseUrl> url;
        UrlType urlType;
        Data displayname;
        void decode(const Data& data);

        friend class SipRoute;
};

 
}

#endif
