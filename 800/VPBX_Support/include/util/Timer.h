#ifndef TIMER_H_
#define TIMER_H_

/*
* $Id: Timer.h,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#include "debug.h"
#include "VMutex.h"
#include "VTime.hxx"
#include <unistd.h>

#include "global.h"
#include <list>

bool operator < ( const timeval& lhs, const timeval& rhs);
void addMs ( timeval* lhs, int timeMs);
void subMs ( timeval* lhs, int timeMs);

long tv_diff ( const timeval& lhs, const timeval& rhs);


typedef unsigned int TimerEventId ;
template < class X >
class TimerObj
{
    public:
        TimerObj()
        {}


        TimerObj(const TimerObj& x) :
        myTime(x.myTime),
        myObject(x.myObject),
        eventId(x.eventId)
        {}


        TimerObj(const timeval& time, X object) :
        myTime(time),
        myObject(object),
        eventId(0)
        {}

        // assignment operator

        void operator = (const TimerObj& x)
        {
            myTime = x.myTime;
            myObject = x.myObject;
            eventId = x.eventId;
        }

        // equality check operator
        bool operator == (const TimerObj& x)
        {
            if ((myTime.tv_sec == x.myTime.tv_sec) && (myTime.tv_usec == x.myTime.tv_usec) && (myObject == x.myObject) && (eventId == x.eventId))
                return ( true );
            return ( false );
        }

        timeval myTime;
        X myObject;
        TimerEventId eventId;
};

template < class X > bool operator < (const TimerObj < X > & x, const TimerObj < X > & y)
{
    return x.myTime < y.myTime;
}

template < class T >
class Timer
{
    public:
        Timer();
        Timer(long usecWait, T bad);
        Timer(const timeval& minWait, T bad);
        void insert(T obj, int msDelay);
        void remove(const TimerObj < T > & obj);
        TimerObj < T > getlastInserted()
        {
            return lastInserted;
        }
        bool sleepFor(timeval* t);

        bool ready();
        T top();
        void pop();
    private:
        TimerEventId nextEventId;
        list < TimerObj < T > > events;
        //    priority_queue<TimerObj<T> > events;
        VMutex* timerMutex;
        timeval myMinWait;
        T bogus;

        static TimerObj < T > lastInserted;
};

template < class T > TimerObj < T > Timer < T > ::lastInserted = TimerObj < T > ();

template < class T >
Timer < T > ::Timer()
        : nextEventId(0)
{
    timerMutex = new VMutex;

    myMinWait.tv_sec = 0;
    myMinWait.tv_usec = 0;

    TimerObj < T > foo;

    events.push_back(foo);
    pop();
}

template < class T >
Timer < T > ::Timer(long usecWait, T bad)
        : nextEventId(0),
        bogus(bad)
{
    timerMutex = new VMutex;

    myMinWait.tv_sec = 0;
    myMinWait.tv_usec = usecWait;

    while(myMinWait.tv_usec > 1000000)
    {
        myMinWait.tv_sec++;
        myMinWait.tv_usec -= 1000000;
    }

    TimerObj < T > foo;

    events.push_back(foo);
    pop();
}

template < class T >
Timer < T > ::Timer(const timeval& minWait, T bad)
        : nextEventId(0),
        myMinWait(minWait),
        bogus(bad)
{
    timerMutex = new VMutex;

    TimerObj < T > foo;

    events.push_back(foo);
    pop();
}



template < class T >
bool Timer < T > ::sleepFor(timeval* t)
{
    timeval now;
    bool retval;

    int err = gettimeofday(&now, NULL);
    assert( !err );

    timerMutex->lock();

    if (!events.empty())
    {
        if (events.front().myTime < now)
        {
            // indicate almost no time to wait!
            t->tv_sec = 0;
            t->tv_usec = 1;
            XDEBUG(3, cout << "diff: " << tv_diff(events.front().myTime, now) << endl; ) ;
            retval = false;
        }
        else
        {
            t->tv_sec = events.front().myTime.tv_sec - now.tv_sec;
            t->tv_usec = events.front().myTime.tv_usec - now.tv_usec;
            while (t->tv_usec < 0)
            {
                t->tv_sec--;
                t->tv_usec += 1000000;
            }
            retval = true;
        }
    }
    else
    {
        *t = myMinWait;
        retval = true;
    }

    timerMutex->unlock();

    return retval;
}


template < class T >
void Timer < T > ::insert(T obj, int msDelay)
{
    assert( INT_MAX > 2000000000 );
    assert( INT_MIN < -2000000000 );

    timeval now;
    int err = gettimeofday(&now, NULL);
    assert( !err );

    XDEBUG(3, cout << "DELAY: " << msDelay << endl;
          );
    addMs(&now, msDelay);

    TimerObj < T > x(now, obj);

    // add the event to the priority queue

    // lock the object
    timerMutex->lock();

    // get a new ID
    TimerEventId id = nextEventId++;
    x.eventId = id;

    // put the message on the FIFO
    events.push_back( x );

    // save the last inserted item
    lastInserted = x;

    // get the list into priority order
    events.sort();

    // unlock the object
    timerMutex->unlock();
}


template < class T >
void Timer < T > ::remove(const TimerObj < T > & obj )
{
    if(!events.empty())
    {
        timerMutex->lock();
        events.remove( obj );

        timerMutex->unlock();
    }
}

template < class T >
void Timer < T > ::pop()
{
    if(!events.empty())
    {
        timerMutex->lock();

        events.pop_front();

        timerMutex->unlock();
    }
}

template < class T >
bool Timer < T > ::ready()
{
    timeval now;
    int err = gettimeofday(&now, NULL);
    assert( !err );

    if (!events.empty())
    {
        if (events.front().myTime < now)
        {
            return true;
        }
    }

    return false;
}

template < class T >
T Timer < T > ::top()
{
    if(!events.empty())
    {
        return events.front().myObject;
    }
    return bogus;
}
#endif
