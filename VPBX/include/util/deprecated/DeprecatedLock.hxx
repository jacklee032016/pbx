#if !defined(ASSIST_DEPRECATED_LOCK_HXX)
#define ASSIST_DEPRECATED_LOCK_HXX
/*
* $Id: DeprecatedLock.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#if defined(ASSIST_USE_DEPRECATED)

    #if defined(ASSIST_WARN_DEPRECATED)

        #warning \
            Assist::Process::Lock has been deprecated. \
            Use Assist::Threads::Lock.

    #endif // defined(ASSIST_WARN_DEPRECATED)

    namespace Assist {
    namespace Process {

        typedef Assist::Threads::Lock    Lock;

    } // namespace Process
    } // namespace Assist

#else

    #error \
        Assist::Process::Lock no longer supported. \
        Use Assist::Threads::Lock.

#endif // defined(ASSIST_USE_DEPRECATED)


#endif // !defined(ASSIST_DEPRECATED_LOCK_HXX)
