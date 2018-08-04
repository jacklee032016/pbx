/*
 * $Id: SipDate.cxx,v 1.2 2007/03/01 20:04:31 lizhijie Exp $
 */

#include "global.h"
#include "symbols.hxx"
#include "cpLog.h"
#include "SipDate.hxx"
#include "SipParserMode.hxx"

static const char SIP_DATE[] = "Date: ";

using namespace Vocal;


string
SipDateParserException::getName( void ) const
{
    return "SipDateParserException";
}


SipDate::SipDate()
{
}


SipDate::SipDate( const Data& srcData )
  :flagDate(false)
{
    try
    {
        decode(srcData);
    }
    catch (SipDateParserException&)
    {

        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in Date Constructor :( ");
            throw SipDateParserException(
                "failed to decode the Date string",
                __FILE__,
                __LINE__, DECODE_DATE_FAILED);
        }

    }

}


void 
SipDate::decode(const Data& data)
{

    Data nData = data;

    try
    {

        scanSipDate(nData);
    }
    catch (SipDateParserException exception)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in Decode of Date :( ");
            throw SipDateParserException(
                "failed to decode the Date string",
                __FILE__,
                __LINE__, DECODE_DATE_FAILED);
        }
    }
}


void
SipDate::scanSipDate(const Data &tmpdata)
{
    Data newdata = tmpdata ;
    Data dateval;
    int ret = newdata.match(",", &dateval, true);
    if (ret == NOT_FOUND)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in scanSipDate of Date :( ");
            throw SipDateParserException(
                "failed to decode the Date string",
                __FILE__,
                __LINE__, DECODE_DATE_FAILED);
        }

    }
    else if (ret == FOUND)
    {
        setWeekday(dateval);
        parseDate(newdata);
    }
    else if (ret == FIRST)
    {

        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in scanSipDate of Date :( ");
            throw SipDateParserException(
                "failed to decode the Date string",
                __FILE__,
                __LINE__, DECODE_DATE_FAILED);
        }

    }
}


void
SipDate::parseDate(const Data & mdata)
{   flagDate = true;
    Data mvalue;
    Data mondata = mdata;
    int ret = mondata.match(" ", &mvalue, true);
    if (ret == FOUND)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in ParseDate of Date :( ");
            throw SipDateParserException(
                "failed to ParseDate() decode the Date string",
                __FILE__,
                __LINE__, DECODE_DATE_FAILED);
        }

    }
    else if (ret == NOT_FOUND)

    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in ParseDate of Date :( ");
            throw SipDateParserException(
                "failed to ParseDate() decode the Date string",
                __FILE__,
                __LINE__, DECODE_DATE_FAILED);
        }

    }
    else if (ret == FIRST)
    {
        Data monvalue;
        int retn = mondata.match(" ", &monvalue, true);
        if (retn == FOUND)
        {
            setDay(monvalue);
            Data mthvalue;
            Data tpdata = mondata;
            int retrn = tpdata.match(" ", &mthvalue, true);
            if (retrn == FOUND)
            {
                setMonth(mthvalue);
                Data tempdata = tpdata;
                Data tempValue;
                int rt = tempdata.match(" ", &tempValue, true);
                if (rt == FOUND)
                {
                    setYear(tempValue);
                    parseTime(tempdata);
                }
                else if (rt == NOT_FOUND)
                {
                    if (SipParserMode::sipParserMode())
                    {
                        cpLog(LOG_ERR, "Failed to Decode in ParseDate of Date :( ");
                        throw SipDateParserException(
                            "failed to ParseDate() decode the Date string",
                            __FILE__,
                            __LINE__, DECODE_DATE_FAILED);
                    }

                }
                else if (rt == FIRST)
                {

                    if (SipParserMode::sipParserMode())
                    {
                        cpLog(LOG_ERR, "Failed to Decode in ParseDate of Date :( ");
                        throw SipDateParserException(
                            "failed to ParseDate() decode the Date string",
                            __FILE__,
                            __LINE__, DECODE_DATE_FAILED);
                    }

                }
            }
            else if (retrn == NOT_FOUND)
            {
                if (SipParserMode::sipParserMode())
                {
                    cpLog(LOG_ERR, "Failed to Decode in ParseDate of Date :( ");
                    throw SipDateParserException(
                        "failed to ParseDate() decode the Date string",
                        __FILE__,
                        __LINE__, DECODE_DATE_FAILED);
                }
            }
            else if (retrn == FIRST)
            {
                if (SipParserMode::sipParserMode())
                {
                    cpLog(LOG_ERR, "Failed to Decode in ParseDate of Date :( ");
                    throw SipDateParserException(
                        "failed to ParseDate() decode the Date string",
                        __FILE__,
                        __LINE__, DECODE_DATE_FAILED);
                }
            }

        }
        else if (retn == NOT_FOUND)
        {
            if (SipParserMode::sipParserMode())
            {
                cpLog(LOG_ERR, "Failed to Decode in ParseDate of Date :( ");
                throw SipDateParserException(
                    "failed to ParseDate() decode the Date string",
                    __FILE__,
                    __LINE__, DECODE_DATE_FAILED);
            }

        }
        else if (retn == FIRST)
        {

            if (SipParserMode::sipParserMode())
            {
                cpLog(LOG_ERR, "Failed to Decode in ParseDate of Date :( ");
                throw SipDateParserException(
                    "failed to ParseDate() decode the Date string",
                    __FILE__,
                    __LINE__, DECODE_DATE_FAILED);
            }
        }
    }
}


void
SipDate::parseTime(const Data & newdata)
{
    Data timedata = newdata;
    Data timevalue;
    int ret = timedata.match(":", &timevalue, true);
    if (ret == FOUND)
    {
        setHour(timevalue);
        Data timeleftdata = timedata;
        Data timeleftvalue;
        int retrn = timeleftdata.match(":", &timeleftvalue, true);
        if (retrn == FOUND)
        {
            setMinutes(timeleftvalue);
            Data seconddata = timeleftdata;
            Data secondvalue;
            int rt = seconddata.match(" ", &secondvalue, true);
            if (rt == FOUND)
            {
                setSeconds(secondvalue);
                setTimezone(seconddata);
            }
            else if (rt == NOT_FOUND)
            {
                if (SipParserMode::sipParserMode())
                {
                    cpLog(LOG_ERR, "Failed to Decode in Method ParseTime() of Date :( ");
                    throw SipDateParserException(
                        "failed in Method ParseDate() decode the Date string",
                        __FILE__,
                        __LINE__, DECODE_DATE_FAILED);
                }


            }
            else if (rt == FIRST)
            {
                if (SipParserMode::sipParserMode())
                {
                    cpLog(LOG_ERR, "Failed to Decode in Method ParseTime() of Date :( ");
                    throw SipDateParserException(
                        "failed in Method ParseDate() decode the Date string",
                        __FILE__,
                        __LINE__, DECODE_DATE_FAILED);
                }
            }
        }
        else if (retrn == NOT_FOUND)
        {
            if (SipParserMode::sipParserMode())
            {
                cpLog(LOG_ERR, "Failed to Decode in Method ParseTime() of Date :( ");
                throw SipDateParserException(
                    "failed in Method ParseDate() decode the Date string",
                    __FILE__,
                    __LINE__, DECODE_DATE_FAILED);
            }

        }
        else if (retrn == FIRST)
        {
            if (SipParserMode::sipParserMode())
            {
                cpLog(LOG_ERR, "Failed to Decode in Method ParseTime() of Date :( ");
                throw SipDateParserException(
                    "failed in Method ParseDate() decode the Date string",
                    __FILE__,
                    __LINE__, DECODE_DATE_FAILED);
            }
        }

    }
    else if (ret == NOT_FOUND)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in Method ParseTime() of Date :( ");
            throw SipDateParserException(
                "failed in Method ParseDate() decode the Date string",
                __FILE__,
                __LINE__, DECODE_DATE_FAILED);
        }
    }
    else if (ret == FIRST)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in Method ParseTime() of Date :( ");
            throw SipDateParserException(
                "failed in Method ParseDate() decode the Date string",
                __FILE__,
                __LINE__, DECODE_DATE_FAILED);
        }

    }
}


void
SipDate::setMonth(const Data & bdata)
{
    Data monthdata = bdata;
    if (monthdata == MONTH1 || monthdata == MONTH2 || monthdata == MONTH3 ||
            monthdata == MONTH4 || monthdata == MONTH5 || monthdata == MONTH6 ||
            monthdata == MONTH7 || monthdata == MONTH8 || monthdata == MONTH9 ||
            monthdata == MONTH10 || monthdata == MONTH11 || monthdata == MONTH12 )
    {
        month = monthdata;
    }

    else
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in Method setMonth() of Date :( ");
            throw SipDateParserException(
                "failed in Method setMonth() decode the Date string",
                __FILE__,
                __LINE__, DECODE_MONTH_FAILED);
        }
    }
}


void
SipDate::setWeekday(const Data & newweekday)
{
    Data testdata = newweekday;
    if (testdata == WEEKDAY1 || testdata == WEEKDAY2 || testdata == WEEKDAY3 ||
            testdata == WEEKDAY4 || testdata == WEEKDAY5 || testdata == WEEKDAY6 ||
            testdata == WEEKDAY7)
    {
        weekday = newweekday;
    }

    else
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in WEEKDAY() of Date :( ");
            throw SipDateParserException(
                "failed in Method weekday() decode the Date string",
                __FILE__,
                __LINE__, DECODE_WEEKDAY_FAILED);
        }
    }
}


void
SipDate::setHour(const Data & newhour)
{
    string testhour = newhour.convertString();
    if (testhour.length() > 2)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in WEEKDAY() of Date :( ");
            throw SipDateParserException(
                "failed in Method Hour() decode the Date string",
                __FILE__,
                __LINE__, DECODE_HOUR_FAILED);
        }
    }
    string DIGIT = "0123456789";
    int test = testhour.find_first_not_of(DIGIT);
    if (test == -1)
    {
        hour = testhour;
    }

    else
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in WEEKDAY() of Date :( ");
            throw SipDateParserException(
                "failed in Method Hour() decode the Date string",
                __FILE__,
                __LINE__, DECODE_HOUR_FAILED);
        }
    }
}


void
SipDate::setSeconds(const Data & newseconds)
{
    string testseconds = newseconds.convertString();
    if (testseconds.length() > 2)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in seconds() of Date :( ");
            throw SipDateParserException("Failed in parsing seconds"
                                         , __FILE__, __LINE__, DECODE_SECONDS_FAILED);
        }
    }
    string DIGIT = "0123456789";
    int test = testseconds.find_first_not_of(DIGIT);
    if (test == -1)
    {
        seconds = testseconds;
    }
    else
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in seconds() of Date :( ");
            throw SipDateParserException("Failed in parsing seconds"
                                         , __FILE__, __LINE__, DECODE_SECONDS_FAILED);
        }


    }


}


void
SipDate::setMinutes(const Data & newminutes)
{
    string testminutes = newminutes.convertString();
    if (testminutes.length() > 2)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in setMinutes() of Date :( ");
            throw SipDateParserException("Failed in SetMinutes()"
                                         , __FILE__, __LINE__, DECODE_MINS_FAILED);
        }
    }
    string DIGIT = "0123456789";
    int test = testminutes.find_first_not_of(DIGIT);
    if (test == -1)
    {
        minutes = testminutes;
    }
    else
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in setMinutes() of Date :( ");
            throw SipDateParserException("Failed in setMinutes"
                                         , __FILE__, __LINE__, DECODE_MINS_FAILED);
        }


    }

}
void
SipDate::setTimezone(const Data & newtimezone)
{
    string temptimezone = newtimezone.convertString();
    Data testvalue;
    int test1 = temptimezone.find("+");
    int test2 = temptimezone.find("-");
    if (test1 == 0)
    {
        timezone = temptimezone;
    }
    else if (test2 == 0)
    {
        timezone = temptimezone;
    }

    else if (newtimezone.length() >= 1)
    {
        string ALPHA = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
        int testtime = temptimezone.find_first_not_of(ALPHA);
        if (testtime == -1)
        {
            timezone = temptimezone;
        }
        else
        {

            if (SipParserMode::sipParserMode())
            {
                cpLog(LOG_ERR, "Failed to Decode in setTimeZone of Date :( ");
                throw SipDateParserException("Failed in setTimeZones"
                                             , __FILE__, __LINE__, DECODE_TIMEZONE_FAILED);
            }
        }
    }

    else if (temptimezone == TIMEZONE1 || temptimezone == TIMEZONE2 || temptimezone == TIMEZONE3 ||
             temptimezone == TIMEZONE4 || temptimezone == TIMEZONE5 || temptimezone == TIMEZONE6 ||
             temptimezone == TIMEZONE7 || temptimezone == TIMEZONE8 || temptimezone == TIMEZONE9 ||
             temptimezone == TIMEZONE10 )
    {
        timezone = temptimezone;

    }
    else
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in setTimeZone of Date :( ");
            throw SipDateParserException("Failed in setTimeZones"
                                         , __FILE__, __LINE__, DECODE_TIMEZONE_FAILED);
        }


    }


}


void
SipDate::setDay(const Data & newData)
{
    if (newData.length() > 2)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in setDay of Date :( ");
            throw SipDateParserException("Failed in setDay"
                                         , __FILE__, __LINE__, DECODE_DAY_FAILED);
        }
    }

    else
    {
        string daytest = newData.convertString();
        string DIGIT = "0123456789";
        int testday = daytest.find_first_not_of(DIGIT);
        if (testday == -1)
        {

            day = newData;
        }
        else
        {
            if (SipParserMode::sipParserMode())
            {
                cpLog(LOG_ERR, "Failed to Decode in setDay of Date :( ");
                throw SipDateParserException("Failed in setDay"
                                             , __FILE__, __LINE__, DECODE_DAY_FAILED);
            }


        }
    }
}


void
SipDate::setYear(const Data & newData)
{
    if (newData.length() < 2 || newData.length() > 4)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in setYear of Date :( ");
            throw SipDateParserException("Failed in setYear"
                                         , __FILE__, __LINE__, DECODE_YEAR_FAILED);
        }
    }
    else
    {
        string yeartest = newData.convertString();
        string DIGIT = "0123456789";
        int testyear = yeartest.find_first_not_of(DIGIT);
        if (testyear == -1)
        {

            year = newData;
        }
        else
        {
            if (SipParserMode::sipParserMode())
            {
                cpLog(LOG_ERR, "Failed to Decode in setYear of Date :( ");
                throw SipDateParserException("Failed in setYear"
                                             , __FILE__, __LINE__, DECODE_YEAR_FAILED);
            }
        }

    }
}


SipDate::SipDate( const SipDate& src)
        :
        day(src.day),
        year(src.year),
        month(src.month),
        weekday(src.weekday),
        hour(src.hour),
        seconds(src.seconds),
        minutes(src.minutes),
        timezone(src.timezone),
        timezonediff(src.timezonediff)
{
}


Data SipDate::get()
{
    Data sipdate;
    sipdate = encode();

    return sipdate;
}


bool
SipDate::operator == (const SipDate& src) const
{
    cpDebug(LOG_DEBUG_STACK, "SipDate == operator");

    if ( ( day == src.day) &&
            ( year == src.year) &&
            ( month == src.month) &&
            ( weekday == src.weekday) &&
            ( hour == src.hour ) &&
            ( seconds == src.seconds ) &&
            ( minutes == src.minutes ) &&
            ( timezone == src.timezone ) &&
            ( timezonediff == src.timezonediff)
       )
    {
        cpDebug(LOG_DEBUG_STACK, "SipDate == operator returning true :)");

        return true;
    }
    else
    {
        cpDebug(LOG_DEBUG_STACK, "SipDate == operator returning false:(");

        return false;
    }
}

#define IsLess(a, b) if ((a) < (b)) return true; else if ((b) < (a)) return false
bool
SipDate::operator <(const SipDate& src) const
{
    // !dlb! this is certainly wrong -- deal with month names and timezones
    IsLess(year, src.year);
    IsLess(month, src.month);
    IsLess(day, src.day);
    IsLess(hour, src.hour);
    IsLess(minutes, src.minutes);
    IsLess(seconds, src.seconds);
    IsLess(timezone, src.timezone);
    IsLess(timezonediff, src.timezonediff);
    
    return false;
}
#undef IsLess

const SipDate&
SipDate::operator=( const SipDate& src)
{
    if (&src != this)
    {
        day = src.day;
        year = src.year;
        month = src.month;
        weekday = src.weekday;
        hour = src.hour;
        seconds = src.seconds;
        minutes = src.minutes;
        timezone = src.timezone;
        timezonediff = src.timezonediff;
    }
    return *this;
}


Data 
SipDate::encode() const
{

    Data data;
    int weeklen = weekday.length();
    int daylen = day.length();
    int monlen = month.length();
    int yearlen = year.length();
    int hourlen = hour.length();
    int minlen = minutes.length();
    int timezonelen = timezone.length();

    if ( (weeklen) &&
            (daylen) &&
            (monlen) &&
            (yearlen) &&
            (hourlen) &&
            (minlen) &&
            (timezonelen) )
    {
        data += SIP_DATE;
        data += SP;
        data += weekday;
        data += ",";
        data += SP;
        data += day;
        data += SP;
        data += month;
        data += SP;
        data += year;
        data += SP;
        data += hour;
        data += ":";
        data += minutes;
        data += ":";
        data += seconds;
        data += SP;
        data += timezone;
        data += CRLF;
    }

    return data;
}


bool
SipDate::getFlagDate() const
{
  return flagDate;
}


Data 
SipDate::getData() const
{
    Data data;
    int weeklen = weekday.length();
    int daylen = day.length();
    int monlen = month.length();
    int yearlen = year.length();
    int hourlen = hour.length();
    int minlen = minutes.length();
    int timezonelen = timezone.length();

    if ( (weeklen) &&
            (daylen) &&
            (monlen) &&
            (yearlen) &&
            (hourlen) &&
            (minlen) &&
            (timezonelen) )
    {
        data += weekday;
        data += ",";
        data += SP;
        data += day;
        data += SP;
        data += month;
        data += SP;
        data += year;
        data += SP;
        data += hour;
        data += ":";
        data += minutes;
        data += ":";
        data += seconds;
        data += SP;
        data += timezone;
    }

    return data;
}


void 
SipDate::set(const Data& data)
{
    try
    {
        decode(data);
    }
    catch ( SipDateParserException&)
    {
        cpLog(LOG_ERR, "could not decode the date correctly");
    }
}


SipHeader*
SipDate::duplicate() const
{
    return new SipDate(*this);
}


bool
SipDate::compareSipHeader(SipHeader* msg) const
{
    SipDate* otherMsg = dynamic_cast<SipDate*>(msg);
    if(otherMsg != 0)
    {
	return (*this == *otherMsg);
    }
    else
    {
	return false;
    }
}
