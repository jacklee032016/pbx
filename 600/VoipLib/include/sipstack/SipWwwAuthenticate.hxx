#ifndef SipWwwAuthenticate_HXX
#define SipWwwAuthenticate_HXX
/*
 * $Id: SipWwwAuthenticate.hxx,v 1.2 2007/03/01 20:04:28 lizhijie Exp $
 */


#include "global.h"

static const char* const SipWwwAuthenticateVersion =
    "$Id: SipWwwAuthenticate.hxx,v 1.2 2007/03/01 20:04:28 lizhijie Exp $";



#include "Data.hxx"
#include <map>
#include "Sptr.hxx"
#include "SipHeader.hxx"
#include "SipParameterList.hxx"

namespace Vocal
{


enum SipWwwAuthenticateErrorType
{
    DECODE_FAILED_WWWAUTHENTICATE

    //may need to change this to be more specific
};

/// Exception handling class
class SipWwwAuthenticateParserException : public VException
{
    public:
        SipWwwAuthenticateParserException( const string& msg,
                                             const string& file,
                                             const int line,
                                             const int error = 0 );

        SipWwwAuthenticateParserException( const string& msg,
                                             const string& file,
                                             const int line,
                                             SipWwwAuthenticateErrorType i)
        : VException( msg, file, line, static_cast < int > (i))
        {
            value = i;
        }

        ///
        SipWwwAuthenticateErrorType
        getError() const
        {
            return value;
        }

        ///
        string
        getName() const ;

    private:
        SipWwwAuthenticateErrorType value;

};



/// data container for WwwAuthenticate header
class SipWwwAuthenticate : public SipHeader
{
    public:
        /// Create one with default values
        SipWwwAuthenticate();

        // TODO - add more constructors that take the parameters needed to build it

        /*** Create by decoding the data string passed in. This is the decode
             or parse. This call should only be used inside the stack and is
             not part of the API */
        SipWwwAuthenticate( const Data& srcData );

        ///
        SipWwwAuthenticate( const SipWwwAuthenticate& src );

        ///
        SipWwwAuthenticate&
        operator = (const SipWwwAuthenticate& src);

        ///
        bool operator ==(const SipWwwAuthenticate& src) const;
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
        scanSipWwwauthorization(const Data& data);


        Data authScheme;
        SipParameterList myParamList;   //contains tokens, and tokenValues.
    
        friend class StatusMsg;
};

 
} // namespace Vocal

#endif
