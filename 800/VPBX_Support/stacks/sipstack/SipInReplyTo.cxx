/*
* $Log: SipInReplyTo.cxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:09  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: SipInReplyTo.cxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#include "global.h"
#include "symbols.hxx"
#include "SipInReplyTo.hxx"
#include "SipParserMode.hxx"
#include "cpLog.h"

using namespace Assist;

string
SipInReplyToParserException::getName( void ) const
{
    return "SipInReplyToParserException";
}

///
SipInReplyTo::SipInReplyTo()
        : inreplyto()
{
}

SipInReplyTo&
SipInReplyTo::operator=(const SipInReplyTo& src)
{
    if ( &src != this )
    {
        inreplyto = src.inreplyto;
        calliddata = src.calliddata;
    }
    return (*this);
}

///
/// 
bool SipInReplyTo::operator ==(const SipInReplyTo & src) const
{
  return (
	  ( inreplyto == src.inreplyto ) &&
	  ( calliddata == src.calliddata ) 
	  );
}
///
SipInReplyTo::SipInReplyTo( const Data& srcData )
        : inreplyto()
{
    Data ndata = srcData;
    try
    {
        decode(ndata);
    }
    catch (SipInReplyToParserException&)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in Constructor : ( ");
            throw SipInReplyToParserException("failed in Decode", __FILE__, __LINE__, DECODE_INREPLYTO_FAILED);

        }

    }

}
void SipInReplyTo::decode(const Data& data)
{

    Data nData = data;

    try
    {

        scanSipInReplyTo(nData);
    }
    catch (SipInReplyToParserException exception)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in Constructor in Sip InReply TO  : ( ");
            throw SipInReplyToParserException(
                exception.getDescription(),
                __FILE__,
                __LINE__, DECODE_INREPLYTO_FAILED
            );
        }
    }

}

void
SipInReplyTo::scanSipInReplyTo(const Data &tmpdata)
{
    SipCallId newid(tmpdata);
    inreplyto = newid;
}


///
SipInReplyTo::SipInReplyTo( const SipInReplyTo& src )
        : inreplyto(src.inreplyto),
        calliddata(src.calliddata)
{}

///

Data SipInReplyTo::get() const
{
    calliddata = inreplyto.getCallIdData();
    return calliddata;
}

///
void SipInReplyTo::set( const Data& newdata )
{
    calliddata = newdata;
    inreplyto.setData(calliddata);
}

///
Data SipInReplyTo::encode() const
{
    Data data;
    Data ldata = get(); 
    if (ldata.length())
    {
        data += "In-Reply-To:";
        data += SP;
        data += ldata;
        data += CRLF;
    }

    return data;
}



SipHeader*
SipInReplyTo::duplicate() const
{
    return new SipInReplyTo(*this);
}


bool
SipInReplyTo::compareSipHeader(SipHeader* msg) const
{
    SipInReplyTo* otherMsg = dynamic_cast<SipInReplyTo*>(msg);
    if(otherMsg != 0)
    {
	return (*this == *otherMsg);
    }
    else
    {
	return false;
    }
}
