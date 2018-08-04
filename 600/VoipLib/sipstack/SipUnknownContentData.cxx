/*
 * $Id: SipUnknownContentData.cxx,v 1.2 2007/03/01 20:04:32 lizhijie Exp $
 */

#include "global.h"
#include "SipUnknownContentData.hxx"

using namespace Vocal;

SipUnknownContentData::SipUnknownContentData()
{
    setContentType(new SipContentType("application", "unknown"));
}


SipUnknownContentData::SipUnknownContentData(const Data& data)
    : myBody(data)
{
    setContentType(new SipContentType("application", "unknown"));
}


SipUnknownContentData::~SipUnknownContentData()
{
}


SipUnknownContentData::SipUnknownContentData( const SipUnknownContentData& rhs )
    :SipContentData(rhs),
     myBody(rhs.myBody)
{
}


Data 
SipUnknownContentData::encodeBody(int& len)
{
    len = myBody.length();
    return myBody;
}


SipUnknownContentData& 
SipUnknownContentData::operator=( const SipUnknownContentData& rhs )
{
    if(this != &rhs)
    {
	this->SipContentData::operator=(rhs);
	myBody = rhs.myBody;
    }
    return *this;
}

SipContentData* 
SipUnknownContentData::duplicate() const
{
    return new SipUnknownContentData(*this);
}
