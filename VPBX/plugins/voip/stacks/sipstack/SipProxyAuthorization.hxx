#ifndef SipProxyAuthorize_HXX
#define SipProxyAuthorize_HXX
/*
* $Log: SipProxyAuthorization.hxx,v $
* Revision 1.1.1.1  2006/11/30 16:26:29  lizhijie
* AS800 VPBX
*
* Revision 1.1  2006/06/03 12:06:38  lizhijie
* no message
*
* $Id: SipProxyAuthorization.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
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


enum SipProxyAuthorizationErrorType
{
    DECODE_FAILED_PROXYAUTHORIZE


    //may need to change this to be more specific
};


/// Exception handling class
class SipProxyAuthorizationParserException : public VException
{

    public:
        SipProxyAuthorizationParserException( const string& msg,
                                              const string& file,
                                              const int line,
                                              const int error = 0 );
        SipProxyAuthorizationParserException( const string& msg,
                                              const string& file,
                                              const int line,
                                              SipProxyAuthorizationErrorType i)
        : VException( msg, file, line, static_cast < int > (i))
        {
            value = i;
        }
        SipProxyAuthorizationErrorType getError() const
        {
            return value;
        }
        string getName() const ;
    private:
        SipProxyAuthorizationErrorType value;

};


/// data container for ProxyAuthorization header
class SipProxyAuthorization : public SipHeader
{
    public:
        /// Create ProxyAuthorization: header with default values
        SipProxyAuthorization();

        /// assignment operator
        const SipProxyAuthorization&
        operator = (const SipProxyAuthorization& src);
  
        /// equality operator
        bool operator == (const SipProxyAuthorization& src) const;

        /*** Create by decoding the data string passed in. This is the decode
        or parse. This call should only be used inside the stack and is
        not part of the API */
        SipProxyAuthorization( const Data& srcData );
        ///
        SipProxyAuthorization( const SipProxyAuthorization& src );

        ///
        void setAuthScheme(const Data & data);

        ///
        Data getAuthScheme() const;

        ///
        void setTokenDetails(const Data& paramName,
                             const Data& paramValue);
        ///
        void setBasicCookie(const Data& encrData);
        ///
        Data getBasicCookie() const;

        /*** return the encoded string version of this. This call
             should only be used inside the stack and is not part of
             the API */
        Data encode() const;
        ///
        Data getTokenValue(const Data& token) const;


	/** method for copying sip headers of any type without knowing
            which type */
	SipHeader* duplicate() const;
	/// compare two headers of (possibly) the same class
	virtual bool compareSipHeader(SipHeader* msg) const;
    private:
        void decode(const Data& data);
        void scanSipProxyauthorization(const Data& data);

        Data myAuthScheme;
        Data myBasicCookie;
        SipParameterList myParamList;  // contains parameters

        friend class SipCommand;
};

 
}

#endif
