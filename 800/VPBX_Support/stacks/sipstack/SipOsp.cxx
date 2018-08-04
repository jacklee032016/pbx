/*
* $Log: SipOsp.cxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:09  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: SipOsp.cxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#include "global.h"
#include "SipOsp.hxx"

using namespace Assist;
    
SipOsp::SipOsp()
{
    setContentType(new SipContentType("application", "osp"));
}


SipOsp::SipOsp( const SipOsp& src )
{
    ospData = src.ospData;
    setContentType(new SipContentType("application", "osp"));
}

    
SipOsp::SipOsp( const Data& data)
{
    ospData = data;
    setContentType(new SipContentType("application", "osp"));
}
    
    
const SipOsp& SipOsp::operator=( const SipOsp& src )
{
    if ( &src != this )
    {
        SipContentData::operator=( src );
        ospData = src.ospData;
    }

    return *this;
}


SipOsp::~SipOsp()
{
}


void SipOsp::setData( const Data& data )
{
    ospData = data;
}

    
Data SipOsp::getData()
{
    return ospData;
}

    
Data SipOsp::encodeBody(int& len) 
{
    len = ospData.length();
    if (getContentLength() != 0)
    {
	getContentLength()->setLength(len);
    }
   
    return ospData;
}


SipContentData* SipOsp::duplicate() const
{
    return new SipOsp(*this);
}
