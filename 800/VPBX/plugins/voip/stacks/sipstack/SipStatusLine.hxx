#ifndef SipStatusLine_HXX
#define SipStatusLine_HXX
/*
* $Log: SipStatusLine.hxx,v $
* Revision 1.1.1.1  2006/11/30 16:26:29  lizhijie
* AS800 VPBX
*
* Revision 1.1  2006/06/03 12:06:38  lizhijie
* no message
*
* $Id: SipStatusLine.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "global.h"
#include "Data.hxx"
#include "VException.hxx"

namespace Assist
{
    
enum SipStatusLineErrorType
{
    DECODE_STATUSLINE_FAILED

    //may need to change this to be more specific
};

/// Exception handling class
class SipStatusLineParserException : public VException
{
    public:
        SipStatusLineParserException( const string& msg,
                                      const string& file,
                                      const int line,
                                      const int error = 0 );
        SipStatusLineParserException( const string& msg,
                                      const string& file,
                                      const int line,
                                      SipStatusLineErrorType i)
        : VException( msg, file, line, static_cast < int > (i))
        {
            value = i;
        }
        SipStatusLineErrorType getError() const
        {
            return value;
        }
        string getName() const ;
    private:
        SipStatusLineErrorType value;

};

/**

   data container for the Status Line in a SIP response.  The status
   line is the one in the form
   <p>
   <pre>
   SIP/2.0 200 OK
   </pre>
   which is the first line of a SIP response.
*/

class SipStatusLine
{
    public:
        /// Create one with default values
        SipStatusLine();

        /// constructor
        SipStatusLine( const Data& scrData );

        /// copy constructor
        SipStatusLine( const SipStatusLine& src );

        /// operator =
        SipStatusLine& operator=( const SipStatusLine& src );

        /// equality operator
        bool operator ==(const SipStatusLine& src) const;

        ///
        int getStatusCode() const;

        ///
        void setStatusCode( const int& newStatusCode);

        ///
        const Data& getStatusCodeData();

        ///
        void setStatusCodeData( const Data& newStatusCode);

        ///
        void scanSipStatusLine(const Data & scandata);

        ///
        const Data& getReasonPhrase() const;

        ///
        void setReasonPhrase( const Data& newReasonPhrase);

        ///
        const Data& getVersion();

        ///
        void setVersion( const Data& newVersion);

        ///
        void parseVersion( const Data& newVer);

        ///
        const Data& getProtocol();

        ///
        void setProtocol( const Data& newProtocol);

        ///
        void setStatusLine( const Data& newstatusLine );

        ///
        const Data& getStatusLine();

        ///
        Data encode() const;

        ///
        void setDefaultReason(Data findStatusCode);

    private:

        void setReasonPhrase(int code);
        void decode(const Data& data);

        Data statusCode;  // may be accessed as an int or as Data
        Data reasonPhrase;
        Data version;
        Data protocol;
        Data statusLine;  //returned during an encode, no accessors
        friend class StatusMsg;
};

 
}

#endif
