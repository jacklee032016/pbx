/*
 * $Id: SipTextData.cxx,v 1.2 2007/03/01 20:04:32 lizhijie Exp $
 */

#include "global.h"
#include "SipTextData.hxx"
#include "cpLog.h"

using namespace Vocal;

SipTextData::SipTextData(const Data& textData, const Data& type)
    : SipContentData(), myTextData(textData),_type(type)
{
    setContentType(new SipContentType("application", _type));
}


SipTextData::~SipTextData()
{
}

SipTextData::SipTextData(const SipTextData& src)
        : SipContentData(src),
          myTextData(src.myTextData),
          _type(src._type)
{
    setContentType(new SipContentType("application", _type));
}


SipTextData&
SipTextData::operator =(const SipTextData& src)
{
   
    if (&src != this )
    {
	SipContentData::operator=(src);
        myTextData = src.myTextData;
    }
    return (*this);
}


Data
SipTextData::encodeBody(int& len)
{
    len = myTextData.length();
    return myTextData;
}


SipContentData* 
SipTextData::duplicate() const
{
    return new SipTextData(*this);
}
