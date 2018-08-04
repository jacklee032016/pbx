/*
 * $Id: SipContentFunction.cxx,v 1.2 2007/03/01 20:04:31 lizhijie Exp $
 */

#include "SipContentFunction.hxx"
#include "SipParserMode.hxx"
#include "cpLog.h"

static const char SIP_CONTFUNCTION[] = "Content-Encoding ";




string
SipContentFunctionParserException::getName( void ) const
{
    return "SipContentFunctionParserException";
}

SipContentFunction::SipContentFunction(SipContentFunction & src)
        : data(src.data)
{}


SipContentFunction::SipContentFunction(void)
{
}


SipContentFunction& SipContentFunction::operator=( const SipContentFunction& src)
{
    if ( &src != this)
    {
        data = src.data;
    }
    return (*this);
}



SipContentFunction::SipContentFunction( const Data& newData )
{
    try
    {
        decode(newData);
    }
    catch (SipContentFunctionParserException& expection)
    {

        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in Constructor :o( ");
            throw SipContentFunctionParserException("failed in Decode", __FILE__, __LINE__, DECODE_CONTENTFUNCTION_FAILED);

        }

    }

}
void SipContentFunction::decode(const Data& data)
{

    Data nData = data;

    try
    {

        scanSipContentFunction(nData);
    }
    catch (SipContentFunctionParserException exception)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in Constructor :o( ");
            throw SipContentFunctionParserException("failed in Decode", __FILE__, __LINE__, DECODE_CONTENTFUNCTION_FAILED);

        }

    }

}

void
SipContentFunction::scanSipContentFunction(const Data &tmpdata)
{
    string newdata = tmpdata ;

    set(newdata);
}


Data SipContentFunction::get()
{
    return data;
}

void SipContentFunction::set( const Data& newData )
{
    data = newData;

}

Data SipContentFunction::encode()
{
    Data ret;
    // if (data.length())
    ret += SIP_CONTFUNCTION ;
    ret += ":";
    ret += data ;

    return ret;
}
