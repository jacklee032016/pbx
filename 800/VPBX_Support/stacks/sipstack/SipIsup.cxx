/*
* $Log: SipIsup.cxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:09  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: SipIsup.cxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#include "global.h"
#include "SipIsup.hxx"

using namespace Assist;

SipIsup::SipIsup()
{
}
    
    
SipIsup::SipIsup(const Data& data)
{
    isupData = data;
    setContentType(new SipContentType("application", "isup"));
}

    
SipIsup::SipIsup( const SipIsup& src )
{
    isupData = src.isupData;
    setContentType(new SipContentType("application", "isup"));
}


const SipIsup& SipIsup::operator=( const SipIsup& src )
{
    if ( &src != this )
    {
        SipContentData::operator=( src );

        isupData = src.isupData;
	
    }
    
    return *this;
}
 


SipIsup::~SipIsup()
{}


void SipIsup::setData( const Data& data )
{
    isupData = data;
   
}

Data SipIsup::getData()
{
    return isupData;
}
 

  

Data SipIsup::encodeBody(int& len)
{
    len = isupData.length();
    
    if (getContentLength() != 0)
    {
	getContentLength()->setLength(len);
    }
  
    return isupData;
}


SipContentData* 
SipIsup::duplicate() const
{
    return new SipIsup(*this);
}
