/*
* $Log: SipContentData.cxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:09  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: SipContentData.cxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#include "global.h"
#include "SipContentData.hxx"

using namespace Assist;

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
