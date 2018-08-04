/*
* $Log: SipHide.cxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:09  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: SipHide.cxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#include "global.h"
#include "SipHide.hxx"
#include "symbols.hxx"
#include "SipParserMode.hxx"

using namespace Assist;


string
SipHideParserException::getName( void ) const
{
    return "SipHideParserException";
}
///
SipHide::SipHide()
{}

///

SipHide::SipHide( const Data& srcData )
{
    try
    {
        decode(srcData);
    }
    catch (SipHideParserException&)
    {


        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in Constructor : ( ");
            throw SipHideParserException("failed in Decode", __FILE__, __LINE__, DECODE_HIDE_FAILED);

        }

    }
}

const SipHide&
SipHide::operator=(const SipHide& src)
{
    if (&src != this)
    {
        data = src.data;
    }
    return (*this);
}

bool
SipHide::operator==(const SipHide& src) const
{
    return     ( data == src.data );
}

void SipHide::decode(const Data& vdata)
{

    Data data1 = vdata;

    try
    {

        scanHide(data1);
    }
    catch (SipHideParserException exception)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in decode() of Hide : ( ");
            throw SipHideParserException(
                exception.getDescription(),
                __FILE__,
                __LINE__, DECODE_HIDE_FAILED
            );
        }
    }

}
void
SipHide::scanHide(const Data & scandata)

{
    Data aedata = scandata;
    Data aevalue;
    if (aedata == HIDE_ROUTE)
    {
        set(aedata);
    }
    if (aedata == HIDE_HOP)
    {
        set(aedata);
    }
    else
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Value out of Range in HIDE it Should be Route|Hop : ( ");
            throw SipHideParserException("failed in Decode", __FILE__, __LINE__, DECODE_HIDE_FAILED);

        }
    }
}

///
SipHide::SipHide( const SipHide& src )
        : data(src.data)
{}


///

Data SipHide::get() const
{
    return data;
}

///
void SipHide::set( const Data& newData)
{
    data = newData;

}

///
Data SipHide::encode() const
{
    Data ret;
    if (data.length())
    {
        ret = HIDE;
        ret += SP;
        ret += data;
        ret += CRLF;
    }
    return ret ;
}


SipHeader*
SipHide::duplicate() const
{
    return new SipHide(*this);
}


bool
SipHide::compareSipHeader(SipHeader* msg) const
{
    SipHide* otherMsg = dynamic_cast<SipHide*>(msg);
    if(otherMsg != 0)
    {
	return (*this == *otherMsg);
    }
    else
    {
	return false;
    }
}
