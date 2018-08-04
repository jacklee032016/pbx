#if !defined(VOCAL_DEPRECATED_APPLICATION_HXX)
#define VOCAL_DEPRECATED_APPLICATION_HXX

/*
 * $Id: DeprecatedApplication.hxx,v 1.2 2007/03/01 20:04:29 lizhijie Exp $
 */


#if defined(VOCAL_USE_DEPRECATED)

    #if defined(VOCAL_WARN_DEPRECATED)

        #warning \
            Vocal::Process::Application has been deprecated. \
            Use Vocal::Application.

    #endif // defined(VOCAL_WARN_DEPRECATED)

    namespace Vocal {
    namespace Process {

        typedef Vocal::Application   Application;

    } // namespace Process
    } // namespace Vocal

#else

    #error \
        Vocal::Process::Application no longer supported. \
        Use Vocal::Application.

#endif // defined(VOCAL_USE_DEPRECATED)


#endif // !defined(VOCAL_DEPRECATED_APPLICATION_HXX)
