#ifndef SipOspAuthorizationToken_HXX
#define SipOspAuthorizationToken_HXX

/*
* $Log: SipOspAuthorization.hxx,v $
* Revision 1.1.1.1  2006/11/30 16:26:29  lizhijie
* AS800 VPBX
*
* Revision 1.1  2006/06/03 12:06:38  lizhijie
* no message
*
* $Id: SipOspAuthorization.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "global.h"
#include "Data.hxx"
#include "SipHeader.hxx"
#include "VException.hxx"

namespace Assist
{


enum SipOspAuthorizationErrorType
{
    DECODE_SIPOSPPARSER_FAILED

    //may need to change this to be more specific
};

/// Exception handling class
class SipOspAuthorizationParserException : public VException
{
    public:
        SipOspAuthorizationParserException( const string& msg,
                                            const string& file,
                                            const int line,
                                            const int error = 0 );
        SipOspAuthorizationParserException( const string& msg,
                                            const string& file,
                                            const int line,
                                            SipOspAuthorizationErrorType i)
        : VException( msg, file, line, static_cast < int > (i))
        {
            value = i;
        }
        SipOspAuthorizationErrorType getError() const
        {
            return value;
        }
        string getName() const ;
    private:
        SipOspAuthorizationErrorType value;

};

/// data container for the OspAuthorization header
class SipOspAuthorization : public SipHeader
{
    public:
        /// Create one with default values
        SipOspAuthorization();

        // TODO - add more constructors that take the parameters needed to build it

        // TODO - add accessor functions to get at the sub components

        /*** Create by decoding the data string passed in. This is the decode
        or parse. This call should only be used inside the stack and is
        not part of the API */
        SipOspAuthorization( const Data& srcData );
        ///
        SipOspAuthorization( const SipOspAuthorization& src );
        ///
        SipOspAuthorization& operator=( const SipOspAuthorization& src );
        
        ///
        bool operator==( const SipOspAuthorization& src ) const;
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
        friend class SipCommand;
        friend class StatusMsg;
};

 
}

#endif
