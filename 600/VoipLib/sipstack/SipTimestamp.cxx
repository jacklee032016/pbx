/*
 * $Id: SipTimestamp.cxx,v 1.2 2007/03/01 20:04:32 lizhijie Exp $
 */

#include "global.h"
#include "SipTimestamp.hxx"
#include "symbols.hxx"
#include "SipParserMode.hxx"
#include "cpLog.h"

using namespace Vocal;

string
SipTimestampParserException::getName( void ) const
{
    return "SipTimeststampParserException";
}
SipTimestamp::SipTimestamp()
{}


SipTimestamp::SipTimestamp( const Data& srcData )
{
    Data fdata = srcData;
    try
    {
        decode(fdata);
    }
    catch (SipTimestampParserException&)
    {

        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in Constructor of TimeStamp :( ");
            throw SipTimestampParserException(
                "failed to decode the Timestamp string :(",
                __FILE__,
                __LINE__, DECODE_TIMESTAMP_FAILED);
        }

    }

}
void SipTimestamp::decode(const Data& data)
{

    Data nData = data;

    try
    {

        scanSipTimestamp(nData);
    }
    catch (SipTimestampParserException&)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in TMIESTAMP decode():( ");
            throw SipTimestampParserException(
                "failed to decode the Timestamp string :(",
                __FILE__,
                __LINE__, DECODE_TIMESTAMP_FAILED);
        }
    }

}

void
SipTimestamp::scanSipTimestamp(const Data &tmpdata)
{
    setTime(tmpdata.convertInt());
}



SipTimestamp::SipTimestamp( const SipTimestamp& src )
{
    time = src.time;
    delay = src.delay;
}

bool
SipTimestamp::operator ==(const SipTimestamp& src) const
{
    cpDebug(LOG_DEBUG_STACK, "SipTimestamp, operator ==");
    if ( (time == src.time) &&
            (delay == src.delay)
       )
    {
        cpDebug(LOG_DEBUG_STACK, "SipTimestamp,operator == returning true");
        return true;
    }
    else
    {

        cpDebug(LOG_DEBUG_STACK, "SipTimestamp,operator == returning false");
        return false;
    }
}


int SipTimestamp::getTime()
{
    return time.convertInt();
}

void SipTimestamp::setTime( const int& newTime )
{
    time = Data(newTime);
}


int SipTimestamp::getDelay()
{
    return delay.convertInt();
}

void SipTimestamp::setDelay( const int& newDelay)
{
    delay = Data(newDelay);
}

Data SipTimestamp::getDelayData()
{
    return delay;
}

void SipTimestamp::setDelayData( const Data& newDelay )
{
    delay = newDelay;
}

Data SipTimestamp::encode() const
{
    Data data;
    if (time.length())
    {
        data += TIMESTAMP;
        data += SP;
        data += time;
        data += CRLF;
    }
    return data;
}


SipHeader*
SipTimestamp::duplicate() const
{
    return new SipTimestamp(*this);
}


bool
SipTimestamp::compareSipHeader(SipHeader* msg) const
{
    SipTimestamp* otherMsg = dynamic_cast<SipTimestamp*>(msg);
    if(otherMsg != 0)
    {
	return (*this == *otherMsg);
    }
    else
    {
	return false;
    }
}
