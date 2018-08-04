/*
* $Id: VTime.cxx,v 1.1.1.1 2006/11/30 16:27:10 lizhijie Exp $
*/

#include "global.h"
#include <cassert>
#include <unistd.h>
#include <stdio.h>
#include <iostream>
#include "VTime.hxx"
#include "LockHelper.hxx"

#include "VMutex.h"


VMutex _mutex;


void VTime::print()
{
    fprintf( stderr, "%u.%010u", seconds, fractional );
}


VTime operator+( const VTime& lhs , const unsigned int msec )
{
    VTime result ( 0, 0 );

    u_int32_t delayFrac = ( msec % 1000 ) * 4294967;

    result.seconds = lhs.seconds + ( msec / 1000 );
    result.fractional = lhs.fractional + delayFrac;
    if ( ( lhs.fractional > result.fractional ) &&
            ( delayFrac > result.fractional ) )
    {
        result.seconds++;
    }
    return result;
}


VTime operator-( const VTime& lhs , const unsigned int msec )
{
    VTime result( 0, 0 );
    u_int32_t delayFrac = ( msec % 1000 ) * 4294967;

    if (lhs.seconds > (msec / 1000))
    {
        result.seconds = lhs.seconds - ( msec / 1000 );
        result.fractional = lhs.fractional - delayFrac;
        if ( delayFrac > lhs.fractional )
        {
            result.seconds--;
        }
    }
    else
    {
        result.seconds = 0;
        if ( delayFrac >= lhs.fractional )
        {
            result.fractional = 0;
        }
        else
        {
            result.fractional = lhs.fractional - delayFrac;
        }
    }
    return result;
}


int operator-( const VTime& lhs , const VTime& rhs )
{
    // partialresult is ms difference
    VTime result;
    int msResult;

    //assert (lhs.seconds >= rhs.seconds);
    if ( lhs == rhs ) return 0;
    //    if ( lhs < rhs ) return 0;

    if ( lhs > rhs )
    {
        result.seconds = lhs.seconds - rhs.seconds;

        if ( lhs.fractional >= rhs.fractional )
        {
            result.fractional = lhs.fractional - rhs.fractional;
        }
        else
        {
            result.seconds--;
            result.fractional = rhs.fractional - lhs.fractional;
        }
        msResult = ( ( result.getSeconds() * 1000 ) +
                     ( result.getFractional() / 4294967 ));
    }
    else
    {
        result.seconds = rhs.seconds - lhs.seconds;

        if ( rhs.fractional >= lhs.fractional )
        {
            result.fractional = rhs.fractional - lhs.fractional;
        }
        else
        {
            result.seconds--;
            result.fractional = lhs.fractional - rhs.fractional;
        }
        msResult = - ( ( ( result.getSeconds() * 1000 ) +
                         ( result.getFractional() / 4294967 ) ) );
    }

    return msResult;
}


bool operator==( const VTime& rhs , const VTime& lhs )
{
    return ( rhs.seconds == lhs.seconds ) ?
           ( rhs.fractional == lhs.fractional ) : ( rhs.seconds == lhs.seconds );
}

bool operator<( const VTime& rhs , const VTime& lhs )
{
    return ( rhs.seconds == lhs.seconds ) ?
           ( rhs.fractional < lhs.fractional ) : ( rhs.seconds < lhs.seconds );
}


bool operator>( const VTime& rhs , const VTime& lhs )
{
    return ( rhs.seconds == lhs.seconds ) ?
           (rhs.fractional > lhs.fractional ) : ( rhs.seconds > lhs.seconds );
}


VTime getVTime()
{
    struct timeval now;
    int err = gettimeofday( &now, NULL );
    assert( !err );

    VTime result ( now.tv_sec, now.tv_usec * 4294 );

    return result;
}

string
VTime::strftime(const string& format)
{
    LockHelper helper(_mutex);
    time_t now;

    (void)time(&now);

    char datebuf[256];
    struct tm localt;
#if !defined(__APPLE__)
    ::strftime(datebuf, 256, format.c_str(), localtime_r(&now, &localt));
#else
    ::strftime(datebuf, 256, format.c_str(), localtime(&now));
#endif
    return (datebuf);
}


#ifdef __vxworks
/*********************************************************************
 
 subroutines for VxWorks
 
*********************************************************************/

#include <netinet/in.h>
#include <netinet/in_systm.h>

/*
 * this implementation of gettimeoftime() uses iptime()
 * which is accurate to the nearest millisecond; microsecond
 * accuracy is faked by multiplying the milliseconds by 1000
 */
int
gettimeofday (struct timeval *tv, struct timezone *tz)
{
    unsigned long int msec;  // msec since 00:00 GMT

    // iptime() returns msec since 00:00 GMT in reverse byte order
    // call ntohl to swap it
    msec = ntohl ( iptime() );
    tv->tv_sec = ( msec / 1000 );
    tv->tv_usec = ( msec % 1000 ) * 1000;
    return 0;
}

#endif // __vxworks


#ifdef WIN32

int
gettimeofday (struct timeval *tv, struct timezone *)
{
    FILETIME file_time;
    GetSystemTimeAsFileTime (&file_time);

    ULARGE_INTEGER _100ns = {file_time.dwLowDateTime,
                           file_time.dwHighDateTime};

    _100ns.QuadPart -= 0x19db1ded53e8000;

    tv->tv_sec = long (_100ns.QuadPart / (10000 * 1000));
    tv->tv_usec = (long) ((_100ns.LowPart % (DWORD) (10000 * 1000)) / 10);

    return 0;
}

#endif // WIN32
