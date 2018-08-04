#if !defined(ASSIST_DEPRECATED_APPLICATION_HXX)
#define ASSIST_DEPRECATED_APPLICATION_HXX
/*
* $Id: DeprecatedApplication.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#if defined(ASSIST_USE_DEPRECATED)

    #if defined(ASSIST_WARN_DEPRECATED)

        #warning \
            Assist::Process::Application has been deprecated. \
            Use Assist::Application.

    #endif // defined(ASSIST_WARN_DEPRECATED)

    namespace Assist {
    namespace Process {

        typedef Assist::Application   Application;

    } // namespace Process
    } // namespace Assist

#else

    #error \
        Assist::Process::Application no longer supported. \
        Use Assist::Application.

#endif // defined(ASSIST_USE_DEPRECATED)


#endif // !defined(ASSIST_DEPRECATED_APPLICATION_HXX)
