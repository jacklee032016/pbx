#ifndef SipWarning_HXX
#define SipWarning_HXX
/*
* $Log: SipWarning.hxx,v $
* Revision 1.1.1.1  2006/11/30 16:26:29  lizhijie
* AS800 VPBX
*
* Revision 1.1  2006/06/03 12:06:38  lizhijie
* no message
*
* $Id: SipWarning.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "global.h"

#include "Data.hxx"
#include "SipHeader.hxx"

namespace Assist
{
    
///
enum SipWarningErrorType
{
    DECODE_WARNING_FAILED,
    SCAN_WARNING_FAILED
    //may need to change this to be more specific
};

///
class SipWarningParserException : public VException
{
    public:
        ///
        SipWarningParserException( const string& msg,
                                   const string& file,
                                   const int line,
                                   const int error = 0 );

        ///
        SipWarningParserException( const string& msg,
                                   const string& file,
                                   const int line,
                                   SipWarningErrorType i )
        : VException( msg, file, line, static_cast < int > ( i ) )
        {
            value = i;
        }
        ///
        SipWarningErrorType getError() const
        {
            return value;
        }
        ///
        string getName() const ;

    private:
        SipWarningErrorType value;

};


/// data container for Warning header
class SipWarning : public SipHeader
{
    public:
        /// Create one with default values
        SipWarning();

        // TODO - add more constructors that take the parameters needed to build it

        // TODO - add accessor functions to get at the sub components

        /*** Create by decoding the data string passed in. This is the decode
        or parse. This call should only be used inside the stack and is
        not part of the API */
        SipWarning( const Data& srcData );
        SipWarning( const SipWarning& src );

        ///
        const SipWarning& operator=( const SipWarning& src );

       ///
       bool operator ==(const SipWarning & src) const;

        ///
        int getCode();

        ///
        void setCode( const int& newCode );

        ///
        void scanWarning( const Data &tmpData );
        ///
        Data getCodeData();

        ///
        void setCodeData( const Data& newCodeData );

        ///
        Data getAgent();

        ///
        void setAgent( const Data& newAgent );

        ///
        Data getText();

        ///
        void setText( const Data& newText );


        /*** return the encoded string version of this. This call should only be
        used inside the stack and is not part of the API */
        Data encode() const;

	/// method for copying sip headers of any type without knowing which type
	SipHeader* duplicate() const;
	/// compare two headers of (possibly) the same class
	virtual bool compareSipHeader(SipHeader* msg) const;
    private:
        Data code;  // may be accessed as int or Data
        Data agent;
        Data text;
        void decode( const Data& retryData );
};

 
} // namespace Assist

#endif
