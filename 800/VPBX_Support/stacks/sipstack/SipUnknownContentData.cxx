/*
* $Log: SipUnknownContentData.cxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:10  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: SipUnknownContentData.cxx,v 1.1.1.1 2006/11/30 16:27:10 lizhijie Exp $
*/

#include "global.h"
#include "SipUnknownContentData.hxx"

using namespace Assist;

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
