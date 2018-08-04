#ifndef SipAcceptEncoding_HXX
#define SipAcceptEncoding_HXX
/*
* $Log: SipAcceptEncoding.hxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:09  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: SipAcceptEncoding.hxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#include "global.h"
#include "Data.hxx"
#include "symbols.hxx"
#include "SipHeader.hxx"
#include "VException.hxx"

namespace Assist
{

enum SipAcceptEncodingErrorType
{
    DECODE_ACCEPTENCODING_FAILED

    //may need to change this to be more specific
};

/// Exception handling class 
class SipAcceptEncodingParserException : public VException
{
    public:
        SipAcceptEncodingParserException( const string& msg,
                                          const string& file,
                                          const int line,
                                          const int error = 0 );
        SipAcceptEncodingParserException( const string& msg,
                                          const string& file,
                                          const int line,
                                          SipAcceptEncodingErrorType i)
        : VException( msg, file, line, static_cast < int > (i))
        {
            value = i;
        }
        SipAcceptEncodingErrorType getError() const
        {
            return value;
        }
        string getName() const ;
    private:
        SipAcceptEncodingErrorType value;

};

/// data container for AcceptEncoding header
class SipAcceptEncoding : public SipHeader
{
    public:
        /// Create one with default values
        SipAcceptEncoding();

        SipAcceptEncoding( const SipAcceptEncoding& src);

        ///
        const SipAcceptEncoding& operator = (const SipAcceptEncoding& src);

        bool operator ==( const SipAcceptEncoding& src) const;


        ///Create by decoding the data string passed in. This is the decode or parse.
        SipAcceptEncoding( const Data& srcData );

      
        ///
        Data getContentCoding() const;
        ///
        void setContentCoding(const Data& srcContentCoding);
        ///
        Data getqValue() const;
        ///
        void setqValue(const Data& srcqValue);
        /// return the encoded string.
        Data encode() const;
	/// method for copying sip headers of any type without knowing which type
	SipHeader* duplicate() const;

	/// compare two headers of (possibly) the same class
	virtual bool compareSipHeader(SipHeader* msg) const;
    private:
        
        ///
        void scanAcceptEncoding(const Data &tmpdata);
        ///
        void decode(const Data& data);
        Data contentCoding;
        Data qValue;
};
 
}

#endif
