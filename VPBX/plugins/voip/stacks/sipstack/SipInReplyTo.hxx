#ifndef SipInReplyTo_HXX
#define SipInReplyTo_HXX

/*
* $Log: SipInReplyTo.hxx,v $
* Revision 1.1.1.1  2006/11/30 16:26:29  lizhijie
* AS800 VPBX
*
* Revision 1.1  2006/06/03 12:06:38  lizhijie
* no message
*
* $Id: SipInReplyTo.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "global.h"
#include "SipCallId.hxx"
#include "Data.hxx"
#include "SipHeader.hxx"
#include "VException.hxx"

namespace Assist
{


enum SipInReplyToErrorType
{
    DECODE_INREPLYTO_FAILED

    //may need to change this to be more specific
};

/// Exception handling class SipInReplyToParserException
class SipInReplyToParserException : public VException
{
    public:
        SipInReplyToParserException( const string& msg,
                                     const string& file,
                                     const int line,
                                     const int error = 0 );
        SipInReplyToParserException( const string& msg,
                                     const string& file,
                                     const int line,
                                     SipInReplyToErrorType i)
        : VException( msg, file, line, static_cast < int > (i))
        {
            value = i;
        }
        SipInReplyToErrorType getError() const
        {
            return value;
        }
        string getName() const ;
    private:
        SipInReplyToErrorType value;
};

class SipCallId;
/// data container for SipInReplyTo header
class SipInReplyTo : public SipHeader
{

    public:
        /// Create one with default values
        SipInReplyTo();

        // TODO - add more constructors that take the parameters needed to build it

        /*** Create by decoding the data string passed in. This is the decode
             or parse. This call should only be used inside the stack and is
             not part of the API */
        SipInReplyTo( const Data& src );
        SipInReplyTo( const SipInReplyTo& src );
        SipInReplyTo& operator=(const SipInReplyTo& src);
        bool operator ==(const SipInReplyTo & src) const;
        void scanSipInReplyTo(const Data& cdata);
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

        SipCallId inreplyto;
        mutable Data calliddata;
        void decode(const Data& data);
};

 
}

#endif
