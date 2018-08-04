/*
 * $Id: SipRetryAfter.cxx,v 1.2 2007/03/01 20:04:31 lizhijie Exp $
 */

#include "global.h"
#include "SipRetryAfter.hxx"
#include "symbols.hxx"
#include "SipParserMode.hxx"
#include "cpLog.h"

using namespace Vocal;

string
SipRetryAfterParserException::getName( void ) const
{
    return "SipRetryAfterParserException";
}
///
SipRetryAfter::SipRetryAfter()
        :
        flagcomment(false),
        flagduration(false)
{
}

///

SipRetryAfter::SipRetryAfter( const Data& srcData )
        :
        flagcomment(false),
        flagduration(false)
{
    try
    {
        decode(srcData);
    }
    catch (SipRetryAfterParserException&)
    {
        if (SipParserMode::sipParserMode())
        {
            throw SipRetryAfterParserException(
                "failed to decode the Retry After string",
                __FILE__,
                __LINE__, DECODE_RETRYAFTER_FAILED);
        }
    }

}


SipRetryAfter&
SipRetryAfter::operator=(const SipRetryAfter& rhs)
{
    if ( &rhs != this )
    {
        date = rhs.date;
        datedata = rhs.datedata;
        comment = rhs.comment;
        flagcomment = rhs.flagcomment;
        flagduration = rhs.flagduration;
    }
    return *this;
}


void SipRetryAfter::decode(const Data& retrydata)
{

    Data data = retrydata;

    try
    {

        scanRetryAfter(data);
    }
    catch (SipRetryAfterParserException& exception)
    {
        if (SipParserMode::sipParserMode())
        {
            throw SipRetryAfterParserException(
                exception.getDescription(),
                __FILE__,
                __LINE__, SCAN_RETRYAFTER_FAILED);

        }
    }

}
void
SipRetryAfter::scanRetryAfter(const Data &tmpdata)
{

    Data rtdata = tmpdata;
    Data newvalue;
    int retn = rtdata.match("(", &newvalue, true);
    if (retn == NOT_FOUND)
    {
        Data rtvalue;
        int ret = rtdata.match(";", &rtvalue, true);
        if (ret == FOUND)
        {
            parseDate(rtvalue);
            parseDuration(rtdata);
        }
        else if (ret == NOT_FOUND)
        {
            parseDate(rtdata);

        }
        else if (ret == FIRST)
        {
            if (SipParserMode::sipParserMode())
            {
                throw SipRetryAfterParserException(
                    "failed to decode the Retry After string in scanRetryAfter :(",
                    __FILE__,
                    __LINE__, DECODE_RETRYAFTER_FAILED);
            }
        }

    }
    else if (retn == FOUND)
    {

        parseDate(newvalue);
        Data finald = rtdata;
        Data finalv;
        int rt = finald.match(")", &finalv, true);
        if (rt == FOUND)
        {
            setComment(finalv);
            Data valuei;
            int reth = finald.match(";", &valuei, true);
            if (reth == FOUND)
            {
                if (SipParserMode::sipParserMode())
                {
                    throw SipRetryAfterParserException(
                        "failed to decode the Retry After string in scanRetryAfter :(",
                        __FILE__,
                        __LINE__, DECODE_RETRYAFTER_FAILED);
                }
            }
            else if (reth == NOT_FOUND)
            {
            }

            else if (reth == FIRST)
            {
                parseDuration(finald);
            }
        }
        else if (rt == NOT_FOUND)
        {
            if (SipParserMode::sipParserMode())
            {
                throw SipRetryAfterParserException(
                    "failed to decode the Retry After string in  scanRetryAfter :(",
                    __FILE__,
                    __LINE__, DECODE_RETRYAFTER_FAILED);
            }
        }
        else if (rt == FIRST)
        {
            if (SipParserMode::sipParserMode())
            {
                throw SipRetryAfterParserException(
                    "failed to decode the Retry After string in  scanRetryAfter :(",
                    __FILE__,
                    __LINE__, DECODE_RETRYAFTER_FAILED);
            }

        }

    }
    else if (retn == FIRST)
    {

        if (SipParserMode::sipParserMode())
        {
            throw SipRetryAfterParserException(
                "failed to decode the Retry After string in scanRetryAfter :(",
                __FILE__,
                __LINE__, DECODE_RETRYAFTER_FAILED);
        }
    }
}

void
SipRetryAfter::parseDuration(const Data& datdata)
{
    Data durdata = datdata;
    Data durvalue;
    int retn = durdata.match("=", &durvalue, true);
    if (retn == FOUND)
    {
        if (durdata == SIP_DURATION)
        {
            setDuration(durdata.convertInt());
        }
    }
    else if (retn == NOT_FOUND)
    {
        if (SipParserMode::sipParserMode())
        {
            throw SipRetryAfterParserException(
                "failed to decode the Retry After string  in ParseDuration :(",
                __FILE__,
                __LINE__, DECODE_RETRYAFTER_FAILED);
        }
    }
    else if (retn == FIRST)
    {
        if (SipParserMode::sipParserMode())
        {
            throw SipRetryAfterParserException(
                "failed to decode the Retry After string in Parse Duration:(",
                __FILE__,
                __LINE__, DECODE_RETRYAFTER_FAILED);
        }
    }
}
void
SipRetryAfter::parseDate(const Data& dadata)
{
    Data dtdata = dadata;
    Data dtvalue;
    int retn = dtdata.match(",", &dtvalue, false);
    if (retn == FOUND)
    {
        SipDate datetmp(dtdata);
        date = datetmp;
    }
    else if (retn == NOT_FOUND)
    {
        setDuration(dtdata.convertInt());
    }
    else if (retn == FIRST)
    {
        if (SipParserMode::sipParserMode())
        {
            throw SipRetryAfterParserException(
                "failed to decode the Retry After string in Parse Date :(",
                __FILE__,
                __LINE__, DECODE_RETRYAFTER_FAILED);
        }
    }

}

///
SipRetryAfter::SipRetryAfter( const SipRetryAfter& src )
        :
        date(src.date),
        datedata(src.datedata),
        comment(src.comment),
        duration(src.duration),
        flagcomment(src.flagcomment),
        flagduration(src.flagduration)
{}


bool
SipRetryAfter::operator ==(const SipRetryAfter& src) const
{
    cpDebug(LOG_DEBUG_STACK, "Retry After == Operator ");

    if ( (date == src.date) &&
            (comment == src.comment) &&
            (duration == src.duration )
       )
    {
        cpDebug(LOG_DEBUG_STACK, "Retry After == Operator  returning True");
        return true;
    }
    else
    {
        cpDebug(LOG_DEBUG_STACK, "Retry After == Operator  returning False");
        return false;
    }

}


///
SipDate SipRetryAfter::getDate() const
{
    return date;
}

///
void SipRetryAfter::setDate( const SipDate& newdate )
{
    date = newdate;

}

///
Data SipRetryAfter::getComment() const
{
    return comment;
}

///
void SipRetryAfter::setComment( const Data& newcomment )
{
    comment = newcomment;
    flagcomment = true;
}

///
int SipRetryAfter::getDuration() const
{
    return duration.convertInt();

}

///
void
SipRetryAfter::setDuration( int newduration )
{
    duration = Data(newduration);
    flagduration = true ;
}


///
Data SipRetryAfter::encode() const
{
    Data ret;
    Data ldatedata = date.encode();

    if (ldatedata.length())
    {

        ret += SIP_RETRYAFTER;
        ret += SP;
        ret += ldatedata;
        if (flagcomment)
        {
            ret += comment;
        }
        if (flagduration)
        {
            ret += ";";
            ret += "duration";
            ret += "=";
            ret += duration;
        }
    }
    return ret;
}



// End of File


SipHeader*
SipRetryAfter::duplicate() const
{
    return new SipRetryAfter(*this);
}


bool
SipRetryAfter::compareSipHeader(SipHeader* msg) const
{
    SipRetryAfter* otherMsg = dynamic_cast<SipRetryAfter*>(msg);
    if(otherMsg != 0)
    {
	return (*this == *otherMsg);
    }
    else
    {
	return false;
    }
}
