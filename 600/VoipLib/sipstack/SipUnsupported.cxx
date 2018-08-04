/*
 * $Id: SipUnsupported.cxx,v 1.2 2007/03/01 20:04:32 lizhijie Exp $
 */

#include "global.h"
#include "SipUnsupported.hxx"
#include "SipParserMode.hxx"
#include "symbols.hxx"

using namespace Vocal;


string
SipUnsupportedParserException::getName( void ) const
{
    return "SipUnsupportedParserException";
}
///
SipUnsupported::SipUnsupported()
{}

///

SipUnsupported::SipUnsupported( const Data& srcData )
{
    try
    {
        decode(srcData);
    }
    catch (SipUnsupportedParserException&)
    {
        if (SipParserMode::sipParserMode())
        {
            throw SipUnsupportedParserException(
                "failed to decode the UnSupported string",
                __FILE__,
                __LINE__, DECODE_UNSUPPORTED_FAILED);
        }
    }
}
void
SipUnsupported ::decode(const Data& reqdata)
{
    set(reqdata);
}
///
SipUnsupported::SipUnsupported( const SipUnsupported& src )
{
    data = src.data;
}

///
SipUnsupported&
SipUnsupported::operator=( const SipUnsupported& src )
{
    if ( &src != this )
    {
        data = src.data;
    }
    return *this;
}

///
bool SipUnsupported::operator ==( const SipUnsupported& src ) const
{
 return  ( data == src.data);   
}
///
Data SipUnsupported::get()
{
    return data;
}

///
void SipUnsupported::set( const Data& newdata )
{
    data = newdata;
}

///
Data SipUnsupported::encode() const
{
    Data ret;
    if (data.length())
    {
        ret += SIP_UNSUPPORTED;

        ret += data;
        ret += CRLF;
    }

    return ret;
}


SipHeader*
SipUnsupported::duplicate() const
{
    return new SipUnsupported(*this);
}


bool
SipUnsupported::compareSipHeader(SipHeader* msg) const
{
    SipUnsupported* otherMsg = dynamic_cast<SipUnsupported*>(msg);
    if(otherMsg != 0)
    {
	return (*this == *otherMsg);
    }
    else
    {
	return false;
    }
}
