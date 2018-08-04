#if !defined(VOCAL_DEPRECATED_RUNNABLE_HXX)
#define VOCAL_DEPRECATED_RUNNABLE_HXX
/*
 * $Id: DeprecatedRunnable.hxx,v 1.2 2007/03/01 20:04:30 lizhijie Exp $
 */


#if defined(VOCAL_USE_DEPRECATED)

    #if defined(VOCAL_WARN_DEPRECATED)

        #warning \
            Vocal::Process::Runnable has been deprecated. \
            Use Vocal::Runnable.

    #endif // defined(VOCAL_WARN_DEPRECATED)

    namespace Vocal {
    namespace Process {

        typedef Vocal::Runnable     Runnable;

    } // namespace Process
    } // namespace Vocal

#else

    #error \
        Vocal::Process::Runnable no longer supported. \
        Use Vocal::Runnable.

#endif // defined(VOCAL_USE_DEPRECATED)


#endif // !defined(VOCAL_DEPRECATED_RUNNABLE_HXX)
