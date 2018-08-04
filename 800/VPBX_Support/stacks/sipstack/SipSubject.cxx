/*
* $Log: SipSubject.cxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:10  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: SipSubject.cxx,v 1.1.1.1 2006/11/30 16:27:10 lizhijie Exp $
*/
#include "global.h"
#include "SipSubject.hxx"
#include "symbols.hxx"
#include "SipParserMode.hxx"
#include "cpLog.h"

using namespace Assist;


string
SipSubjectParserException::getName( void ) const
{
    return "SipSubjectParserException";
}

SipSubject::SipSubject()
{}


SipSubject::SipSubject( const SipSubject & src)
{
    data = src.data;
}

const SipSubject& SipSubject::operator=(const SipSubject& src)
{
    if ( &src != this)
    {
        data = src.data;
    }
    return (*this);
}
///
bool SipSubject::operator==(const SipSubject& src) const
{
    return ( data ==src.data );
}

SipSubject::SipSubject( const Data& srcData )
{
    Data wdata = srcData;
    try
    {
        decode(wdata);
    }
    catch (SipSubjectParserException&)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in Constructor of Subject :( ");
            throw SipSubjectParserException(
                "failed to decode the Subject string :(",
                __FILE__,
                __LINE__, DECODE_SUBJECT_FAILED);
        }


    }

}
void SipSubject::decode(const Data& indata)
{

    Data nData = indata;

    try
    {

        scanSipSubject(nData);
    }
    catch (SipSubjectParserException& exception)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in Constructor of Subject :( ");
            throw SipSubjectParserException(
                exception.getDescription(),
                __FILE__,
                __LINE__, DECODE_SUBJECT_FAILED
            );
        }
    }
}

void
SipSubject::scanSipSubject(const Data &tmpdata)
{
    set(tmpdata);
}



Data SipSubject::get() const
{
    return data;
}

void SipSubject::set( const Data& newdata)
{
    data = newdata;
}

Data SipSubject::encode() const
{
    Data ret;
    if (data.length())
    {
        ret = SUBJECT;
        ret += SP;
        ret += data;
        ret += CRLF;
    }
    return ret;
}



SipHeader*
SipSubject::duplicate() const
{
    return new SipSubject(*this);
}


bool
SipSubject::compareSipHeader(SipHeader* msg) const
{
    SipSubject* otherMsg = dynamic_cast<SipSubject*>(msg);
    if(otherMsg != 0)
    {
	return (*this == *otherMsg);
    }
    else
    {
	return false;
    }
}
