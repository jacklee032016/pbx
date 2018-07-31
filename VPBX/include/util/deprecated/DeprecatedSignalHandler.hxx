#if !defined(ASSIST_DEPRECATED_SIGNAL_HANDLER_HXX)
#define ASSIST_DEPRECATED_SIGNAL_HANDLER_HXX
/*
* $Id: DeprecatedSignalHandler.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#if defined(ASSIST_USE_DEPRECATED)

    #if defined(ASSIST_WARN_DEPRECATED)

        #warning \
            Assist::Process::SignalHandler has been deprecated. \
            Use Assist::Signals::SignalHandler.

    #endif // defined(ASSIST_WARN_DEPRECATED)

    namespace Assist {
    namespace Process {

        typedef Assist::Signals::SignalHandler  SignalHandler;

    } // namespace Process
    } // namespace Assist

#else

    #error \
        Assist::Process::SignalHandler no longer supported. \
        Use Assist::Signals::SignalHandler.

#endif // defined(ASSIST_USE_DEPRECATED)


#endif // !defined(ASSIST_DEPRECATED_SIGNAL_HANDLER_HXX)
