/*
 * $Id: SipContentData.cxx,v 1.2 2007/03/01 20:04:31 lizhijie Exp $
 */


#include "global.h"
#include "SipContentData.hxx"

using namespace Vocal;

SipContentData::SipContentData()
        : contentType(),
	  contentLength(),
	  contentDisposition()
{
}


SipContentData::~SipContentData()
{
}


SipContentData::SipContentData( const SipContentData& rhs )
{
    contentType = rhs.contentType;
    contentLength = rhs.contentLength;
    contentDisposition = rhs.contentDisposition;
    
}


SipContentData&
SipContentData::operator=( const SipContentData& rhs )
{
    if ( &rhs != this )
    {
        contentType = rhs.contentType;
	contentLength = rhs.contentLength;
	contentDisposition = rhs.contentDisposition;
	
    }
    return *this;
}


Data
SipContentData::encodeHeaders() const
{
    Data data;
    
    if (contentType != 0)
    {
	data += contentType->encode();
    }
    if (contentLength != 0)
    {
	data += contentLength->encode();
    }
    if (contentDisposition != 0)
    {
	data += contentDisposition->encode();
    }
    return data;
}
 

Sptr <SipContentType> SipContentData::getContentType()
{
    return contentType;
}


void SipContentData::setContentType(Sptr <SipContentType>  contType)
{
    contentType = contType;
}
    

Sptr <SipContentLength> SipContentData::getContentLength()
{
    return contentLength;
}

    
void SipContentData::setContentLength(Sptr <SipContentLength> contLen)
{
    contentLength = contLen;
}

    
Sptr <SipContentDisposition> SipContentData::getContentDisposition()
{
    return contentDisposition;
}

    
void SipContentData::setContentDisposition(Sptr <SipContentDisposition> contDisp)
{
    contentDisposition = contDisp;
}
