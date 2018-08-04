/*
 * $Id: SipProxyRequire.cxx,v 1.2 2007/03/01 20:04:31 lizhijie Exp $
 */

#include "global.h"
#include "symbols.hxx"
#include "SipProxyRequire.hxx"
#include "SipParserMode.hxx"

using namespace Vocal;

static const char SIP_PROXYREQUIRE[] = "Proxy-Require";
string
SipProxyRequireParserException::getName( void ) const
{
    return "SipProxyRequireParserException";
}
///
SipProxyRequire::SipProxyRequire()
{
}

///

SipProxyRequire::SipProxyRequire( const Data& srcData )
{
    try
    {
        decode(srcData);
    }
    catch (SipProxyRequireParserException&)
    {
        if (SipParserMode::sipParserMode())
        {
            throw SipProxyRequireParserException(
                "failed to decode the Proxy Require string",
                __FILE__,
                __LINE__, DECODE_PROXYREQUIRE_FAILED);
        }
    }
}


///
SipProxyRequire::SipProxyRequire( const SipProxyRequire& src )
        : data(src.data)
{}

///

SipProxyRequire&
SipProxyRequire::operator=( const SipProxyRequire& src)
{
    if (&src != this)
    {
        data = src.data;
    }
    return *this;
}


bool
SipProxyRequire::operator==( const SipProxyRequire& src) const
{
  return
        (data == src.data ) ;
}

void SipProxyRequire::decode(const Data& reqdata)
{
    set(reqdata);
}
///
Data SipProxyRequire::get() const
{
    Data ret(data);
    return ret;
}

///
void SipProxyRequire::set( const Data& newdata )
{
    data = newdata;
}

///
Data SipProxyRequire::encode() const
{
    Data ret;
    ret += data;
    return ret;
}



SipHeader*
SipProxyRequire::duplicate() const
{
    return new SipProxyRequire(*this);
}


bool
SipProxyRequire::compareSipHeader(SipHeader* msg) const
{
    SipProxyRequire* otherMsg = dynamic_cast<SipProxyRequire*>(msg);
    if(otherMsg != 0)
    {
	return (*this == *otherMsg);
    }
    else
    {
	return false;
    }
}
