#ifndef VTHREAD_HXX_
#define VTHREAD_HXX_
/*
* $Id: VThread.hxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#include "VTime.hxx"
#include "VThreadException.hxx"

/********************************************************************
  OS independent definitions
********************************************************************/

// VThread priorities
const unsigned long VTHREAD_PRIORITY_DEFAULT = 0;

// VThread stack
const unsigned long VTHREAD_STACK_SIZE_DEFAULT = 131072; // in bytes -- 2 ^ 17

// VThread flags
const unsigned long VTHREAD_SCHED_MASK = 0x0000000f;
const unsigned long VTHREAD_SCHED_DEFAULT = 0x00000001;
const unsigned long VTHREAD_SCHED_FIFO = 0x00000002;
const unsigned long VTHREAD_SCHED_RR = 0x00000004;

/********************************************************************
  OS dependent definitions
********************************************************************/

#include "VThread_vxworks.hxx"
#include "VThread_posix.hxx"

// VThread constants
const vthread_t VTHREAD_ID_INVALID = 0;

/** Class VThread
 ** Defines a simple thread class which provides
 ** means of controlling a separate thread of control in which
 ** the scheduling mechanism and priority may be specified. Other
 ** potentially operating system dependent capabilites are
 ** provided in generic fashion as deemed necessary and useful.
 **
 ** Each instance of the class supports one thread of execution.
 ** Creating multiple threads requires creating multiple VThread objects.
 **
 ** The following exceptions may be thrown by methods of this class:
 ** VExceptionMemory(),
 ** VThreadExceptionInvalidAttributes(),
 ** VThreadExceptionInvalidPriority()
 **/
class VThread
{
    public:

        /**
         ** Create an empty VThread object
         ** Note, this does NOT start a new thread of control
         ** associated with the VThread.
         */
        VThread( );

        /**
         ** Delete a VThread object
         ** Note, this does not stop the thread of control
         ** associated with the VThread.
         */
        virtual ~VThread( );

        /**
         ** Creates a new thread of control, with the specified attributes,
         ** running startFunc with startArgs.
         **
         ** The flags are a bitwise-OR of the following:
         ** VTHREAD_SCHED_FIFO, VTHREAD_SCHED_RR, VTHREAD_SCHED_DEFAULT,
         **( rest not supported yet)
         ** VTHREAD_CANCEL_DISABLE, VTHREAD_CANCEL_ENABLE, 
         ** VTHREAD_CANCEL_DEFERRED, VTHREAD_CANCEL_ASYNCHRONOUS,
         ** VTHREAD_BOUND, VTHREAD_NEW_LWP, VTHREAD_DETACHED,
         ** VTHREAD_SUSPENDED, VTHREAD_DAEMON, VTHREAD_JOINABLE,
         **
         ** By default, or if priority is set to
         ** VTHREAD_PRIORITY_DEFAULT, an "appropriate" priority
         ** value for the given scheduling policy (specified in flags,
         ** e.g., VTHREAD_SCHED_DEFAULT) is used. This value is calculated
         ** dynamically, and is the median value between the minimum
         ** and maximum priority values for the given policy. If an
         ** explicit value is given, it is used. Note that actual
         ** priority values are EXTREMEMLY implementation-dependent,
         ** and are probably best avoided.
         ** 
         ** The following exceptions may be thrown by this method:
         ** VThreadExceptionInvalidAttributes(),
         ** VThreadExceptionInvalidPriority()
         **
         ** Returns 0, if successful, or a negative error code.
         */
        int
        spawn( void *(*startFunc)(void *),
               void *startArgs = 0,
               unsigned long flags = 0,
               unsigned long priority = VTHREAD_PRIORITY_DEFAULT,
               unsigned long stack_size = VTHREAD_STACK_SIZE_DEFAULT
             ) throw (VThreadException&);

        /**
         ** Wait for thread to exit. Returns 0 if successful, or a
         ** negative error code.
         */
        int
        join( void **status = 0 );


        /**
         ** Returns the priority of the thread
         */
        int
        getPriority() const;


        /**
         ** Set the priority of the thread. Returns 0 if successful, or a
         ** negative error code.
         **
         ** The following exceptions may be thrown by this method:
         ** VThreadExceptionInvalidPriority()
         **/
        int
        setPriority( int priority ) throw(VThreadExceptionInvalidPriority&);


        /**
         ** Return the unique Id of the VThread
         */
        const vthread_t*
        getId() const;


        /**
         ** Return ptr to the os specific attributes of the VThread
         */
        const vthread_attr_t*
        getAttributes() const;

        /**
         ** Exit the current thread of control associated with the VThread 
         */
        void
        exit();

        /**
           Return the VThread ID of the current process
        */

        static vthread_t
        selfId ();

    private:
        vthread_t myId;
        vthread_attr_t myAttributes;
};

// VTHREAD_H_
#endif
