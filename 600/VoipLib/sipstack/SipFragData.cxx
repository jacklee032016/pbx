/*
 * $Id: SipFragData.cxx,v 1.2 2007/03/01 20:04:31 lizhijie Exp $
 */

#include "global.h"
#include "SipFragData.hxx"
#include "cpLog.h"

using namespace Vocal;

SipFragData::SipFragData(const Data& fragData)
    : SipContentData(), myFragData(fragData)
{
    Sptr <SipContentType> contType = getContentType();
    if (contType == 0)
    {
        contType = new SipContentType();
    }
    contType->setType("application");
    contType->setSubType("sipfrag");
    setContentType(contType);
}


SipFragData::~SipFragData()
{
}

SipFragData::SipFragData(const SipFragData& src)
        : SipContentData(src),
          myFragData(src.myFragData)
{
 
    Sptr <SipContentType> contType = getContentType();
    if (contType == 0)
    {
        contType = new SipContentType();
    }
    contType->setType("application");
    contType->setSubType("sipfrag");
    setContentType(contType);
}


SipFragData&
SipFragData::operator =(const SipFragData& src)
{
   
    if (&src != this )
    {
	SipContentData::operator=(src);
        myFragData = src.myFragData;
    }
    return (*this);
}


Data
SipFragData::encodeBody(int& len)
{
    len = myFragData.length();
    return myFragData;
}


SipContentData* 
SipFragData::duplicate() const
{
    return new SipFragData(*this);
}
