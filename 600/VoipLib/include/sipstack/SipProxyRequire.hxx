#ifndef SipProxyRequire_HXX
#define SipProxyRequire_HXX
/*
 * $Id: SipProxyRequire.hxx,v 1.2 2007/03/01 20:04:28 lizhijie Exp $
 */

#include "global.h"

#include "Data.hxx"
#include "SipHeader.hxx"
#include "VException.hxx"

namespace Vocal
{


enum SipProxyRequireErrorType
{
    DECODE_PROXYREQUIRE_FAILED

    //may need to change this to be more specific
};

/// Exception handling class
class SipProxyRequireParserException : public VException
{
    public:
        SipProxyRequireParserException( const string& msg,
                                        const string& file,
                                        const int line,
                                        const int error = 0 );
        SipProxyRequireParserException( const string& msg,
                                        const string& file,
                                        const int line,
                                        SipProxyRequireErrorType i)
        : VException( msg, file, line, static_cast < int > (i))
        {
            value = i;
        }
        SipProxyRequireErrorType getError() const
        {
            return value;
        }
        string getName() const ;
    private:
        SipProxyRequireErrorType value;

};


/// data container for ProxyRequire header
class SipProxyRequire : public SipHeader
{
    public:
        /// Create one with default values
        SipProxyRequire();

        // TODO - add more constructors that take the parameters needed to build it

        // TODO - add accessor functions to get at the sub components

        /*** Create by decoding the data string passed in. This is the decode
             or parse. This call should only be used inside the stack and is
             not part of the API */
        SipProxyRequire( const Data& srcData );
        SipProxyRequire( const SipProxyRequire& src );
        SipProxyRequire& operator=(const SipProxyRequire& src);
        bool operator==( const SipProxyRequire& src) const;
///
        Data get() const;

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
