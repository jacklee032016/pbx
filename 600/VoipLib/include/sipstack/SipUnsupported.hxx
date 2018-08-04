#ifndef SipUnsupported_HXX
#define SipUnsupported_HXX
/*
 * $Id: SipUnsupported.hxx,v 1.2 2007/03/01 20:04:28 lizhijie Exp $
 */

#include "global.h"

#include "Data.hxx"
#include "SipHeader.hxx"

namespace Vocal
{
    
enum SipUnsupportedErrorType
{
    DECODE_UNSUPPORTED_FAILED

    //may need to change this to be more specific
};

/// Exception handling class
class SipUnsupportedParserException : public VException
{
    public:
        SipUnsupportedParserException( const string& msg,
                                       const string& file,
                                       const int line,
                                       const int error = 0 );
        SipUnsupportedParserException( const string& msg,
                                       const string& file,
                                       const int line,
                                       SipUnsupportedErrorType i)
        : VException( msg, file, line, static_cast < int > (i))
        {
            value = i;
        }
        SipUnsupportedErrorType getError() const
        {
            return value;
        }
        string getName() const ;
    private:
        SipUnsupportedErrorType value;

};

/// data container for Unsupported header
class SipUnsupported : public SipHeader
{
    public:
        /// Create one with default values
        SipUnsupported();

        // TODO - add more constructors that take the parameters needed to build it

        // TODO - add accessor functions to get at the sub components

        /*** Create by decoding the data string passed in. This is the decode
        or parse. This call should only be used inside the stack and is
        not part of the API */
        SipUnsupported( const Data& srcData );
        ///
        SipUnsupported( const SipUnsupported& src );
        ///
        SipUnsupported& operator=( const SipUnsupported& src );
        ///      
        bool operator==( const SipUnsupported& src ) const;
        ///
        Data get();

        ///
        void set( const Data& newData);

        /*** return the encoded string version of this. This call should only be
        used inside the stack and is not part of the API */
        Data encode() const;

	/// method for copying sip headers of any type without knowing which type
	SipHeader* duplicate() const;
	/// compare two headers of (possibly) the same class
	virtual bool compareSipHeader(SipHeader* msg) const;
    private:
        Data data;
        void decode(const Data& data);
};

 
} // namespace Vocal

#endif
