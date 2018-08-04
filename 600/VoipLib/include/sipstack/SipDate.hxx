#ifndef SipDate_HXX
#define SipDate_HXX
/*
 * $Id: SipDate.hxx,v 1.2 2007/03/01 20:04:28 lizhijie Exp $
 */

#include "Data.hxx"
#include "SipHeader.hxx"
#include "VException.hxx"


namespace Vocal
{


enum SipDateErrorType
{
    DECODE_DATE_FAILED,
    DECODE_WEEKDAY_FAILED,
    DECODE_HOUR_FAILED,
    DECODE_MONTH_FAILED,
    DECODE_SECONDS_FAILED,
    DECODE_TIMEZONE_FAILED,
    DECODE_DAY_FAILED,
    DECODE_YEAR_FAILED,
    DECODE_MINS_FAILED
    //may need to change this to be more specific
};

/// Exception handling class SipDateParserException
class SipDateParserException : public VException
{
    public:
        SipDateParserException( const string& msg,
                                const string& file,
                                const int line,
                                const int error = 0 );
        SipDateParserException( const string& msg,
                                const string& file,
                                const int line,
                                SipDateErrorType i)
        : VException( msg, file, line, static_cast < int > (i))
        {
            value = i;
        }
        SipDateErrorType getError() const
        {
            return value;
        }
        string getName() const ;
    private:
        SipDateErrorType value;
};



/// data container for Date header
class SipDate : public SipHeader
{
    public:
        /// Create one with default values
        SipDate();

        // TODO - add more constructors that take the parameters needed to build it

        // TODO - add accessor functions to get at the sub components

        /*** Create by decoding the data string passed in. This is the decode
        or parse. This call should only be used inside the stack and is
        not part of the API */
        SipDate( const Data& srcData );
        SipDate( const SipDate& src );

        ///
        bool operator ==(const SipDate& src) const;

        bool operator <(const SipDate& src) const;

        ///
        const SipDate& operator=( const SipDate& src );
        ///
        Data get();
        ///
        void scanSipDate(const Data& data);
        ///
        void parseDate(const Data& data);
        ///
        void parseTime(const Data& data);
        ///
        void setMonth(const Data& newData);
        ///
        void setYear(const Data& newData);
        ///
        void setDay(const Data& newData);
        ///
        void setSeconds(const Data& newseconds);
        ///
        void setHour(const Data& newhour);
        ///
        void setMinutes(const Data& newminutes);
        ///
        void setTimezone(const Data& newtimezone);
        ///
        void setWeekday(const Data& newweekday);
        ///
        Data getDay() const
        {
            return day;
        }
        ///
        Data getMonth() const
        {
            return month;
        }
        ///
        Data getYear() const
        {
            return year;
        }
        ///
        Data getSeconds() const
        {
            return seconds;
        }
        ///
        Data getHour() const
        {
            return hour;
        }
        ///
        Data getMinutes() const
        {
            return minutes;
        }
        ///
        Data getTimezone() const
        {
            return timezone;
        }
        ///
        Data getWeekday() const
        {
            return timezonediff;
        }

        /*** return the encoded string version of this. This call
	     should only be used inside the stack and is not part of
	     the API */
        Data encode() const;

        ///
        //Data getDay() const { return day;}

        ///
        Data getData() const;

        ///
        bool getFlagDate() const;
        ///
        //Data getDay() const { return day;}
        /// takes the entire date string
        void set(const Data& data);   

	/** method for copying sip headers of any type without knowing
            which type */
	SipHeader* duplicate() const;
	/// compare two headers of (possibly) the same class
	virtual bool compareSipHeader(SipHeader* msg) const;
    private:
        bool flagDate;
        Data day;
        Data year;
        Data month;
        Data weekday;
        Data hour;
        Data seconds;
        Data minutes;
        Data timezone;
        Data timezonediff;
        void decode(const Data& data);
    
        friend class SipMsg;
};
 
} // namespace Vocal

#endif
