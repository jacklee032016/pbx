#ifndef SipResponseKey_HXX
#define SipResponseKey_HXX
/*
* $Log: SipResponseKey.hxx,v $
* Revision 1.1.1.1  2006/11/30 16:26:29  lizhijie
* AS800 VPBX
*
* Revision 1.1  2006/06/03 12:06:38  lizhijie
* no message
*
* $Id: SipResponseKey.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "global.h"

#include "Data.hxx"
#include "SipHeader.hxx"
#include "VException.hxx"

namespace Assist
{

enum SipResponseKeyErrorType
{
    DECODE_RESPONSEKEY_FAILED

    //may need to change this to be more specific
};

/// Exception handling class
class SipResponseKeyParserException : public VException
{
    public:
        SipResponseKeyParserException( const string& msg,
                                       const string& file,
                                       const int line,
                                       const int error = 0 );
        SipResponseKeyParserException( const string& msg,
                                       const string& file,
                                       const int line,
                                       SipResponseKeyErrorType i)
        : VException( msg, file, line, static_cast < int > (i))
        {
            value = i;
        }
        SipResponseKeyErrorType getError() const
        {
            return value;
        }
        string getName() const ;
    private:
        SipResponseKeyErrorType value;

};

/// data container for ResponseKey header
class SipResponseKey : public SipHeader
{
    public:
        /// Create one with default values
        SipResponseKey();

        // TODO - add more constructors that take the parameters needed to build it

        /*** Create by decoding the data string passed in. This is the decode
             or parse. This call should only be used inside the stack and is
             not part of the API */
        SipResponseKey( const Data& srcData );
        SipResponseKey( const SipResponseKey& src);
        SipResponseKey& operator=(const SipResponseKey& src);
        bool operator==( const SipResponseKey& src) const;
        ///
        Data getKey();

        ///
        void setKey( const Data& newKey);

        ///
        Data getParam();

        ///
        void setParam( const Data& newParam);

        /*** return the encoded string version of this. This call should only be
             used inside the stack and is not part of the API */
        Data encode() const;

	/// method for copying sip headers of any type without knowing which type
	SipHeader* duplicate() const;
	/// compare two headers of (possibly) the same class
	virtual bool compareSipHeader(SipHeader* msg) const;
    private:
        Data key;
        Data param;
        void decode(const Data& data);
        void parseKeyParm(const Data& keyval);
};

 
}

#endif
