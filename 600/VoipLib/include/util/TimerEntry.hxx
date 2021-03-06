#ifndef TIMER_ENTRY_DOT_H
#define TIMER_ENTRY_DOT_H
/*
 * $Id: TimerEntry.hxx,v 1.2 2007/03/01 20:04:29 lizhijie Exp $
 */


#include "Writer.hxx"
#include "Sptr.hxx"
#include "TimeVal.hxx"
#include <cassert>


/** Infrastructure common to VOCAL.
 */
namespace Vocal 
{


/** Infrastructure common to VOCAL to manipulate the time.
 */
namespace TimeAndDate
{


#if !defined(VOCAL_MILLISECONDS_T)
#define VOCAL_MILLISECONDS_T

/* 1/1000th of a seconds.<br><br>
 */
typedef int milliseconds_t;

#endif // !defined(VOCAL_MILLISECONDS_T)


/** The key for a timer entry.<br><br>
 */
typedef void * TimerEntryId;


/** An entry in a TimerContainer.<br><br>
 *
 *  A TimerEntry holds some type of event and the time the event is 
 *  supposed to take place. The TimerContainer is an ordered list 
 *  (by time) of TimerEntries.
 *
 *  @see    Vocal::TimeAndDate::TimerContainer
 *  @see    Vocal::TimeAndDate::TimeVal
 */
template < class Msg >
class TimerEntry : public Vocal::IO::Writer
{
    public:

	/** Construct with an event and the time of expiration (in ms).
	 */
        TimerEntry(Sptr < Msg > , milliseconds_t time, bool relative = true);


	/** Construct with an event and the time of expiration.
	 */
        TimerEntry(Sptr < Msg > , const TimeVal & time, bool relative = true);


	/** Copy constructor.
	 */
        TimerEntry(const TimerEntry &);

        TimerEntry();

	/** Virtual destructor.
	 */
        virtual ~TimerEntry();


	/** Assignment operator.
	 */
        TimerEntry &	operator=(const TimerEntry &);


	/** Return the time of expiration for the event.
	 */
        milliseconds_t	getTimeout() const;


	/** Return the opaque id for this timer entry.
	 */
        TimerEntryId	getId() const;


	/** Return the associated event.
	 */
        Sptr < Msg > getMessage() const;


	/** Returns true if the expiration time for the associcated 
	 *  event has passed.
	 */
        bool	hasExpired() const;


	/** Equality relational operator.
	 */
        bool	operator==(const TimerEntry &) const;


	/** Inequality relational operator.
	 */
        bool	operator!=(const TimerEntry &) const;


	/** Less than relational operator.
	 */
        bool	operator< (const TimerEntry &) const;


	/** Less than or equal to relational operator.
	 */
        bool	operator<=(const TimerEntry &) const;


	/** Greater than relational operator.
	 */
        bool	operator> (const TimerEntry &) const;


	/** Greater than or equal to relational operator.
	 */
        bool	operator>=(const TimerEntry &) const;


	/** Write the TimerEntry to an ostream.
	 */
        virtual ostream & writeTo(ostream &) const;


    private:

        TimeVal     	absExpires_;
        Sptr < Msg > 	msg_;
};


#include "TimerEntry.cc"


} // namespace Time
} // namespace Vocal




#endif // !defined(TIMER_ENTRY_DOT_H)
