#ifndef SipRequire_HXX
#define SipRequire_HXX
/*
* $Log: SipRequire.hxx,v $
* Revision 1.1.1.1  2006/11/30 16:26:29  lizhijie
* AS800 VPBX
*
* Revision 1.1  2006/06/03 12:06:38  lizhijie
* no message
*
* $Id: SipRequire.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "global.h"

#include "Data.hxx"
#include "cpLog.h"
#include "SipHeader.hxx"
#include "VException.hxx"

namespace Assist
{



enum SipRequireErrorType
{
    DECODE_REQUIRE_FAILED

    //may need to change this to be more specific
};

/// Exception handling class
class SipRequireParserException : public VException
{
    public:
        SipRequireParserException( const string& msg,
                                   const string& file,
                                   const int line,
                                   const int error = 0 );
        SipRequireParserException( const string& msg,
                                   const string& file,
                                   const int line,
                                   SipRequireErrorType i)
        : VException( msg, file, line, static_cast < int > (i))
        {
            value = i;
        }
        SipRequireErrorType getError() const
        {
            return value;
        }
        string getName() const ;
    private:
        SipRequireErrorType value;
};
/// data container for Require header
class SipRequire : public SipHeader
{
    public:
        /// Create one with default values
        SipRequire();

        /*** Create by decoding the data string passed in. This is the decode
             or parse. This call should only be used inside the stack and is
             not part of the API */
        SipRequire( const Data& srcData );
        SipRequire( const SipRequire& src);
        SipRequire& operator=(const SipRequire& src);
        bool operator==( const SipRequire& src ) const;
        void scanSipRequire(const Data& data);
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

 
} 

#endif
