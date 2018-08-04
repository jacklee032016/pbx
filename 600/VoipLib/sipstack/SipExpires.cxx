/*
 * $Id: SipExpires.cxx,v 1.2 2007/03/01 20:04:31 lizhijie Exp $
 */

#include "global.h"
#include "SipExpires.hxx"
#include "symbols.hxx"
#include "SipParserMode.hxx"
#include "cpLog.h"

using namespace Vocal;


string
SipExpiresParserException::getName( void ) const
{
    return "SipExpiresParserException";
}
SipExpires::SipExpires()
    : delta("")
{}


SipExpires::SipExpires( const Data& srcData )
    : delta("")
{
    Data fdata = srcData;
    try
    {
        decode(fdata);
    }
    catch (SipExpiresParserException&)
    {

        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in EXPIRE Constructor :( ");
            throw SipExpiresParserException(
                "failed to decode the EXPIRE string",
                __FILE__,
                __LINE__, DECODE_EXPIRE_FAILED);
        }

    }

}
void SipExpires::decode(const Data& data)
{

    Data nData = data;

    try
    {

        scanSipExpires(nData);
    }
    catch (SipExpiresParserException exception)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in EXPIRE Constructor :( ");
            throw SipExpiresParserException(
                "failed to decode the EXPIRE string",
                __FILE__,
                __LINE__, DECODE_EXPIRE_FAILED);
        }

    }

}

void
SipExpires::scanSipExpires(const Data &tmpdata)

{
    for(int i=0; i < tmpdata.length(); i++)
    {
        if(!isdigit(tmpdata.getChar(i)))
        {
            setDelta("3600");
            return;
        }        
    }
    setDelta(tmpdata);
}




SipExpires::SipExpires( const SipExpires& src )
    : delta(src.delta)
{}


bool
SipExpires::operator ==(const SipExpires& src) const
{
    return delta == src.delta;
}

bool
SipExpires::operator <( const SipExpires& src ) const
{
    return delta < src.delta;
}

const SipExpires&
SipExpires::operator=( const SipExpires& src )
{
    if ( &src != this )
    {
        delta = src.delta;
    }
    return *this;
}

void SipExpires::setDelta ( const Data& data)
{
    delta = data;
    cpDebug(LOG_DEBUG_STACK, "set delta in expires");
}

Data SipExpires::getDelta() const
{
    return delta;
}

///
Data SipExpires::encode() const
{
    Data data;    
    if (delta != "")
    {
        data = SIP_EXPIRES;
        data += SP;
        data += getData();
	data += CRLF;
    }
    return data;
}

Data SipExpires::getData() const
{
    return delta;
}



SipHeader*
SipExpires::duplicate() const
{
    return new SipExpires(*this);
}


bool
SipExpires::compareSipHeader(SipHeader* msg) const
{
    SipExpires* otherMsg = dynamic_cast<SipExpires*>(msg);
    if(otherMsg != 0)
    {
	return (*this == *otherMsg);
    }
    else
    {
	return false;
    }
}
