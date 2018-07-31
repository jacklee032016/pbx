#ifndef SipProxyAuthenticate_HXX
#define SipProxyAuthenticate_HXX
/*
* $Log: SipProxyAuthenticate.hxx,v $
* Revision 1.1.1.1  2006/11/30 16:26:29  lizhijie
* AS800 VPBX
*
* Revision 1.1  2006/06/03 12:06:38  lizhijie
* no message
*
* $Id: SipProxyAuthenticate.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "global.h"

#include "Data.hxx"
#include <map>
#include "Sptr.hxx"
#include "SipHeader.hxx"
#include "SipParameterList.hxx"
#include "VException.hxx"

namespace Assist
{


enum SipProxyAuthenticateErrorType
{
    DECODE_FAILED_PROXYAUTHENTICATE


    //may need to change this to be more specific
};

/// Exception handling class
class SipProxyAuthenticateParserException : public VException
{
    public:
        SipProxyAuthenticateParserException( const string& msg,
                                             const string& file,
                                             const int line,
                                             const int error = 0 );

        SipProxyAuthenticateParserException( const string& msg,
                                             const string& file,
                                             const int line,
                                             SipProxyAuthenticateErrorType i)
        : VException( msg, file, line, static_cast < int > (i))
        {
            value = i;
        }

        ///
        SipProxyAuthenticateErrorType
        getError() const
        {
            return value;
        }

        ///
        string
        getName() const ;

    private:
        SipProxyAuthenticateErrorType value;

};



/// data container for ProxyAuthenticate header
class SipProxyAuthenticate : public SipHeader
{
    public:
        /// Create one with default values
        SipProxyAuthenticate();

        // TODO - add more constructors that take the parameters needed to build it

        /*** Create by decoding the data string passed in. This is the decode
             or parse. This call should only be used inside the stack and is
             not part of the API */
        SipProxyAuthenticate( const Data& srcData );

        ///
        SipProxyAuthenticate( const SipProxyAuthenticate& src );

        ///
        SipProxyAuthenticate&
        operator = (const SipProxyAuthenticate& src);

        ///
        bool operator ==(const SipProxyAuthenticate& src) const;
        ///
        void
        setAuthScheme(const Data & data)
        {
            authScheme = data;
        }

        ///
        Data
        getAuthScheme() const
        {
            return authScheme;
        }

        ///
        void
        setAuthTokenDetails(const Data& token, const Data& tokenValue);

        /*** return the encoded string version of this. This call should only be
             used inside the stack and is not part of the API */
        Data
        encode() const;

        ///
        void
        setRealmValue(const Data& realmValue);

        ///
        Data
        getTokenValue(const Data& token) const;

        ///
        Data
        getRealmValue() const;

	/// method for copying sip headers of any type without knowing which type
	SipHeader* duplicate() const;
	/// compare two headers of (possibly) the same class
	virtual bool compareSipHeader(SipHeader* msg) const;
    private:

        void
        decode(const Data& data);

        void
        scanSipProxyauthorization(const Data& data);


        Data authScheme;
        SipParameterList myParamList;   //contains tokens, and tokenValues.
    
        friend class StatusMsg;
};

 
}

#endif
