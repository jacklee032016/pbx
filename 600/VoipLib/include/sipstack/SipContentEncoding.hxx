#ifndef SipContentEncoding_HXX
#define SipContentEncoding_HXX
/*
 * $Id: SipContentEncoding.hxx,v 1.2 2007/03/01 20:04:28 lizhijie Exp $
 */

#include "global.h"

#include "Data.hxx"
#include "SipHeader.hxx"
#include "VException.hxx"


namespace Vocal
{

enum SipContentEncodingErrorType
{
    DECODE_CONTENTENCODING_FAILED,
    DECODE_TOKEN_FAILED
    //may need to change this to be more specific
};

/// Exception handling class 
class SipContentEncodingParserException : public VException
{
    public:
        SipContentEncodingParserException( const string& msg,
                                           const string& file,
                                           const int line,
                                           const int error = 0 );
        SipContentEncodingParserException( const string& msg,
                                           const string& file,
                                           const int line,
                                           SipContentEncodingErrorType i)
        : VException( msg, file, line, static_cast < int > (i))
        {
            value = i;
        }
        SipContentEncodingErrorType getError() const
        {
            return value;
        }
        string getName() const ;
    private:
        SipContentEncodingErrorType value;

};

/// data container for ContentEncoding header
class SipContentEncoding : public SipHeader
{
    public:
        /// Create one with default values
        SipContentEncoding();

        // TODO - add more constructors that take the parameters needed to build it

        // TODO - add accessor functions to get at the sub components

        /*** Create by decoding the data string passed in. This is the decode
             or parse. This call should only be used inside the stack and is
             not part of the API */
        SipContentEncoding( const Data& newData );
        ///
        SipContentEncoding( const SipContentEncoding& src);

        ///
        Data get();
        ///
        void set( const Data& newData);
        ///
        void scanSipContentEncoding(const Data& data);
        /*** return the encoded string version of this. This call should only be
             used inside the stack and is not part of the API */
        Data encode() const;
        ///
        bool operator ==( const SipContentEncoding& src) const;
        ///
        const SipContentEncoding& operator=( const SipContentEncoding& src);

	/// method for copying sip headers of any type without knowing which type
	SipHeader* duplicate() const;
	/// compare two headers of (possibly) the same class
	virtual bool compareSipHeader(SipHeader* msg) const;
    private:
        ///
        Data data;
        ///
        void decode(const Data& data);
};
 
} // namespace Vocal

#endif
