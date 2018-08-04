/*
* $Log: SipOspAuthorization.cxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:09  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: SipOspAuthorization.cxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#include "global.h"
#include "SipOspAuthorization.hxx"
#include "SipParserMode.hxx"
#include "symbols.hxx"

using namespace Assist;

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
