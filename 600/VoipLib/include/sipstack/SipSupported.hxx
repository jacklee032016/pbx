#ifndef SipSupported_HXX
#define SipSupported_HXX
/*
 * $Id: SipSupported.hxx,v 1.2 2007/03/01 20:04:28 lizhijie Exp $
 */

#include "global.h"

#include "Data.hxx"
#include "SipHeader.hxx"

namespace Vocal
{
    
enum SipSupportedErrorType
{
    DECODE_SUPPORTED_FAILED


    //may need to change this to be more specific
};

/// Exception handling class
class SipSupportedParserException : public VException
{
    public:
        SipSupportedParserException( const string& msg,
                                   const string& file,
                                   const int line,
                                   const int error = 0 );
        SipSupportedParserException( const string& msg,
                                   const string& file,
                                   const int line,
                                   SipSupportedErrorType i)
        : VException( msg, file, line, static_cast < int > (i))
        {
            value = i;
        }
        SipSupportedErrorType getError() const
        {
            return value;
        }
        string getName() const ;
    private:
        SipSupportedErrorType value;

};
/// data container for Supported header
class SipSupported : public SipHeader
{
    public:
        /// Create one with default values
        SipSupported();

        /*** Create by decoding the data string passed in. This is the decode
             or parse. This call should only be used inside the stack and is
             not part of the API */
        SipSupported( const Data& srcData );
        SipSupported( const SipSupported& src );
        void scanSipSupported(const Data& data);
        ///
        Data get();

        ///
        void set( const Data& newData);
        ///
        SipSupported& operator=(const SipSupported& src);
        /*** return the encoded string version of this. This call should only be
             used inside the stack and is not part of the API */
        Data encode() const;
        ///
        bool operator==(const SipSupported&) const;

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
