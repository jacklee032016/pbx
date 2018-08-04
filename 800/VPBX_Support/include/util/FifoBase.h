#if !defined(ASSIST_FIFOBASE_H)
#define ASSIST_FIFOBASE_H
/*
* $Id: FifoBase.h,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#include "Mutex.hxx"
#include "Condition.hxx"
#include "TimerContainer.hxx"
#include "Sptr.hxx"
#include "Lock.hxx"
#include "TimeVal.hxx"
#include "NonCopyable.hxx"
#include <list>



using namespace Assist::TimeAndDate;


namespace Assist {
namespace TimeAndDate {
class TimeVal;
}
}


/** Infrastructure common to ASSIST.
 */
namespace Assist 
{


using Assist::TimeAndDate::TimeVal;
using Assist::Threads::Mutex;
using Assist::Threads::Condition;
using Assist::Threads::Lock;
using std::list;


/** 
    First in first out queue interface, with the added functionality of 
    being able to handle timed entries.
    <P>
    <B>Example 1:</b>
    <p>
    In a shared context:
    <pre>
      Fifo&lt;int> f;
    </pre>
    <p>
    In thread 1:
    <p>
    <pre>
      f.add(1); // add the number 1 to the fifo f
    </pre>
    <p>
    In thread 2:
    <p>
    <pre>
      int x = f.getNext();
      // now, x == 1
    </pre>

    <b>Example 2:</b>
    <p>
    In a shared context:
    <pre>
      Fifo&lt;int> f;
    </pre>

    <p>
    In thread 1:
    <p>
    <pre>
      // add the number 1 to the fifo f after 1000 ms delay
      f.addDelayMs(1, 1000); 
    </pre>
    <p>
    In thread 2:
    <p>
    <pre>
      int x = f.getNext();
      // now, x == 1
      
      // note that this will block for 1000ms for the number to be put
      // in the fifo.
    </pre>

 */
template < class Msg >
class FifoBase : public NonCopyable
{
   public:


      /** Id for delayed events. Needed to cancel an event.
       */
      typedef Assist::TimeAndDate::TimerEntryId EventId;


   protected:


      /** Create an empty fifo.
       */
      FifoBase();


   public:

      /** Delete the fifo.
       */
      virtual ~FifoBase();


      /** Add a message to the fifo.
       */
      void	add(const Msg &);


      /** If the relative timeout is greater than 0, the message will 
       *  be added to the fifo after the number of milliseconds have passed.
       *          
       *  The returned value is an opaque id that can be used
       *  to cancel the event before the timer expires. If the
       *  time is specified as 0, this defaults the to the
       *  regular add(), returning an id of 0.
       */
      EventId addDelay(
         const Msg &,
         const TimeVal & relativeTimeout);


      /** If the expiry time is later than the time now, the message will 
       *  be added to the fifo after the time has passed.
       *          
       *  The returned value is an opaque id that can be used
       *  to cancel the event before the timer expires. If the
       *  time is specified as 0, this defaults the to the
       *  regular add(), returning an id of 0.
       */
      EventId addUntil(
         const Msg &,
         const TimeVal & when);


      /** If the relative timeout (in milliseconds) is greater than 0, 
       *  the message will be added to the fifo after the number of 
       *  milliseconds have passed.
       *          
       *  The returned value is an opaque id that can be used
       *  to cancel the event before the timer expires. If the
       *  time is specified as 0, this defaults the to the
       *  regular add(), returning an id of 0.
       */
      EventId addDelayMs(
         const Msg &,
         const milliseconds_t relativeTimeout);


      /** If the expiry time (in milliseconds) is later than the
       *  time now, the message will be added to the fifo after the
       *  time has passed.
       *          
       *  The returned value is an opaque id that can be used
       *  to cancel the event before the timer expires. If the
       *  time is specified as 0, this defaults the to the
       *  regular add(), returning an id of 0.
       */
      EventId addUntilMs(
         const Msg &,
         const milliseconds_t when);


      /** Cancel a delayed event.
       */
      void cancel(EventId);


      /** Blocks until a message is available, or the specified timeout has
       *  expired. If the return code is 0, then the fifo was interrupted
       *  via a signal or the timeout expired without a message being added
       *  to the queue. Using block is the only way to guarantee detection
       *  of a signal has been thrown.
       */
      int block(milliseconds_t relativeTimeout = -1)
         throw ( VException );


      /** Returns the first message available. It will wait if no
       *  messages are available. If a signal interrupts the wait,
       *  it will retry the wait. Signals can therefore not be caught
       *  via getNext. If you need to detect a signal, use block
       *  prior to calling getNext.
       */
      Msg getNext() throw ( VException );


      /** Get the current size of the fifo. Note that the current
       *  size includes all of the pending events, even those which
       *  have not yet activated so you should not use this function
       *  to determine whether a call to getNext() will block or not.
       *  Use messageAvailable() instead.
       */
      unsigned int size() const;


      /** Get the current number of messages available. Note that 
       *  the size does not include all of the pending events.
       *  You may not want to this function to determine the number of times
       *  you could call getNext() before it would block, since messages
       *  may become available during message processing. Consider using 
       *  messageAvailable() instead.
       */
      unsigned int sizeAvailable() const;


      /** Get the current number of messages pending. Note that the current
       *  size does not include the available messages. You should not use 
       *  this function to determine whether a call to getNext() will block 
       *  or not. Use messageAvailable() instead.
       */
      unsigned int sizePending() const;


      /** Returns true if a message is available.
       */
      bool	messageAvailable();


      /** Indicate that the fifo no longer accepts messages.
       */
      void	shutdown();


      /** Relational operators. Useful if this is ever in a container.
       */
      bool	operator==(const FifoBase &) const;


      /** Relational operators. Useful if this is ever in a container.
       */
      bool	operator!=(const FifoBase &) const;


      /** Relational operators. Useful if this is ever in a container.
       */
      bool	operator< (const FifoBase &) const;


      /** Relational operators. Useful if this is ever in a container.
       */
      bool	operator<=(const FifoBase &) const;


      /** Relational operators. Useful if this is ever in a container.
       */
      bool	operator> (const FifoBase &) const;


      /** Relational operators. Useful if this is ever in a container.
       */
      bool	operator>=(const FifoBase &) const;


   protected:

      /** Returns true if a message is available. The default implementation
       *  looks at the size of the fifo. Derived classes may also observe
       *  other resources as well. Note that the mutex should not be locked
       *  by this routine, since it would cause a deadlock.
       */
      virtual bool	messageAvailableNoLock();


      /** Wake's up the blocked thread. Note that the mutex is locked
       *  upon entry. It is the responsibility of the user to unlock
       *  the mutex if necessary, making sure to relock it before exit.
       */
      virtual void	wakeup() throw ( VException ) = 0;


      /** Blocks the thread, for the given time, waiting for a message.
       *  Note that the mutex is locked upon entry. It is the responsibility
       *  of the user to unlock the mutex if necessary, making sure
       *  to relock it before exit. The return code is positive for
       *  to indicate activity, i.e. messages added to the queue. For
       *  a signal or a timeout, 0 should be returned.
       */
      virtual int	wait(milliseconds_t relativeTimeout)
         throw ( VException ) = 0;


      typedef list < Sptr < Msg > > MessageContainer;


      /**
       */
      MessageContainer    myFifo;
      unsigned long myFifoSize;

      /** Protects fifo and timer.
       */
      mutable Threads::Mutex  myMutex;

   private:

      int blockNoLock(milliseconds_t relativeTimeout = -1)
         throw ( VException );


      void postAddTimed(const EventId &);


      Assist::TimeAndDate::TimerContainer < Msg > myTimer;
      unsigned long myTimerSize;


      bool	myShutdown;
};


/** Opaque id so that time delayed fifo events may be cancelled.
 *  For backwards compatibility. Use FifoBase::EventId instead.
 */
typedef     Assist::TimeAndDate::TimerEntryId 	FifoEventId;


#include "FifoBase.cc"


} // namespace Assist


#endif // !defined(ASSIST_FIFOBASE_H)
