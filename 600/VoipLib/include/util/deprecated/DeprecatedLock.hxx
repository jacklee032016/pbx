#if !defined(VOCAL_DEPRECATED_LOCK_HXX)
#define VOCAL_DEPRECATED_LOCK_HXX

/*
 * $Id: DeprecatedLock.hxx,v 1.2 2007/03/01 20:04:30 lizhijie Exp $
 */


#if defined(VOCAL_USE_DEPRECATED)

    #if defined(VOCAL_WARN_DEPRECATED)

        #warning \
            Vocal::Process::Lock has been deprecated. \
            Use Vocal::Threads::Lock.

    #endif // defined(VOCAL_WARN_DEPRECATED)

    namespace Vocal {
    namespace Process {

        typedef Vocal::Threads::Lock    Lock;

    } // namespace Process
    } // namespace Vocal

#else

    #error \
        Vocal::Process::Lock no longer supported. \
        Use Vocal::Threads::Lock.

#endif // defined(VOCAL_USE_DEPRECATED)


#endif // !defined(VOCAL_DEPRECATED_LOCK_HXX)
