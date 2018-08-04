/*
* $Log: SipMaxForwards.cxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:09  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: SipMaxForwards.cxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#include "global.h"
#include "symbols.hxx"
#include "SipMaxForwards.hxx"
#include "SipParserMode.hxx"

using namespace Assist;


string
SipMaxForwardsParserException::getName( void ) const
{
    return "SipMaxForwardsParserException";
}

SipMaxForwards::SipMaxForwards(): value(70)
{}


SipMaxForwards::SipMaxForwards( const Data& data ) //TODO
{
    try
    {
        decode(data);
    }
    catch (SipMaxForwardsParserException& exception)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode  SipMaxForwards  : ( ");
            throw SipMaxForwardsParserException(
                exception.getDescription(),
                __FILE__,
                __LINE__, DECODE_MAXFORWARDS_FAILED
            );
        }

    }

}

const SipMaxForwards&
SipMaxForwards::operator=(const SipMaxForwards& src)
{
    if (&src != this)
    {
        value = src.value;
    }
    return (*this);
}

bool
SipMaxForwards::operator==(const SipMaxForwards& src) const
{
  return ( value == src.value );
}
void SipMaxForwards::decode(const Data& data)
{

    Data nData = data;

    try
    {

        scanSipMaxForwards(nData);
    }
    catch (SipMaxForwardsParserException exception)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in decode() of SipMaxForwards  : ( ");
            throw SipMaxForwardsParserException(
                exception.getDescription(),
                __FILE__,
                __LINE__, DECODE_MAXFORWARDS_FAILED
            );
        }
    }
}

void
SipMaxForwards::scanSipMaxForwards(const Data &tmpdata)
{
    string newdata = tmpdata.convertString() ;

    string DIGIT = "0123456789";
    int test = newdata.find_first_not_of(DIGIT);
    if (test == -1)
    {
        set(newdata);
    }
    else
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed in parsing token  : ( ");
            throw SipMaxForwardsParserException(
                "Failed in parsing token",
                __FILE__,
                __LINE__, DECODE_MAXFORWARDS_FAILED
            );
        }


    }
}


SipMaxForwards::SipMaxForwards( const SipMaxForwards& src )
        : value(src.value)
{}

///

int
SipMaxForwards::get() const
{
    return value;
}

///
void
SipMaxForwards::set( const Data& newValue )
{
    if (newValue.length())
    {
        value = newValue.convertInt();
    }
}

void
SipMaxForwards::set( int newValue )
{
    if (newValue >= 0)
    {
        value = newValue;
    }
}

int
SipMaxForwards::decrement()
{
    if (value > 0)
    {
        return --value;
    }
    else
    {
        return -1;
    }
}


Data SipMaxForwards::encode() const
{

    Data data;
    if (value > 0)
    {
        data = MAX_FORWARDS;
        data += SP;
        data += Data(value);
        data += CRLF;
    }

    return data;
}


SipHeader*
SipMaxForwards::duplicate() const
{
    return new SipMaxForwards(*this);
}


bool
SipMaxForwards::compareSipHeader(SipHeader* msg) const
{
    SipMaxForwards* otherMsg = dynamic_cast<SipMaxForwards*>(msg);
    if(otherMsg != 0)
    {
	return (*this == *otherMsg);
    }
    else
    {
	return false;
    }
}

ostream&
operator<<(ostream& s, const SipMaxForwards &smf)
{
    s << smf.encode();
    return s;
}
