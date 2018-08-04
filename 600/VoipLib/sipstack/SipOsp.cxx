/*
 * $Id: SipOsp.cxx,v 1.2 2007/03/01 20:04:31 lizhijie Exp $
 */

#include "global.h"
#include "SipOsp.hxx"

using namespace Vocal;
    
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
