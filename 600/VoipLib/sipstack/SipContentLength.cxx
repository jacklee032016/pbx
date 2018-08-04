/*
 * $Id: SipContentLength.cxx,v 1.2 2007/03/01 20:04:31 lizhijie Exp $
 */

#include "global.h"
#include "SipContentLength.hxx"
#include "SipMsg.hxx"
#include "symbols.hxx"

using namespace Vocal;

string
SipContentLengthParserException::getName( void ) const
{
    return "SipUrlParserException";
}


SipContentLength::SipContentLength(const SipContentLength& src)
        : contentLength(src.contentLength)
{}



SipContentLength::SipContentLength()
{}



SipContentLength::SipContentLength( const Data& data ) //TODO
{

    try
    {
        decode(data);
    }
    catch (SipContentLengthParserException&)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in Content Length Constructor :o( ");
            throw SipContentLengthParserException("failed in Decode", __FILE__, __LINE__, DECODE_CONTENTLENGTH_FAILED);

        }
    }
}


void
SipContentLength::parse( const Data& cseqdata)
{
    Data data = cseqdata;
    setLengthData(cseqdata);
}



void
SipContentLength::decode( const Data& clstr )
{
    try
    {
        parse(clstr);
    }
    catch (SipContentLengthParserException exception)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in Content Length  :o( ");
            throw SipContentLengthParserException("failed in Decode", __FILE__, __LINE__, DECODE_CONTENTLENGTH_FAILED);

        }
    }
}


void SipContentLength::setLength(int length)
{
    contentLength = Data(length);
}


Data SipContentLength::encode() const
{
    Data encoding;

    encoding = CONTENT_LENGTH;
    encoding += SP;
    if ( contentLength.length() != 0)
      {
	encoding += contentLength;
      }
    else
      {
	encoding += Data("0");
      }

    encoding += CRLF;
    

    return encoding;
}


void SipContentLength::setLengthData(const Data& contentlngth)
{
    contentLength = contentlngth;
}


Data SipContentLength::getLength() const
{
    return contentLength;
}


bool SipContentLength::operator ==( const SipContentLength& src) const
{
    if (contentLength == src.contentLength)
    {
        return true;
    }
    else
    {
        return false;
    }
}


const SipContentLength&
SipContentLength::operator=(const SipContentLength& src)
{
    if (&src != this)
    {
        contentLength = src.contentLength;
    }
    return *this;
}


SipHeader*
SipContentLength::duplicate() const
{
    return new SipContentLength(*this);
}


bool
SipContentLength::compareSipHeader(SipHeader* msg) const
{
    SipContentLength* otherMsg = dynamic_cast<SipContentLength*>(msg);
    if(otherMsg != 0)
    {
	return (*this == *otherMsg);
    }
    else
    {
	return false;
    }
}
