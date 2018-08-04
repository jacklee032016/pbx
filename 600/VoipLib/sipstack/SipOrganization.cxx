/*
 * $Id: SipOrganization.cxx,v 1.2 2007/03/01 20:04:31 lizhijie Exp $
 */

#include "global.h"
#include "symbols.hxx"
#include "SipOrganization.hxx"
#include "SipParserMode.hxx"
#include "cpLog.h"

using namespace Vocal;

string
SipOrganizationParserException::getName( void ) const
{
    return "SipOrganizationParserException";
}

///
SipOrganization::SipOrganization()
{
}

///

SipOrganization::SipOrganization( const Data& srcData )
{
    Data ndata = srcData;
    try
    {
        decode(ndata);
    }
    catch (SipOrganizationParserException&)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in Constructor : ( ");
            throw SipOrganizationParserException("failed in Decode",
                                                 __FILE__,
                                                 __LINE__,
                                                 DECODE_ORGANIZATION_FAILED);
        }
    }
}



///
SipOrganization& SipOrganization::operator = (const SipOrganization& src)
{
    if (&src != this)
    {
        organization = src.organization;
    }
    return (*this);
}
///
bool 
SipOrganization::operator == (const SipOrganization& src) const
{
  return ( organization == src.organization );
}


void SipOrganization::decode(const Data& data)
{

    Data nData = data;

    try
    {
        scanSipOrganization(nData);
    }
    catch (SipOrganizationParserException exception)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in Constructor : ( ");

            throw SipOrganizationParserException(
                exception.getDescription(),
                __FILE__,
                __LINE__, DECODE_ORGANIZATION_FAILED );
        }
    }
}

void
SipOrganization::scanSipOrganization(const Data &tmpdata)
{
    string newdata = tmpdata.convertString() ;

    set(newdata);
}


///
SipOrganization::SipOrganization( const SipOrganization& src )
        : organization(src.organization)
{}

///

Data SipOrganization::get() const
{
    return organization;
}

///
void SipOrganization::set( const Data& newdata )
{
    organization = newdata;
}

///
Data SipOrganization::encode() const
{
    Data data;
    if (organization.length())
    {
        data = ORGANIZATION;
        data += SP;
        data += organization;
        data += CRLF;
    }

    return data;
}



SipHeader*
SipOrganization::duplicate() const
{
    return new SipOrganization(*this);
}


bool
SipOrganization::compareSipHeader(SipHeader* msg) const
{
    SipOrganization* otherMsg = dynamic_cast<SipOrganization*>(msg);
    if(otherMsg != 0)
    {
	return (*this == *otherMsg);
    }
    else
    {
	return false;
    }
}
