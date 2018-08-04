#if !defined(VOCAL_DEPRECATED_SIGNAL_HANDLER_HXX)
#define VOCAL_DEPRECATED_SIGNAL_HANDLER_HXX

/*
 * $Id: DeprecatedSignalHandler.hxx,v 1.2 2007/03/01 20:04:30 lizhijie Exp $
 */


#if defined(VOCAL_USE_DEPRECATED)

    #if defined(VOCAL_WARN_DEPRECATED)

        #warning \
            Vocal::Process::SignalHandler has been deprecated. \
            Use Vocal::Signals::SignalHandler.

    #endif // defined(VOCAL_WARN_DEPRECATED)

    namespace Vocal {
    namespace Process {

        typedef Vocal::Signals::SignalHandler  SignalHandler;

    } // namespace Process
    } // namespace Vocal

#else

    #error \
        Vocal::Process::SignalHandler no longer supported. \
        Use Vocal::Signals::SignalHandler.

#endif // defined(VOCAL_USE_DEPRECATED)


#endif // !defined(VOCAL_DEPRECATED_SIGNAL_HANDLER_HXX)
