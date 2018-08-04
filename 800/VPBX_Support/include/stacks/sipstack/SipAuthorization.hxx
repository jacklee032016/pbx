#ifndef SIP_AUTHORIZATION_HXX_
#define SIP_AUTHORIZATION_HXX_

/*
* $Log: SipAuthorization.hxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:09  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: SipAuthorization.hxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
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

enum SipAuthorizationErrorType
{
    DECODE_FAILED_AUTHORIZE


    //may need to change this to be more specific
};

/// Exception handling class 
class SipAuthorizationParserException : public VException
{

    public:
        SipAuthorizationParserException( const string& msg,
                                         const string& file,
                                         const int line,
                                         const int error = 0 );
        SipAuthorizationParserException( const string& msg,
                                         const string& file,
                                         const int line,
                                         SipAuthorizationErrorType i)
        : VException( msg, file, line, static_cast < int > (i))
        {
            value = i;
        }
        SipAuthorizationErrorType getError() const
        {
            return value;
        }
        string getName() const ;
    private:
        SipAuthorizationErrorType value;

};


/// data container for Authorization header
class SipAuthorization : public SipHeader
{
    public:
        /// Create Authorization: header with default values
        SipAuthorization();

        /// assignment operator 
        SipAuthorization& operator=(const SipAuthorization& src);
        ///
        bool operator ==( const SipAuthorization& src) const;

        /*** Create by decoding the data string passed in. This is the decode
             or parse. This call should only be used inside the stack and is
             not part of the API */
        SipAuthorization( const Data& srcData );
        ///
        SipAuthorization( const SipAuthorization& src );

        ///
        void setAuthScheme(const Data& data);

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
         *  which type */
	SipHeader* duplicate() const;

	/// compare two headers of (possibly) the same class
	virtual bool compareSipHeader(SipHeader* msg) const;
    private:
        void decode(const Data& data);
        void scanSipAuthorization(const Data& data);

        Data myAuthScheme;
        Data myBasicCookie;
        SipParameterList myParamList;   //contains parameters

        friend class SipCommand;
};
 
}

#endif
