/*
* $Log: SipFragData.cxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:09  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: SipFragData.cxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#include "global.h"
#include "SipFragData.hxx"
#include "cpLog.h"

using namespace Assist;

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
