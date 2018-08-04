/*
* $Log: SipProxyAuthenticate.cxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:09  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: SipProxyAuthenticate.cxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#include "global.h"
#include "cpLog.h"
#include "SipProxyAuthenticate.hxx"
#include "symbols.hxx"
#include "SipParserMode.hxx"

using namespace Assist;


string
SipProxyAuthenticateParserException::getName( void ) const
{
    return "SipProxyAuthenticateParserException";
}


SipProxyAuthenticate::SipProxyAuthenticate()
    : myParamList(',')
{
}


SipProxyAuthenticate::SipProxyAuthenticate( const SipProxyAuthenticate& src )
        : authScheme(src.authScheme),
        myParamList(src.myParamList)
{
}


SipProxyAuthenticate::SipProxyAuthenticate( const Data& srcData )
        : SipHeader(srcData), 
          authScheme(),
          myParamList(',')
{
    Data fdata = srcData;
    try
    {
        decode(fdata);
    }
    catch (SipProxyAuthenticateParserException&)
    {
        if (SipParserMode::sipParserMode())
        {

            throw SipProxyAuthenticateParserException(
                "failed to decode the ProxyAuthenticate string",
                __FILE__,
                __LINE__,
                DECODE_FAILED_PROXYAUTHENTICATE);
        }
    }
}


SipProxyAuthenticate&
SipProxyAuthenticate::operator = (const SipProxyAuthenticate& src)
{
    if (&src != this)
    {
        authScheme = src.authScheme;
        myParamList= src.myParamList;
    }
    return (*this);
}


bool
SipProxyAuthenticate::operator ==(const SipProxyAuthenticate& src) const
{
    return  ( isEqualNoCase(authScheme, src.authScheme) &&
              myParamList == src.myParamList);
}


void
SipProxyAuthenticate::decode(const Data& data)
{
    Data nData = data;

    try
    {
        scanSipProxyauthorization(nData);
    }
    catch (SipProxyAuthenticateParserException exception)
    {
        if (SipParserMode::sipParserMode())
        {

            throw SipProxyAuthenticateParserException(
                exception.getDescription(),
                __FILE__,
                __LINE__,
                DECODE_FAILED_PROXYAUTHENTICATE
            );
        }
    }
}


void
SipProxyAuthenticate::scanSipProxyauthorization(const Data& tmpdata)
{
    Data authdetails = tmpdata ;
    Data authType;
    int ret = authdetails.match(" ", &authType, true);
    if (ret == FIRST)
    {
        if (SipParserMode::sipParserMode())
        {

            throw SipProxyAuthenticateParserException(
                "failed to decode the ProxyAuthenticate string",
                __FILE__,
                __LINE__,
                DECODE_FAILED_PROXYAUTHENTICATE);
        }

    }
    else if (ret == FOUND)
    {
        setAuthScheme(authType);
        cpDebug(LOG_DEBUG_STACK,"Found aunth sceme [%s]", authType.logData());

        if ( isEqualNoCase(authType, AUTH_BASIC ) ||
             isEqualNoCase(authType, AUTH_DIGEST ) ||
             isEqualNoCase(authType, AUTH_PGP ) );
        {
            try
            {
                myParamList.decode(authdetails, ',');
            }
            catch (SipProxyAuthenticateParserException&)
            {
                if (SipParserMode::sipParserMode())
                {
                    throw (SipProxyAuthenticateParserException(
                        "failed in parsing auth tokens",
                        __FILE__,
                                   __LINE__,
                        DECODE_FAILED_PROXYAUTHENTICATE));
                }
            }
        }
        
    }
    else if (ret == NOT_FOUND)
    {
        throw SipProxyAuthenticateParserException(
            "No AuthScheme",
            __FILE__,
            __LINE__,
            DECODE_FAILED_PROXYAUTHENTICATE);
    }

}



/*** return the encoded string version of this. This call should only be
     used inside the stack and is not part of the API */
Data
SipProxyAuthenticate::encode() const
{
    Data data;

    if ( authScheme.length() )
    {
        data += SIP_PROXYAUTHENTICATE;
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
SipProxyAuthenticate::setAuthTokenDetails(const Data& token,
        const Data& tokenValue)
{
    myParamList.setValue(token, tokenValue);
}


///
void
SipProxyAuthenticate::setRealmValue(const Data& data)
{
    myParamList.setValue(REALM, data);
}


///
Data
SipProxyAuthenticate::getTokenValue(const Data& token) const
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
SipProxyAuthenticate::getRealmValue() const
{
    return getTokenValue(REALM);
}



SipHeader*
SipProxyAuthenticate::duplicate() const
{
    return new SipProxyAuthenticate(*this);
}


bool
SipProxyAuthenticate::compareSipHeader(SipHeader* msg) const
{
    SipProxyAuthenticate* otherMsg = dynamic_cast<SipProxyAuthenticate*>(msg);
    if(otherMsg != 0)
    {
	return (*this == *otherMsg);
    }
    else
    {
	return false;
    }
}
