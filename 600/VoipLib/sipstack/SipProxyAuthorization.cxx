/*
 * $Id: SipProxyAuthorization.cxx,v 1.2 2007/03/01 20:04:31 lizhijie Exp $
 */

#include "global.h"
#include "SipProxyAuthorization.hxx"
#include "symbols.hxx"
#include "SipParserMode.hxx"
#include "cpLog.h"

using namespace Vocal;


string
SipProxyAuthorizationParserException::getName( void ) const
{
    return "SipProxyAuthorizationParserException";
}


SipProxyAuthorization::SipProxyAuthorization()
    :myParamList(',')
{
}


SipProxyAuthorization::SipProxyAuthorization( const Data& srcData )
    : myAuthScheme(),
      myParamList(',')
{
    Data fdata = srcData;
    try
    {
        decode(fdata);
    }
    catch (SipProxyAuthorizationParserException&)
    {
        if (SipParserMode::sipParserMode())
        {

            throw SipProxyAuthorizationParserException(
                "failed to decode the ProxyAuthorization header",
                __FILE__,
                __LINE__,
                DECODE_FAILED_PROXYAUTHORIZE);
        }
    }
}


SipProxyAuthorization::SipProxyAuthorization( const SipProxyAuthorization& src )
        : myAuthScheme(src.myAuthScheme),
          myBasicCookie(src.myBasicCookie),
          myParamList(src.myParamList)
{
}


const SipProxyAuthorization&
SipProxyAuthorization::operator = (const SipProxyAuthorization& src)
{
    if (this != &src)
    {
        myParamList = src.myParamList;
        myAuthScheme = src.myAuthScheme;
        myBasicCookie = src.myBasicCookie;
    }
    return *this;
}


bool
SipProxyAuthorization::operator == (const SipProxyAuthorization& src) const
{
   return ( (myParamList == src.myParamList ) && 
            isEqualNoCase(myAuthScheme, src.myAuthScheme ) &&
            isEqualNoCase(myBasicCookie, src.myBasicCookie ) );
}


void
SipProxyAuthorization::decode(const Data& data)
{
    Data nData = data;

    try
    {
        scanSipProxyauthorization(nData);
    }
    catch (SipProxyAuthorizationParserException exception)
    {
        if (SipParserMode::sipParserMode())
        {
            throw SipProxyAuthorizationParserException(
                exception.getDescription(),
                __FILE__,
                __LINE__,
                DECODE_FAILED_PROXYAUTHORIZE
            );
        }
    }
}


void
SipProxyAuthorization::scanSipProxyauthorization(const Data& tmpdata)
{
    Data authdetails = tmpdata ;
    Data authType;
    int ret = authdetails.match(" ", &authType, true);
    if (ret == FIRST)
    {
        if (SipParserMode::sipParserMode())
        {
            throw SipProxyAuthorizationParserException(
                "failed to decode the ProxyAuthorization header",
                __FILE__,
                __LINE__,
                DECODE_FAILED_PROXYAUTHORIZE);
        }
    }
    else if (ret == FOUND)
    {
        if (  (isEqualNoCase( authType , AUTH_BASIC )) ||
              (isEqualNoCase( authType , AUTH_DIGEST )) ||
              (isEqualNoCase( authType ,AUTH_PGP ))
           )
        {
            cpDebug(LOG_DEBUG_STACK, "Got Auth type of %s", authType.logData());
            myAuthScheme = authType;

            if (isEqualNoCase(authType , AUTH_BASIC))
            {
                //store cookie.
                setBasicCookie(authdetails);
                return ;
            }

            try
            {
                myParamList.decode(authdetails, ',', true);
            }
            catch (SipProxyAuthorizationParserException&)
            {
                if (SipParserMode::sipParserMode())
                {
                    throw (SipProxyAuthorizationParserException(
                               "failed in parsing auth tokens",
                               __FILE__,
                               __LINE__,
                               DECODE_FAILED_PROXYAUTHORIZE));
                }
            }

        }
        else
        {
            if (SipParserMode::sipParserMode())
            {
                throw SipProxyAuthorizationParserException(
                    "auth scheme incorrect.",
                    __FILE__,
                    __LINE__,
                    DECODE_FAILED_PROXYAUTHORIZE);
            }
        }

    }
    if (ret == NOT_FOUND)
    {
        throw SipProxyAuthorizationParserException(
            "No AuthScheme",
            __FILE__,
            __LINE__,
            DECODE_FAILED_PROXYAUTHORIZE);
    }
}


/*** return the encoded string version of this. This call should only be
     used inside the stack and is not part of the API */
Data
SipProxyAuthorization::encode() const
{
    Data data;

    if ( myAuthScheme.length() )
    {
        data += SIP_PROXYAUTHORIZATION;
        data += SP;
        data += myAuthScheme;
        data += SP;

        if (isEqualNoCase(myAuthScheme, AUTH_BASIC))
        {
            data += myBasicCookie;
        }
        else
        {
            data += myParamList.encode();
        }//other than basic.
        data += CRLF;
    }
    return data;
}


void 
SipProxyAuthorization::setAuthScheme(const Data& data)
{
    myAuthScheme = data;
}


Data 
SipProxyAuthorization::getAuthScheme() const
{
    return myAuthScheme;
}


void
SipProxyAuthorization::setBasicCookie(const Data& data)
{
    myBasicCookie = data;
}


Data
SipProxyAuthorization::getBasicCookie() const
{
    return myBasicCookie;
}


void
SipProxyAuthorization::setTokenDetails(const Data& paramName,
                                       const Data& paramValue)
{
    myParamList.setValue(paramName, paramValue);
}


Data 
SipProxyAuthorization::getTokenValue(const Data& token) const
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

    return (ret);
}


SipHeader*
SipProxyAuthorization::duplicate() const
{
    return new SipProxyAuthorization(*this);
}


bool
SipProxyAuthorization::compareSipHeader(SipHeader* msg) const
{
    SipProxyAuthorization* otherMsg = dynamic_cast<SipProxyAuthorization*>(msg);
    if(otherMsg != 0)
    {
	return (*this == *otherMsg);
    }
    else
    {
	return false;
    }
}
