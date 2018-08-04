#if !defined(VOCAL_DEPRECATED_SIGNAL_ACTION_HXX)
#define VOCAL_DEPRECATED_SIGNAL_ACTION_HXX

/*
 * $Id: DeprecatedSignalAction.hxx,v 1.2 2007/03/01 20:04:30 lizhijie Exp $
 */


#if defined(VOCAL_USE_DEPRECATED)

    #if defined(VOCAL_WARN_DEPRECATED)

        #warning \
            Vocal::Process::SignalAction has been deprecated. \
            Use Vocal::Signals::SignalAction.

    #endif // defined(VOCAL_WARN_DEPRECATED)

    namespace Vocal {
    namespace Process {

        typedef Vocal::Signals::SignalAction   SignalAction;

    } // namespace Process
    } // namespace Vocal

#else

    #error \
        Vocal::Process::SignalAction no longer supported. \
        Use Vocal::Signals::SignalAction.

#endif // defined(VOCAL_USE_DEPRECATED)


#endif // !defined(VOCAL_DEPRECATED_SIGNAL_ACTION_HXX)
