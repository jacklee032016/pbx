#ifndef SipRoute_HXX
#define SipRoute_HXX
/*
* $Log: SipRoute.hxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:09  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: SipRoute.hxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#include "BaseUrl.hxx"
#include "global.h"
#include "symbols.hxx"
#include "Data.hxx"
#include "cpLog.h"
#include "SipHeader.hxx"
#include "Sptr.hxx"
#include "VException.hxx"

namespace Assist
{

class SipRecordRoute;
    
enum SipRouteErrorType
{
    DECODE_ROUTE_FAILED,
    ROUTE_URL_PARSE_FAIL

    //may need to change this to be more specific
};

/// Exception handling class
class SipRouteParserException : public VException
{
    public:
        ///
        SipRouteParserException( const string& msg,
                                 const string& file,
                                 const int line,
                                 const int error = 0 );
        ///
        SipRouteParserException( const string& msg,
                                 const string& file,
                                 const int line,
                                 SipRouteErrorType i)
        : VException( msg, file, line, static_cast < int > (i))
        {
            value = i;
        }
        ///
        SipRouteErrorType getError() const
        {
            return value;
        }
        ///
        string getName() const ;
    private:
        ///
        SipRouteErrorType value;

};


class SipRouteParserAddParmException : public SipRouteParserException
{
    public:
        ///
        SipRouteParserAddParmException( const string& msg,
                                        const string& file,
                                        const int line,
                                        const int error = 0 );
        ///
        string getName( void ) const;
};

/// Exception handling class SipUrlParserUrlParmsException
class SipRouteParserUrlException : public SipRouteParserException
{
    public:
        ///
        SipRouteParserUrlException( const string& msg,
                                    const string& file,
                                    const int line,
                                    const int error = 0 );
        ///
        string getName( void ) const;
};





/// data container for Route header
class SipRoute : public SipHeader
{
    public:
        /// Create one with default values
        SipRoute(UrlType uType = SIP_URL);

        /*** Create by decoding the data string passed in. This is the decode
             or parse. This call should only be used inside the stack and is
             not part of the API */
        SipRoute( const Data& srcData );
        ///
        SipRoute( const SipRoute& src );
        ///
        explicit SipRoute( const SipRecordRoute& rr);
        
        ///
        void parse(const Data & tmpdata);
        ///
        void parseUrl(const Data & data);
        ///
        void setDisplayName(const Data & newdisplayname)
        {
            displayname = newdisplayname;
        }
        ///
        Data getDisplayName() const
        {
            return displayname;
        }
        ///
        Sptr<BaseUrl> getUrl() const;

        ///
        void setUrl( Sptr <BaseUrl> url);

        /*** return the encoded string version of this. This call should only be
             used inside the stack and is not part of the API */
        Data encode() const;

        ///
        const SipRoute& operator =(const SipRoute& src);

        ///
        bool operator ==(const SipRoute& src) const;
	/// method for copying sip headers of any type without knowing which type
	SipHeader* duplicate() const;
	/// compare two headers of (possibly) the same class
	virtual bool compareSipHeader(SipHeader* msg) const;
    private:
        Sptr <BaseUrl> url;
        UrlType urlType;
        Data displayname;
        void decode(const Data& data);
};

ostream&
operator<<(ostream& s, const SipRoute& route);

 
} 

#endif
