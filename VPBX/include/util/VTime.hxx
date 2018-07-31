#ifndef VTIME_H_
#define VTIME_H_

/*
* $Id: VTime.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#ifdef __vxworks

#include <time.h>
#include <sys/times.h>

/**
 ** Operates the same as linux gettimeofday() except the
 ** usec field of struct timeval is rounded to nearest msec
 ** (eg. usec = msec*1000)
 */
int
gettimeofday (struct timeval *tv, struct timezone *tz);

#else

#include "global.h"
#include <sys/time.h>

#endif
// __vxworks

#ifdef WIN32
int
gettimeofday (struct timeval *tv, struct timezone *tz);
#endif // WIN32

#include <sys/types.h>
#include "vtypes.h"
// stuff here
#include <string>


/** Class VTime
 ** Defines a time class which provides means of
 ** creating, adding, subtracting, and comparing times. 
 ** VTime objects contain a second component and a fraction of second
 ** component. Both may be specified and accessed by the user.
 ** Routines to add and subtract times in terms of milliseconds are
 ** also provided.
 **/
class VTime
{
    public:
        /// Create VTime object with specified seconds and fraction of a second.
        VTime( u_int32_t sec, u_int32_t frac ): seconds(sec), fractional(frac)
        {}

        /// Create a new VTime object with the current system time.
        VTime()
        {
            *this = getVTime();
        }

        /// Access functions
        u_int32_t getSeconds()
        {
            return seconds;
        }
        u_int32_t getFractional()
        {
            return fractional;
        }

        /// Display time stored in VTime in decimal notation
        void print();

        static string strftime(const string & format);


    private:
        // this is the numbers of seconds
        u_int32_t seconds;
        /** this is the fraction of the second.  1 second == 2^32 fractional .
         */
        u_int32_t fractional;

        /// add specified number of milliseconds to VTime
        friend VTime operator+( const VTime& lhs , const unsigned int msec );

        /// subtract specified number of milliseconds from VTime
        friend VTime operator-( const VTime& lhs , const unsigned int msec );

        /// returns difference in terms of milliseconds, or 0 if rhs > lhs
        friend int operator-( const VTime& lhs , const VTime& rhs );

        friend bool operator==( const VTime& lhs , const VTime& rhs );
        friend bool operator<( const VTime& lhs , const VTime& rhs );
        friend bool operator>( const VTime& lhs , const VTime& rhs );

        /// returns a VTime object based on the current system time
        friend VTime getVTime();


};

// V_TIME_H_
#endif
