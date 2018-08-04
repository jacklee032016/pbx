#if !defined(ASSIST_CONDITION_HXX)
#define ASSIST_CONDITION_HXX
/*
* $Id: Condition.hxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/


#include "NonCopyable.hxx"
#include "Mutex.hxx"


/** Infrastructure common to ASSIST.
 */
namespace Assist 
{


/** Infrastructure common to ASSIST to create and manage threads.
 */
namespace Threads
{


/** Defines a simple conditional variable class which
 *  allows threads to block until shared data changes state.
 *  A VCondition must always be associated with a
 *  VMutex to avoid the race condition where a thread prepares to
 *  wait on a condition variable and another thread signals the
 *  condition just before the first thread actually waits on it.
 */
class Condition : public NonCopyable
{
    public:


        /** Create an Condition object initialized with operating system
         *  dependant defaults (if any). 
         */
        Condition();


        /** Delete a Condition object
         */
        virtual ~Condition();


        /** Block on the condition. Will return after the relativeTime, 
         *  specified in millseconds, has passed. If set to -1, the default,
         *  it will wait indefinately. Uses the specified mutex to synchronize 
         *  access to the condition. Returns 0, if successful, or an errorcode.
         */
        int wait (Mutex* mutex, int relativeTimeInMillis = -1);


        /** Block on the condition. Will return after the relativeTime, 
         *  specified in microseconds, has passed. If set to -1, the default,
         *  it will wait indefinately. Uses the specified mutex to synchronize 
         *  access to the condition. Returns 0, if successful, or an errorcode.
         */
        int uwait(Mutex* mutex, int relativeTimeInMicros = -1);


        /** Signal one waiting thread.
         *  Returns 0, if successful, or an errorcode.
         */
        int signal();


        /** Signal all waiting threads.
         *  Returns 0, if successful, or an errorcode.
         */
        int broadcast();
         

        /** Returns the operating system dependent unique id of the condition.
         */
        const vcondition_t*
        getId() const;

    private:

        mutable vcondition_t myId;
};


} // namespace Threads
} // namespace Assist


/** usleep() replacement.<br><br> 
 *
 *  This will not throw a SIGALRM.
 */
void vusleep(unsigned long usec);


#endif // !defined(ASSIST_CONDITION_HXX)
