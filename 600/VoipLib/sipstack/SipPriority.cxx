/*
 * $Id: SipPriority.cxx,v 1.2 2007/03/01 20:04:31 lizhijie Exp $
 */

#include "global.h"
#include "SipPriority.hxx"
#include "symbols.hxx"
#include "cpLog.h"
#include "SipParserMode.hxx"

using namespace Vocal;


///
string
SipPriorityParserException::getName( void ) const
{
    return "SipPriorityParserException";
}


///
SipPriority::SipPriority()
{
}


///

SipPriority::SipPriority( const Data& srcData )
{
    try
    {
        decode(srcData);
    }
    catch (SipPriorityParserException&)
    {
        if (SipParserMode::sipParserMode())
        {
            throw SipPriorityParserException(
                "failed to decode the Priority string",
                __FILE__,
                __LINE__, DECODE_FAILED_PRIORITY);
        }
    }
}


///
SipPriority::SipPriority( const SipPriority& src )
{}



///

SipPriority&
SipPriority::operator = (const SipPriority& src)
{
    if (&src != this)
    {
        data = src.data;
    }
    return (*this);
}
///

bool
SipPriority::operator == (const SipPriority& src) const
{
  return ( data == src.data );
}


///
void
SipPriority::decode(const Data& vdata)
{

    Data data1 = vdata;

    try
    {
        scanPriority(data1);
    }
    catch (SipPriorityParserException exception)
    {
        if (SipParserMode::sipParserMode())
        {
            throw SipPriorityParserException(
                "failed to decode the Priority string",
                __FILE__,
                __LINE__, DECODE_FAILED_PRIORITY);
        }
    }
}



///
void
SipPriority::scanPriority(const Data & scandata)
{
    Data testdata = scandata;

    if ( (testdata == SIP_EMERGENCY) |
            (testdata == SIP_URGENT) |
            (testdata == SIP_NORMAL) |
            (testdata == SIP_NONURGENT))
    {
        set(testdata);
    }
    else
    {
        cpDebug(LOG_DEBUG_STACK, "Sip Priority Out of Range");
    }
}


///
Data SipPriority::get() const
{
    return data;
}

///
void SipPriority::set( const Data& newdata )
{
    data = newdata;

}


///
Data SipPriority::encode() const
{
    Data ret;
    if (data.length())
    {
        ret += SIP_PRIORITY;
        ret += SP;
        ret += data;
        ret += CRLF;
    }

    return ret;
}


SipHeader*
SipPriority::duplicate() const
{
    return new SipPriority(*this);
}


bool
SipPriority::compareSipHeader(SipHeader* msg) const
{
    SipPriority* otherMsg = dynamic_cast<SipPriority*>(msg);
    if(otherMsg != 0)
    {
	return (*this == *otherMsg);
    }
    else
    {
	return false;
    }
}
