#ifndef SipAllow_HXX
#define SipAllow_HXX

/*
* $Log: SipAllow.hxx,v $
* Revision 1.1.1.1  2006/11/30 16:26:29  lizhijie
* AS800 VPBX
*
* Revision 1.1  2006/06/03 12:06:38  lizhijie
* no message
*
* $Id: SipAllow.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "global.h"

#include "Data.hxx"
#include "SipHeader.hxx"
#include "VException.hxx"

namespace Assist
{

enum SipAllowErrorType
{
    DECODE_ALLOW_FAILED

    //may need to change this to be more specific
};

/// Exception handling class 
class SipAllowParserException : public VException
{
    public:
        SipAllowParserException( const string& msg,
                                 const string& file,
                                 const int line,
                                 const int error = 0 );
        SipAllowParserException( const string& msg,
                                 const string& file,
                                 const int line,
                                 SipAllowErrorType i)
        : VException( msg, file, line, static_cast < int > (i))
        {
            value = i;
        }
        SipAllowErrorType getError() const
        {
            return value;
        }
        string getName() const ;
    private:
        SipAllowErrorType value;

};

/// data container for Allow header
class SipAllow : public SipHeader
{
    public:
        /// Create one with default values
        SipAllow();
       ///Create by decoding the data string passed in. This is the decode or parse.
        SipAllow( const Data& srcData );
        ///
        SipAllow( const SipAllow& src);
        ///
        const SipAllow& operator =(const SipAllow& src);
        ///
        void scanSipAllow(const Data& cdata);
        ///return the encoded string version of this.
        Data encode() const;
        ///
        bool operator ==( const SipAllow& src ) const;
       ///
       Data getAllowMethod() const;
       ///
       void setAllowMethod( const Data& newData ); 

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
