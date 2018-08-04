/*
* $Log: SipAllow.cxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:09  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: SipAllow.cxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/
#include "global.h"
#include "symbols.hxx"
#include "cpLog.h"
#include "SipAllow.hxx"
#include "SipParserMode.hxx"


using namespace Assist;

string
SipAllowParserException::getName( void ) const
{
    return "SipAllowParserException";
}

///
SipAllow::SipAllow()
{}

///

SipAllow::SipAllow( const Data& srcData)
{

    Data ndata = srcData;
    try
    {
        decode(ndata);
    }
    catch (SipAllowParserException&)
    {

        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in Constructor :o( ");
            throw SipAllowParserException("failed in Decode of Allow ", __FILE__, __LINE__, DECODE_ALLOW_FAILED);

        }

    }

}

bool SipAllow::operator == (const SipAllow& src) const
{
    return ( data == src.data);
   
}


void SipAllow::decode(const Data& vdata)
{
    try
    {

        scanSipAllow(vdata);
    }
    catch (SipAllowParserException exception)
    {

        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in Allow Constructor :o( ");
            throw SipAllowParserException("failed in ALLOW Decode", __FILE__, __LINE__, DECODE_ALLOW_FAILED);

        }
    }
}

void
SipAllow::scanSipAllow(const Data &newdata)
{
    setAllowMethod(newdata);
}


///
SipAllow::SipAllow( const SipAllow& src)
{
    data = src.data;
}

///
const SipAllow&
SipAllow::operator = (const SipAllow& src)
{
    if (&src != this)
    {
        data = src.data;
    }
    return (*this);
}

///
Data SipAllow::getAllowMethod() const
{
    return data;
}

///
void SipAllow::setAllowMethod( const Data& newData )
{
    data = newData;
}

///void
Data SipAllow::encode() const
{
    Data allow;

    allow += data;
    return allow;

}



SipHeader*
SipAllow::duplicate() const
{
    return new SipAllow(*this);
}


bool
SipAllow::compareSipHeader(SipHeader* msg) const
{
    SipAllow* otherMsg = dynamic_cast<SipAllow*>(msg);
    if(otherMsg != 0)
    {
	return (*this == *otherMsg);
    }
    else
    {
	return false;
    }
}

