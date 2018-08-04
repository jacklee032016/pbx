/*
* $Log: SipResponseKey.cxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:10  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: SipResponseKey.cxx,v 1.1.1.1 2006/11/30 16:27:10 lizhijie Exp $
*/

#include "global.h"
#include "SipResponseKey.hxx"
#include "symbols.hxx"
#include "SipParserMode.hxx"

using namespace Assist;


string
SipResponseKeyParserException::getName( void ) const
{
    return "SipResponseKeyParserException";
}
///
SipResponseKey::SipResponseKey()
{
}

SipResponseKey::SipResponseKey(const Data& srcData)
{
    try
    {
        decode(srcData);
    }
    catch (SipResponseKeyParserException&)
    {
        if (SipParserMode::sipParserMode())
        {
            throw SipResponseKeyParserException("failed to decode the Response Key string",
                                                __FILE__,
                                                __LINE__, DECODE_RESPONSEKEY_FAILED);
        }
    }
}
SipResponseKey::SipResponseKey(const SipResponseKey& src)
{
    key = src.key;
    param = src.param;
}


///
SipResponseKey&
SipResponseKey::operator=( const SipResponseKey& src)
{
    if (&src != this)
    {
        key = src.key;
        param = src.param;
    }
    return *this;
}

///
bool
SipResponseKey::operator==( const SipResponseKey& src) const
{
  return (
        ( key == src.key ) &&
        ( param == src.param )
    );
}
///
Data SipResponseKey::getKey()
{
    return key;
}

///
void SipResponseKey::setKey( const Data& newkey )
{
    key = newkey;
}

void SipResponseKey::decode(const Data& reqdata)
{
    Data keydata = reqdata;
    Data keyscheme;
    int ret = keydata.match("SP", &keyscheme, true);
    if (ret == FIRST)
    {
        if (SipParserMode::sipParserMode())
        {
            throw SipResponseKeyParserException(
                "failed to decode the Response Key SPace is not found between Key scheme & key param string",
                __FILE__,
                __LINE__, DECODE_RESPONSEKEY_FAILED);
        }
    }

    else if (ret == FOUND)
    {
        setKey(keyscheme);
        parseKeyParm(keydata);
    }
    else if (ret == NOT_FOUND)
    {
        if (SipParserMode::sipParserMode())
        {
            throw SipResponseKeyParserException(
                "failed to decode the Response Key SPace is not found between Key scheme & key param string",
                __FILE__,
                __LINE__, DECODE_RESPONSEKEY_FAILED);
        }
    }
}
void
SipResponseKey::parseKeyParm(const Data& keyval)
{
    setParam(keyval);
}


///
Data SipResponseKey::getParam()
{
    return param;
}

///
void SipResponseKey::setParam( const Data& newparam )
{
    param = newparam;
}

///
Data SipResponseKey::encode() const
{
    Data ret;
    if ((key.length()) && (param.length()))
    {
        ret += SIP_RESPONSEKEY;
        ret += SP;
        ret += key;
        ret += "=";
        ret += param;
        ret += CRLF;
    }
    return ret;
}



SipHeader*
SipResponseKey::duplicate() const
{
    return new SipResponseKey(*this);
}


bool
SipResponseKey::compareSipHeader(SipHeader* msg) const
{
    SipResponseKey* otherMsg = dynamic_cast<SipResponseKey*>(msg);
    if(otherMsg != 0)
    {
	return (*this == *otherMsg);
    }
    else
    {
	return false;
    }
}
