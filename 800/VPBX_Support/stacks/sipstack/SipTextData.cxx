/*
* $Log: SipTextData.cxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:10  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: SipTextData.cxx,v 1.1.1.1 2006/11/30 16:27:10 lizhijie Exp $
*/

#include "global.h"
#include "SipTextData.hxx"
#include "cpLog.h"

using namespace Assist;

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
