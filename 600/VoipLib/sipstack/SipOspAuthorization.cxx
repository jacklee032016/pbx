/*
 * $Id: SipOspAuthorization.cxx,v 1.2 2007/03/01 20:04:31 lizhijie Exp $
 */

#include "global.h"
#include "SipOspAuthorization.hxx"
#include "SipParserMode.hxx"
#include "symbols.hxx"

using namespace Vocal;

string
SipOspAuthorizationParserException::getName( void ) const
{
    return "SipOspAuthorizationParserException";
}
///
SipOspAuthorization::SipOspAuthorization()
{}

///

SipOspAuthorization::SipOspAuthorization( const Data& srcData )
{
    try
    {
        decode(srcData);
    }
    catch (SipOspAuthorizationParserException&)
    {
        if (SipParserMode::sipParserMode())
        {
            throw SipOspAuthorizationParserException(
                "failed to decode the OSP string",
                __FILE__,
                __LINE__, DECODE_SIPOSPPARSER_FAILED);
        }
    }
}
void
SipOspAuthorization ::decode(const Data& reqdata)
{
    set(reqdata);
}
///
SipOspAuthorization::SipOspAuthorization( const SipOspAuthorization& src )
{
    data = src.data;
}

///
SipOspAuthorization&
SipOspAuthorization::operator=( const SipOspAuthorization& src )
{
    if ( &src != this )
    {
        data = src.data;
    }
    return *this;
}

///
bool
SipOspAuthorization::operator==( const SipOspAuthorization& src ) const
{
    return ( data == src.data );
}
///
Data SipOspAuthorization::get()
{
    return data;
}

///
void SipOspAuthorization::set( const Data& newdata )
{
    data = newdata;
}

///
Data SipOspAuthorization::encode() const
{
    Data ret;
    if (data.length())
    {
        ret += OSP_AUTH;
        ret += SP;
        ret += data;
        ret += CRLF;
    }

    return ret;
}


SipHeader*
SipOspAuthorization::duplicate() const
{
    return new SipOspAuthorization(*this);
}


bool
SipOspAuthorization::compareSipHeader(SipHeader* msg) const
{
    SipOspAuthorization* otherMsg = dynamic_cast<SipOspAuthorization*>(msg);
    if(otherMsg != 0)
    {
	return (*this == *otherMsg);
    }
    else
    {
	return false;
    }
}
