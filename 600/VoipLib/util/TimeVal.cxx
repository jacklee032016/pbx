/*
 * $Id: TimeVal.cxx,v 1.2 2007/03/01 20:04:32 lizhijie Exp $
 */


#include "global.h"
#include "TimeVal.hxx"
#include "VTime.hxx"
#include <cassert>


using Vocal::TimeAndDate::TimeVal;
using Vocal::TimeAndDate::milliseconds_t;


TimeVal::TimeVal()
{
    clear();
}


TimeVal::TimeVal(milliseconds_t src)
{
    clear();
    operator+=(src);
}

TimeVal::TimeVal(long sec, long usec)
{
    clear();
    tv_sec = sec;
    tv_usec = usec;
}
        


TimeVal::TimeVal(const timeval & src)
{
    copy(src);
    normalize();
}


TimeVal::~TimeVal()
{
}


TimeVal &
TimeVal::operator=(const timeval & src)
{
    if ( this != & src )
    {
        copy(src);
        normalize();
    }
    return ( *this );
}


const TimeVal &
TimeVal::now()
{
    int rc = gettimeofday(this, 0);
    assert( rc == 0 );

    return ( *this );
}


milliseconds_t
TimeVal::milliseconds() const
{
    return ( (tv_sec * 1000) + (tv_usec / 1000) );
}


TimeVal
TimeVal::operator+(const timeval & right) const
{
    TimeVal left(*this);
    return ( left += right );
}


TimeVal
TimeVal::operator+(milliseconds_t right) const
{
    TimeVal left(*this);
    return ( left += right );
}


TimeVal &
TimeVal::operator+=(const timeval & src)
{
    tv_sec += src.tv_sec;
    tv_usec += src.tv_usec;

    normalize();

    return ( *this );
}


TimeVal &
TimeVal::operator+=(milliseconds_t src)
{
    tv_sec += src / 1000;
    tv_usec += (src % 1000) // Between [0..1000) milliseconds.
               * 1000;      // Convert to microseconds.

    normalize();

    return ( *this );
}


TimeVal
TimeVal::operator-(const timeval & right) const
{
    TimeVal left(*this);
    return ( left -= right );
}


TimeVal
TimeVal::operator-(milliseconds_t right) const
{
    TimeVal left(*this);
    return ( left -= right );
}


TimeVal &
TimeVal::operator-=(const timeval & src)
{
    tv_sec -= src.tv_sec;
    tv_usec -= src.tv_usec;

    normalize();

    return ( *this );
}


TimeVal &
TimeVal::operator-=(milliseconds_t src)
{
    tv_sec -= src / 1000;
    tv_usec += (src % 1000) // Between [0..1000) milliseconds.
               * 1000;      // Convert to microseconds.

    normalize();

    return ( *this );
}


bool
TimeVal::operator==(const timeval & src) const
{
    // Create a normalize value for the timeval to simplify comparison.
    //
    TimeVal right(src);

    return ( tv_sec == right.tv_sec
             && tv_usec == right.tv_usec
           );
}


bool
TimeVal::operator<(const timeval & src) const
{
    // Create a normalize value for the timeval to simplify comparison.
    //
    TimeVal right(src);

    if ( tv_sec < right.tv_sec )
    {
        return ( true );
    }
    else if ( (tv_sec == right.tv_sec) && (tv_usec < right.tv_usec) )
    {
        return ( true );
    }
    else
    {
        return ( false );
    }
}

bool
TimeVal::operator<(const TimeVal & right) const
{
    if ( tv_sec < right.tv_sec )
    {
        return ( true );
    }
    else if ( (tv_sec == right.tv_sec) && (tv_usec < right.tv_usec) )
    {
        return ( true );
    }
    else
    {
        return ( false );
    }
}


void
TimeVal::clear()
{
    tv_sec = tv_usec = 0;
}


void
TimeVal::copy(const timeval & src)
{
    tv_sec = src.tv_sec;
    tv_usec = src.tv_usec;
}


void
TimeVal::normalize()
{
    // The purpose of normalize is to ensure that the tv_usec field
    // is in [0,100000).
    //

    if ( tv_usec < 0 )
    {
        long num_sec = ( -tv_usec / 1000000) + 1;

        assert(num_sec > 0);

        tv_sec -= num_sec;
        tv_usec += num_sec * 1000000;

        // The result of this operation may have tv_usec == 1000000.
        //
    }


    if ( tv_usec >= 1000000 )
    {
        tv_sec += tv_usec / 1000000;
        tv_usec %= 1000000;
    }
}


ostream &
TimeVal::writeTo(ostream & out) const
{
    return ( out << "{ " << tv_sec << ", " << tv_usec << " }" );
}

