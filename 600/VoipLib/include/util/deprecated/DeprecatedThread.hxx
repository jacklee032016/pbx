#if !defined(VOCAL_DEPRECATED_THREAD_HXX)
#define VOCAL_DEPRECATED_THREAD_HXX

/*
 * $Id: DeprecatedThread.hxx,v 1.2 2007/03/01 20:04:30 lizhijie Exp $
 */


#if defined(VOCAL_USE_DEPRECATED)

    #if defined(VOCAL_WARN_DEPRECATED)

        #warning \
            Vocal::Process::Thread has been deprecated. \
            Use Vocal::Threads::Thread.

    #endif // defined(VOCAL_WARN_DEPRECATED)

    namespace Vocal {
    namespace Process {

        typedef Vocal::Threads::Thread   Thread;

    } // namespace Process
    } // namespace Vocal

#else

    #error \
        Vocal::Process::Thread no longer supported. \
        Use Vocal::Threads::Thread.

#endif // defined(VOCAL_USE_DEPRECATED)


#endif // !defined(VOCAL_DEPRECATED_THREAD_HXX)
