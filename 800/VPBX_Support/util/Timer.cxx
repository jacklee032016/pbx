/*
* $Id: Timer.cxx,v 1.1.1.1 2006/11/30 16:27:10 lizhijie Exp $
*/

#include "global.h"
#include "Timer.h"

/// compares two timevals
bool operator < ( const timeval& lhs, const timeval& rhs)
{
    bool ret;

    if (lhs.tv_sec == rhs.tv_sec)
    {
        ret = (lhs.tv_usec < rhs.tv_usec);
    }
    else
    {
        ret = (lhs.tv_sec < rhs.tv_sec);
    }
    return ret;
}

void addMs ( timeval* lhs, int timeMs)
{
    long timeUsec = timeMs * 1000;
    lhs->tv_usec += timeUsec;

    while (lhs->tv_usec > 1000000)
    {
        lhs->tv_sec += 1;
        lhs->tv_usec -= 1000000;
    }
}

void subMs ( timeval* lhs, int timeMs)
{
    long timeUsec = timeMs * 1000;
    long usecs = lhs->tv_usec - timeUsec;

    while (usecs < 0)
    {
        lhs->tv_sec -= 1;
        usecs += 1000000;
    }
    lhs->tv_usec = usecs;
}

long tv_diff ( const timeval& lhs, const timeval& rhs)
{
    return (lhs.tv_sec - rhs.tv_sec) * 1000000
           + ( lhs.tv_usec - rhs.tv_usec);
}
