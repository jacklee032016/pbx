#if !defined(ASSIST_DEPRECATED_THREAD_HXX)
#define ASSIST_DEPRECATED_THREAD_HXX
/*
* $Id: DeprecatedThread.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#if defined(ASSIST_USE_DEPRECATED)

    #if defined(ASSIST_WARN_DEPRECATED)

        #warning \
            Assist::Process::Thread has been deprecated. \
            Use Assist::Threads::Thread.

    #endif // defined(ASSIST_WARN_DEPRECATED)

    namespace Assist {
    namespace Process {

        typedef Assist::Threads::Thread   Thread;

    } // namespace Process
    } // namespace Assist

#else

    #error \
        Assist::Process::Thread no longer supported. \
        Use Assist::Threads::Thread.

#endif // defined(ASSIST_USE_DEPRECATED)


#endif // !defined(ASSIST_DEPRECATED_THREAD_HXX)
