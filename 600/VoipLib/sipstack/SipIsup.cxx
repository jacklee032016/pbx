/*
 * $Id: SipIsup.cxx,v 1.2 2007/03/01 20:04:31 lizhijie Exp $
 */

#include "global.h"
#include "SipIsup.hxx"

using namespace Vocal;

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
