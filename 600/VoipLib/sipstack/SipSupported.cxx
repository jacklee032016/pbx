/*
 * $Id: SipSupported.cxx,v 1.2 2007/03/01 20:04:32 lizhijie Exp $
 */

#include "global.h"
#include "SipSupported.hxx"
#include "symbols.hxx"
#include "SipParserMode.hxx"
#include "cpLog.h"

using namespace Vocal;

string
SipSupportedParserException::getName( void ) const
{
    return "SipSupportedParserException";
}

SipSupported::SipSupported()
{}


SipSupported::SipSupported( const SipSupported & src)
{
    data = src.data;
}

SipSupported& SipSupported::operator=(const SipSupported& src)
{
    if ( &src != this)
    {
        data = src.data;
    }
    return (*this);
}

SipSupported::SipSupported( const Data& srcData )
{
    Data wdata = srcData;
    try
    {
        decode(wdata);
    }
    catch (SipSupportedParserException&)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in Constructor of Supported :( ");
            throw SipSupportedParserException(
                "failed to decode the Supported string :(",
                __FILE__,
                __LINE__, DECODE_SUPPORTED_FAILED);
        }


    }

}
void SipSupported::decode(const Data& idata)
{

    Data nData = idata;

    try
    {

        scanSipSupported(nData);
    }
    catch (SipSupportedParserException exception)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in Constructor of Supported :( ");
            throw SipSupportedParserException(
                exception.getDescription(),
                __FILE__,
                __LINE__, DECODE_SUPPORTED_FAILED
            );
        }
    }
}

void
SipSupported::scanSipSupported(const Data &tmpdata)
{
    set(tmpdata);
}



Data SipSupported::get()
{
    return data;
}

void SipSupported::set( const Data& newdata)
{
    data = newdata;
}

Data SipSupported::encode() const
{
    Data ret;
    if (data.length())
    {
        ret += data;
    }
    return ret;
}


bool SipSupported::operator ==(const SipSupported& src) const
{
    if (data == src.data)
    {
        return true;
    }  
    else
    {
        return false;
    }
}



SipHeader*
SipSupported::duplicate() const
{
    return new SipSupported(*this);
}


bool
SipSupported::compareSipHeader(SipHeader* msg) const
{
    SipSupported* otherMsg = dynamic_cast<SipSupported*>(msg);
    if(otherMsg != 0)
    {
	return (*this == *otherMsg);
    }
    else
    {
	return false;
    }
}
