/*
* $Log: SipWwwAuthenticate.cxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:10  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: SipWwwAuthenticate.cxx,v 1.1.1.1 2006/11/30 16:27:10 lizhijie Exp $
*/

#include <iostream>

#include "global.h"
#include "symbols.hxx"
#include "cpLog.h"
#include "SipWwwAuthenticate.hxx"
#include "SipParserMode.hxx"

using namespace Assist;


string
SipWwwAuthenticateParserException::getName( void ) const
{
    return "SipWwwAuthenticateParserException";
}


SipWwwAuthenticate::SipWwwAuthenticate()
    : myParamList(',')
{
}


SipWwwAuthenticate::SipWwwAuthenticate( const SipWwwAuthenticate& src )
        : authScheme(src.authScheme),
        myParamList(src.myParamList)
{
}


SipWwwAuthenticate::SipWwwAuthenticate( const Data& srcData )
        : SipHeader(srcData), 
          authScheme(),
          myParamList(',')
{
    Data fdata = srcData;
    try
    {
        decode(fdata);
    }
    catch (SipWwwAuthenticateParserException&)
    {
        if (SipParserMode::sipParserMode())
        {

            throw SipWwwAuthenticateParserException(
                "failed to decode the ProxyAuthenticate string",
                __FILE__,
                __LINE__,
                DECODE_FAILED_WWWAUTHENTICATE);
        }
    }
}


SipWwwAuthenticate&
SipWwwAuthenticate::operator = (const SipWwwAuthenticate& src)
{
    if (&src != this)
    {
        authScheme = src.authScheme;
        myParamList= src.myParamList;
    }
    return (*this);
}


bool
SipWwwAuthenticate::operator ==(const SipWwwAuthenticate& src) const
{
    return ( ( isEqualNoCase(authScheme,src.authScheme) ) &&
             ( myParamList == src.myParamList) );
}


void
SipWwwAuthenticate::decode(const Data& data)
{
    Data nData = data;

    try
    {
        scanSipWwwauthorization(nData);
    }
    catch (SipWwwAuthenticateParserException exception)
    {
        if (SipParserMode::sipParserMode())
        {

            throw SipWwwAuthenticateParserException(
                exception.getDescription(),
                __FILE__,
                __LINE__,
                DECODE_FAILED_WWWAUTHENTICATE
            );
        }
    }
}


void
SipWwwAuthenticate::scanSipWwwauthorization(const Data& tmpdata)
{
    Data authdetails = tmpdata ;
    Data authType;
    int ret = authdetails.match(" ", &authType, true);
    if (ret == FIRST)
    {
        if (SipParserMode::sipParserMode())
        {

            throw SipWwwAuthenticateParserException(
                "failed to decode the ProxyAuthenticate string",
                __FILE__,
                __LINE__,
                DECODE_FAILED_WWWAUTHENTICATE);
        }

    }
    else if (ret == FOUND)
    {
        setAuthScheme(authType);
        cpDebug(LOG_DEBUG_STACK,"Found aunth sceme [%s]", authType.logData());

        if ( ( isEqualNoCase(authType,AUTH_BASIC)) ||
             ( isEqualNoCase(authType,AUTH_DIGEST)) ||
             ( isEqualNoCase(authType,AUTH_PGP ) )
            )
        {
            try
            {
                myParamList.decode(authdetails, ',');
            }
            catch (SipWwwAuthenticateParserException&)
            {
                if (SipParserMode::sipParserMode())
                {
                    throw (SipWwwAuthenticateParserException(
                        "failed in parsing auth tokens",
                        __FILE__,
                                   __LINE__,
                        DECODE_FAILED_WWWAUTHENTICATE));
                }
            }
        }
        
    }
    else if (ret == NOT_FOUND)
    {
        throw SipWwwAuthenticateParserException(
            "No AuthScheme",
            __FILE__,
            __LINE__,
            DECODE_FAILED_WWWAUTHENTICATE);
    }

}



/*** return the encoded string version of this. This call should only be
     used inside the stack and is not part of the API */
Data
SipWwwAuthenticate::encode() const
{
    Data data;

    if ( authScheme.length() )
    {
        data += SIP_WWWAUTHENTICATE;
        data += SP;
        data += authScheme;
        data += SP;

        data += myParamList.encode();

        data += CRLF;
    }

    return data;
}


///
void
SipWwwAuthenticate::setAuthTokenDetails(const Data& token,
        const Data& tokenValue)
{
    myParamList.setValue(token, tokenValue);
}


///
void
SipWwwAuthenticate::setRealmValue(const Data& data)
{
    myParamList.setValue(REALM, data);
}


///
Data
SipWwwAuthenticate::getTokenValue(const Data& token) const
{
    Data ret;
    //Strip off any "" from the value
    string tokenstr = myParamList.getValue(token).convertString();
    int pos;
    pos = tokenstr.find("\"");

    if (pos != static_cast<int>(string::npos))
    {
         tokenstr = tokenstr.substr(pos + 1, tokenstr.length() - 2);                }

    ret = tokenstr;

    return ret;
}


///
Data
SipWwwAuthenticate::getRealmValue() const
{
    return getTokenValue(REALM);
}



SipHeader*
SipWwwAuthenticate::duplicate() const
{
    return new SipWwwAuthenticate(*this);
}


bool
SipWwwAuthenticate::compareSipHeader(SipHeader* msg) const
{
    SipWwwAuthenticate* otherMsg = dynamic_cast<SipWwwAuthenticate*>(msg);
    if(otherMsg != 0)
    {
	return (*this == *otherMsg);
    }
    else
    {
	return false;
    }
}
