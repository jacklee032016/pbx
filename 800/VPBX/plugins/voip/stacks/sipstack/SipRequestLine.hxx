#ifndef SipRequestLine_HXX
#define SipRequestLine_HXX
/*
* $Log: SipRequestLine.hxx,v $
* Revision 1.1.1.1  2006/11/30 16:26:29  lizhijie
* AS800 VPBX
*
* Revision 1.1  2006/06/03 12:06:38  lizhijie
* no message
*
* $Id: SipRequestLine.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "global.h"

#include "Data.hxx"
#include "SipUrl.hxx"
#include "symbols.hxx"
#include "BaseUrl.hxx"
#include "VException.hxx"

namespace Assist
{

class SipMsg;
class SipUrl;

enum SipRequestLineErrorType
{
    DECODE_REQUESTLINE_FAILED

    //may need to change this to be more specific
};

/// Exception handling class
class SipRequestLineParserException : public VException
{
    public:
        SipRequestLineParserException( const string& msg,
                                       const string& file,
                                       const int line,
                                       const int error = 0 );
        SipRequestLineParserException( const string& msg,
                                       const string& file,
                                       const int line,
                                       SipRequestLineErrorType i)
            : VException( msg, file, line, static_cast < int > (i))
            {
                value = i;
            }
        const SipRequestLineErrorType& getError() const
            {
                return value;
            }
        string getName() const ;
    private:
        SipRequestLineErrorType value;

};

/** 
    data container for the RequestLine of a SIP request msg.
    The request line is the one in the form
   <p>
   <pre>
   INVITE sip:a@b.com SIP/2.0
   </pre>
   which is the first line of a SIP request (in this case, INVITE).
*/

class SipRequestLine
{
    public:
        /// Create one with default values
        SipRequestLine(UrlType type = SIP_URL);
        ///
        SipRequestLine(const SipRequestLine& src);

        ///
        void setTransportParam(const Data& transport);

        ///
        Data getTransportParam() const;

        ///
        Data getHost() const;

        ///
        void setHost(const Data& host);
        ///
        void setPort(const Data& port);
        ///
        void setUrl(Sptr <BaseUrl> url);
        /// get the request URI
        Sptr <BaseUrl> getUrl() const;

        ///
        Data getMethod() const;
        ///
        void setMethod(const Data& newmethod);
        ///
        SipRequestLine( const Data& data );
        ///
        SipRequestLine(Method method, Sptr<BaseUrl> url);
        ///
        SipRequestLine(const Data& data, Sptr<BaseUrl> url);
        ///
        Data encode() const;
        ///
        void setProtocol(const Data& newprotocol);
        ///
        void setVersion(const Data& newversion);
        ///
        void parseVersion(const Data& newver);
        ///
        Data getProtocol() const;
        ///
        Data getVersion() const;
        ///
        const SipRequestLine& operator=(const SipRequestLine& src);
        ///
        bool operator==(const SipRequestLine& src) const; 

        /** decode the text of a RequestLine.
            @param data   text to be decoded
            @exception   thrown if there is an error parsing
        */
        void decode(const Data& data) throw (SipRequestLineParserException&);
    private:

        /// scan the SIP request line
        void scanSipRequestLine(const Data & scandata) 
            throw (SipRequestLineParserException &);

        Sptr<BaseUrl> requestUrl;
        UrlType urlType;
        Data method;
        Data version;
        Data protocol;
};

}

#endif
