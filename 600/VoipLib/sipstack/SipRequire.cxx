/*
 * $Id: SipRequire.cxx,v 1.2 2007/03/01 20:04:31 lizhijie Exp $
 */

#include "global.h"
#include "SipRequire.hxx"
#include "SipParserMode.hxx"

using namespace Vocal;

string
SipRequireParserException::getName( void ) const
{
    return "SipRequireParserException :o(";
}


///
SipRequire::SipRequire()
{
}

///

SipRequire::SipRequire( const Data& srcData )
{
    Data wdata = srcData;
    try
    {
        decode(wdata);
    }
    catch (SipRequireParserException&)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in Constructor : ( ");
            throw SipRequireParserException("failed in Decode", __FILE__, __LINE__, DECODE_REQUIRE_FAILED);

        }

    }

}

///
SipRequire&
SipRequire::operator=( const SipRequire& src )
{
    if (&src != this)
    {
        data = src.data;
    }
    return *this;
}

///
bool
SipRequire::operator==( const SipRequire& src ) const
{
    return ( data == src.data );
}

void
SipRequire::decode(const Data& ndata)
{

    Data nData = ndata;

    try
    {

        scanSipRequire(nData);
    }
    catch (SipRequireParserException exception)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in Constructor : ( ");
            throw SipRequireParserException(
                exception.getDescription(),
                __FILE__,
                __LINE__, DECODE_REQUIRE_FAILED
            );
        }
    }

}

void
SipRequire::scanSipRequire(const Data &tmpdata)
{
    set(tmpdata);
}

///
SipRequire::SipRequire( const SipRequire& src )
        : data(src.data)
{}

///

Data SipRequire::get()
{
    return data;
}

///
void SipRequire::set( const Data& newdata )
{
    data = newdata;
}

///
Data SipRequire::encode() const
{
    Data ret;
    ret += data;
    return ret;



}



SipHeader*
SipRequire::duplicate() const
{
    return new SipRequire(*this);
}


bool
SipRequire::compareSipHeader(SipHeader* msg) const
{
    SipRequire* otherMsg = dynamic_cast<SipRequire*>(msg);
    if(otherMsg != 0)
    {
	return (*this == *otherMsg);
    }
    else
    {
	return false;
    }
}
