/*
* $Log: SipEncryption.cxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:09  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: SipEncryption.cxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#include "global.h"
#include "SipEncryption.hxx"
#include "SipParserMode.hxx"
#include "cpLog.h"

using namespace Assist;

string
SipEncryptionParserException::getName( void ) const
{
    return "SipEncryptionParserException";
}

SipEncryption::SipEncryption()
{}


SipEncryption::SipEncryption( const Data& srcData )
{
    try
    {
        decode(srcData);
    }
    catch (SipEncryptionParserException&)
    {
        if (SipParserMode::sipParserMode())
        {
            throw SipEncryptionParserException(
                "failed to decode the Encryption string",
                __FILE__,
                __LINE__, DECODE_ENCRYPTION_FAILED);
        }
    }
}


void SipEncryption::decode(const Data& vdata)
{
    Data data1 = vdata;

    try
    {
        scanEncryption(data1);
    }
    catch (SipEncryptionParserException exception)
    {
        if (SipParserMode::sipParserMode())
        {
            throw SipEncryptionParserException(
                "failed to decode the Encryption string",
                __FILE__,
                __LINE__, DECODE_ENCRYPTION_FAILED);
        }
    }
}

void
SipEncryption::scanEncryption(const Data & scandata)

{
    Data aedata = scandata;
    Data aevalue;
    int reth = aedata.match(" ", &aevalue, true);
    if (reth == FOUND)
    {
        setScheme(aevalue);
        Data encData = aedata;
        Data encValue;
        int ret = encData.match("=", &encValue, true);
        if (ret == FOUND)
        {
            setToken(encValue);
            setTokenValue(encData);
        }
        else if (ret == NOT_FOUND)
        {}

        else if (ret == FIRST)
        {}

    }
    else if (reth == NOT_FOUND)
    {}

    else if (reth == FIRST)
    {}

}


SipEncryption::SipEncryption( const SipEncryption& src )
        :
        scheme(src.scheme),
        token(src.token),
        tokenvalue(src.tokenvalue)
{}



bool
SipEncryption::operator ==(const SipEncryption& src) const
{
    cpDebug(LOG_DEBUG_STACK, "Encryption == Operator ");


    if ( (scheme == src.scheme) &&
            (token == src.token)
       )
    {
        cpDebug(LOG_DEBUG_STACK, "Encryption == Operator  returning True :)");
        return true;
    }
    else
    {
        cpDebug(LOG_DEBUG_STACK, "Encryption == Operator  returning False :(");
        return false;
    }
}


const SipEncryption&
SipEncryption::operator=( const SipEncryption& src )
{
    if (this != &src)
    {
        scheme = src.scheme;
        token = src.token;
        tokenvalue = src.tokenvalue;
    }
    return *this;
}


///
Data SipEncryption::getScheme()
{
    return scheme;
}

///
void SipEncryption::setScheme( const Data& newscheme )
{
    scheme = newscheme;
}

///
Data SipEncryption::getToken()
{
    return token;
}

///
void SipEncryption::setToken( const Data& newToken )
{
    token = newToken;
}

void SipEncryption::setTokenValue( const Data& newTokenvalue )
{
    tokenvalue = newTokenvalue;
}
Data SipEncryption::getTokenValue()
{
    return tokenvalue;
}
Data SipEncryption::encode() const
{
    Data data;
    if (scheme.length())
    {
        data += ENCRYPTION;
        data += SP;
        data += scheme;
        data += SP;
        data += token;
        data += "=";
        data += tokenvalue;
        data += CRLF;
    }
    return data;
}



SipHeader*
SipEncryption::duplicate() const
{
    return new SipEncryption(*this);
}


bool
SipEncryption::compareSipHeader(SipHeader* msg) const
{
    SipEncryption* otherMsg = dynamic_cast<SipEncryption*>(msg);
    if(otherMsg != 0)
    {
	return (*this == *otherMsg);
    }
    else
    {
	return false;
    }
}
