/*
 * $Id: NtpTime.cxx,v 1.2 2007/03/01 20:04:31 lizhijie Exp $
 */

#include "global.h"
#include "NtpTime.hxx"

#include <stdio.h>
#include <iostream>
#include <sys/types.h>
#include "vtypes.h"
#ifndef __vxworks
#include <sys/time.h>
#endif
#include "VTime.hxx"
#include <unistd.h>
#include <assert.h>


void NtpTime::print()
{
    fprintf (stderr, "%u.%010u", seconds, fractional);
}

// 2^32 = 4294,967,296
NtpTime operator+( const NtpTime& lhs , const unsigned int msec)
{
    NtpTime result (0, 0);

    u_int32_t delayFrac = (msec % 1000) * 4294967;

    result.seconds = lhs.seconds + (msec / 1000);
    result.fractional = lhs.fractional + delayFrac;
    if ( (lhs.fractional > result.fractional) && (delayFrac > result.fractional) )
        result.seconds++;

    return result;
}


NtpTime operator-( const NtpTime& lhs , const unsigned int msec)
{
    NtpTime result (0, 0);

    u_int32_t delayFrac = (msec % 1000) * 4294967;

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

// It returns the difference in milisec between lhs and rhs
int operator-( const NtpTime& lhs , const NtpTime& rhs )
{
    NtpTime result;
    unsigned int msResult;

    if (lhs == rhs) return 0;

    if (lhs > rhs)
    {
        result.seconds = lhs.seconds - rhs.seconds;

        if (lhs.fractional > rhs.fractional)
        {
            result.fractional = lhs.fractional - rhs.fractional;
        }
        else if (lhs.fractional < rhs.fractional)
        {
            result.seconds--;
            result.fractional = lhs.fractional - rhs.fractional;
        }
        else
        {
            result.fractional = 0;
        }
        msResult = (result.getSeconds() * 1000)
                   + (result.getFractional() / 4294967);
    }
    else
    {
        result.seconds = rhs.seconds - lhs.seconds;

        if (rhs.fractional >= lhs.fractional)
        {
            result.fractional = rhs.fractional - lhs.fractional;
        }
        else if (rhs.fractional < lhs.fractional)
        {
            result.seconds--;
            result.fractional = rhs.fractional - lhs.fractional;
        }
        else
        {
            result.fractional = 0;
        }
        msResult = -( (result.getSeconds() * 1000)
                      + (result.getFractional() / 4294967));
    }

    return msResult;
}


bool operator==( const NtpTime& rhs , const NtpTime& lhs )
{
    return (rhs.seconds == lhs.seconds) ?
           (rhs.fractional == lhs.fractional) : (rhs.seconds == lhs.seconds);
}

bool operator<( const NtpTime& rhs , const NtpTime& lhs )
{
    return (rhs.seconds == lhs.seconds) ?
           (rhs.fractional < lhs.fractional) : (rhs.seconds < lhs.seconds);
}


bool operator>( const NtpTime& rhs , const NtpTime& lhs )
{
    return (rhs.seconds == lhs.seconds) ?
           (rhs.fractional > lhs.fractional) : (rhs.seconds > lhs.seconds);
}


NtpTime getNtpTime()
{
    struct timeval now;
    int err = gettimeofday(&now, NULL);
    assert( !err );

    NtpTime result ( now.tv_sec, now.tv_usec*4294);

    return result;
}
