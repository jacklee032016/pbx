#ifndef TIMER_CONTAINER_DOT_H
#define TIMER_CONTAINER_DOT_H
/*
 * $Id: TimerContainer.hxx,v 1.2 2007/03/01 20:04:29 lizhijie Exp $
 */

#include <set>
#include <map>

#include "TimerEntry.hxx"
#include "Writer.hxx"
#include "Sptr.hxx"
#include "Lock.hxx"
#include "Mutex.hxx"
#include "VMissingDataException.hxx"
#include "TimeVal.hxx"


/** Infrastructure common to VOCAL.
 */
namespace Vocal 
{


/** Infrastructure common to VOCAL to manipulate the time.<br><br>
 */
namespace TimeAndDate
{

using Vocal::Threads::Lock;

class TimeVal;


/** An ordered list of TimerEntries. This list is a list of events and the
 *  time those events expire.<br><br>
 *
 *  @see    Vocal::TimeAndDate::TimerEntry
 */
template < class Msg >
class TimerContainer : public Vocal::IO::Writer
{
    public:


    	/** Default constructor
	 */
        TimerContainer();


    	/** Virtual destructor
	 */
        virtual	~TimerContainer();


        /** Add a new message to the timer.
         */
        TimerEntryId	add(Sptr < Msg > ,
                         milliseconds_t when,
                         bool           relative = true);


        /** Add a new message to the timer.
         */
        TimerEntryId	add(Sptr < Msg > ,
                         const TimeVal &    when,
                         bool               relative = true);


        /** Cancel a delayed event. Returns true is id was found, and
         *  false otherwise.
         */
        bool cancel(TimerEntryId);


        /** Get the timeout value for the first available
         *  message. Returns -1 if no messages available
         *  (conveying infinite timeout).
         */
        int getTimeout();


        /** Returns the identifier of the first entry.
         */
        TimerEntryId	getFirstTimerEntryId();


        /** Returns true if a message is available.
         */
        bool	messageAvailable();


        /** Returns the first message available. Throws a
         * status exception if no messages are available.
         */
        Sptr < Msg > getMessage()
        throw ( VMissingDataException );


    	/** Write a TimerContainer to an ostream.
	 */
        ostream & writeTo(ostream & out) const;


        /** Return the number of all the pending events in the TimerContainer
	 */
        unsigned int size() const;

        ///
        mutable Threads::Mutex      myMutex;


    private:

      void postAdd(const TimerEntry<Msg> &);

      typedef std::multiset< TimerEntry<Msg> > TimerSet;
      TimerSet _timerSet;

      std::map<TimerEntryId, TimerEntry<Msg> > _idToTimer;
};


#include "TimerContainer.cc"


} // namespace Time
} // namespace Vocal


#endif // !defined(TIMER_CONTAINER_DOT_H)
