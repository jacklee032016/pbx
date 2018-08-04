/*
 * $Id: SipAuthorization.cxx,v 1.2 2007/03/01 20:04:31 lizhijie Exp $
 */

#include "global.h"
#include "SipAuthorization.hxx"
#include "symbols.hxx"
#include "SipParserMode.hxx"
#include "cpLog.h"

using namespace Vocal;

string
SipAuthorizationParserException::getName( void ) const
{
    return "SipAuthorizationParserException";
}


SipAuthorization::SipAuthorization()
    : myParamList(',')
{
}


SipAuthorization::SipAuthorization( const Data& srcData )
    : myAuthScheme(),
      myParamList(',')
{
    Data fdata = srcData;
    try
    {
        decode(fdata);
    }
    catch (SipAuthorizationParserException&)
    {
        if (SipParserMode::sipParserMode())
        {

            throw SipAuthorizationParserException(
                "failed to decode the Authorization header",
                __FILE__,
                __LINE__,
                DECODE_FAILED_AUTHORIZE);
        }
    }
}


SipAuthorization::SipAuthorization( const SipAuthorization& src )
        : myAuthScheme(src.myAuthScheme),
          myBasicCookie(src.myBasicCookie),
          myParamList(src.myParamList)
{
}


SipAuthorization& 
SipAuthorization::operator =( const SipAuthorization& src)
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
SipAuthorization::operator ==( const SipAuthorization& src) const
{
   return ( myParamList == src.myParamList && 
            isEqualNoCase(myAuthScheme, src.myAuthScheme ) &&
            isEqualNoCase(myBasicCookie, src.myBasicCookie )  );
}


void 
SipAuthorization::decode(const Data& data)
{
    Data nData = data;

    try
    {
        scanSipAuthorization(nData);
    }
    catch (SipAuthorizationParserException exception)
    {
        if (SipParserMode::sipParserMode())
        {
            throw SipAuthorizationParserException(
                exception.getDescription(),
                __FILE__,
                __LINE__,
                DECODE_FAILED_AUTHORIZE
            );
        }
    }
}


void
SipAuthorization::scanSipAuthorization(const Data& tmpdata)
{
    Data authdetails = tmpdata ;
    Data authType;
    int ret = authdetails.match(" ", &authType, true);
    if (ret == FIRST)
    {
        if (SipParserMode::sipParserMode())
        {
            throw SipAuthorizationParserException(
                "failed to decode the Authorization header",
                __FILE__,
                __LINE__,
                DECODE_FAILED_AUTHORIZE);
        }
    }

    if (ret == FOUND)
    {
        if (  (isEqualNoCase( authType , AUTH_BASIC )) ||
              (isEqualNoCase( authType , AUTH_DIGEST )) ||
              (isEqualNoCase( authType ,AUTH_PGP ))
           )
        {

            setAuthScheme(authType);

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
            catch (SipAuthorizationParserException&)
            {
                if (SipParserMode::sipParserMode())
                {
                    throw (SipAuthorizationParserException(
                               "failed in parsing auth tokens",
                               __FILE__,
                               __LINE__,
                               DECODE_FAILED_AUTHORIZE));
                }
            }

        }
        else
        {
            if (SipParserMode::sipParserMode())
            {
                throw SipAuthorizationParserException(
                    "auth scheme incorrect.",
                    __FILE__,
                    __LINE__,
                    DECODE_FAILED_AUTHORIZE);
            }
        }

    }
    if (ret == NOT_FOUND)
    {
        throw SipAuthorizationParserException(
            "No AuthScheme",
            __FILE__,
            __LINE__,
            DECODE_FAILED_AUTHORIZE);
    }

}


/*** return the encoded string version of this. This call should only be
     used inside the stack and is not part of the API */
Data 
SipAuthorization::encode() const
{
    Data data;

    if ( myAuthScheme.length() )
    {
        data += AUTHORIZATION;
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
SipAuthorization::setAuthScheme(const Data& data)
{
    myAuthScheme = data;
}


Data 
SipAuthorization::getAuthScheme() const
{
    return myAuthScheme;
}


void
SipAuthorization::setBasicCookie(const Data& data)
{
    myBasicCookie = data;
}


Data
SipAuthorization::getBasicCookie() const
{
    return myBasicCookie;
}


void 
SipAuthorization::setTokenDetails(const Data& paramName,
                             const Data& paramValue)
{
    myParamList.setValue(paramName, paramValue);
}


Data 
SipAuthorization::getTokenValue(const Data& token) const
{
    Data ret;
    //Strip off any "" from the value
    string tokenstr = myParamList.getValue(token).convertString();
    int pos;
    pos = tokenstr.find("\"");

    if (pos != static_cast<int>(string::npos))
    {
         tokenstr = tokenstr.substr(pos + 1, tokenstr.length() - 2);
    }

    ret = tokenstr;
    return (ret);
}


SipHeader*
SipAuthorization::duplicate() const
{
    return new SipAuthorization(*this);
}


bool
SipAuthorization::compareSipHeader(SipHeader* msg) const
{
    SipAuthorization* otherMsg = dynamic_cast<SipAuthorization*>(msg);
    if(otherMsg != 0)
    {
	return (*this == *otherMsg);
    }
    else
    {
	return false;
    }
}
