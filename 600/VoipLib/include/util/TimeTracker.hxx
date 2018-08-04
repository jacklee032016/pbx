#ifndef TimeTracker_HXX_
#define TimeTracker_HXX_
/*
 * $Id: TimeTracker.hxx,v 1.2 2007/03/01 20:04:29 lizhijie Exp $
 */


#include <cstdlib>
#include <iostream>
#include <time.h>
#include <stdio.h>

class TimeTracker
{
    public:
        TimeTracker(ostream& os): _os(os)
        {
           tv_sec = 0;
           tv_usec = 0;
        };

        inline void endTime()
        {
           struct timeval tv = { 0, 0 };
           struct timezone tz = { 0, 0 };
           gettimeofday(&tv, &tz);
           long long sec = tv.tv_sec - tv_sec;
           long long usec = tv.tv_usec - tv_usec;

           char msbuf[50];
           sprintf(msbuf, "%3.3f msec", ((sec* 1000000) + usec)/1000.0);
           _os << "Time spent:" << msbuf << endl;
        }

        inline void startTime()
        {
           struct timeval tv = { 0, 0 };
           struct timezone tz = { 0, 0 };
           gettimeofday(&tv, &tz);
           tv_sec = tv.tv_sec;
           tv_usec = tv.tv_usec;
        }

        ~TimeTracker() { }
    private:
        long tv_sec;
        long tv_usec;
        ostream& _os;
};

#endif
