/*
* $Log: SipPriority.cxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:09  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: SipPriority.cxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/
#include "global.h"
#include "SipPriority.hxx"
#include "symbols.hxx"
#include "cpLog.h"
#include "SipParserMode.hxx"

using namespace Assist;


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

