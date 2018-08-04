/*
 * $Id: SipContentEncoding.cxx,v 1.2 2007/03/01 20:04:31 lizhijie Exp $
 */

#include "SipContentEncoding.hxx"
#include "SipParserMode.hxx"
#include "cpLog.h"

static const char SIP_CONTENCODING[] = "Content-Encoding ";

using namespace Vocal;


string
SipContentEncodingParserException::getName( void ) const
{
    return "SipContentEncodingParserException";
}

SipContentEncoding::SipContentEncoding(const SipContentEncoding& src)
        : data(src.data)
{}


SipContentEncoding::SipContentEncoding(void)
{}


SipContentEncoding::SipContentEncoding( const Data& newData )
{
    try
    {
        decode(newData);
    }
    catch (SipContentEncodingParserException&)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in Constructor :o( ");
            throw SipContentEncodingParserException("failed in Decode", __FILE__, __LINE__, DECODE_CONTENTENCODING_FAILED);

        }
    }
}


const SipContentEncoding&
SipContentEncoding::operator=(const SipContentEncoding& src)
{
    if (this != &src)
    {
        data = src.data;
    }
    return *this;
}


void
SipContentEncoding::decode(const Data& indata)
{
    Data nData = indata;
    try
    {
        scanSipContentEncoding(nData);
    }
    catch (SipContentEncodingParserException exception)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in Constructor :o( ");
            throw SipContentEncodingParserException("failed in Decode", __FILE__, __LINE__, DECODE_CONTENTENCODING_FAILED);
        }
    }
}

void
SipContentEncoding::scanSipContentEncoding(const Data &tmpdata)
{
    string newdata = tmpdata.convertString() ;

    string TOKEN = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ-.!%*_+'~";
    int test = newdata.find_first_not_of(TOKEN);
    if (test == -1)
    {
        set(newdata);
    }
    else
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to parse Token :o( ");
            throw SipContentEncodingParserException("failed in Parse'n Token ", __FILE__, __LINE__, DECODE_TOKEN_FAILED);

        }
    }
}


Data
SipContentEncoding::get()
{
    return data;
}


void
SipContentEncoding::set( const Data& newData )
{
    data = newData;
}


Data SipContentEncoding::encode() const
{
    Data ret;
    // if (data.length())
    ret += SIP_CONTENCODING ;
    ret += ":";
    ret += data ;

    return ret;
}


bool
SipContentEncoding::operator ==( const SipContentEncoding& src) const
{
    if ( data == src.data )
    {
        return true;
    }
    else
    {
        return false;
    }
}


SipHeader*
SipContentEncoding::duplicate() const
{
    return new SipContentEncoding(*this);
}


bool
SipContentEncoding::compareSipHeader(SipHeader* msg) const
{
    SipContentEncoding* otherMsg = dynamic_cast<SipContentEncoding*>(msg);
    if(otherMsg != 0)
    {
	return (*this == *otherMsg);
    }
    else
    {
	return false;
    }
}
