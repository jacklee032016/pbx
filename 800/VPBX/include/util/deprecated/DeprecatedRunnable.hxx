#if !defined(ASSIST_DEPRECATED_RUNNABLE_HXX)
#define ASSIST_DEPRECATED_RUNNABLE_HXX
/*
* $Id: DeprecatedRunnable.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#if defined(ASSIST_USE_DEPRECATED)

    #if defined(ASSIST_WARN_DEPRECATED)

        #warning \
            Assist::Process::Runnable has been deprecated. \
            Use Assist::Runnable.

    #endif // defined(ASSIST_WARN_DEPRECATED)

    namespace Assist {
    namespace Process {

        typedef Assist::Runnable     Runnable;

    } // namespace Process
    } // namespace Assist

#else

    #error \
        Assist::Process::Runnable no longer supported. \
        Use Assist::Runnable.

#endif // defined(ASSIST_USE_DEPRECATED)


#endif // !defined(ASSIST_DEPRECATED_RUNNABLE_HXX)
