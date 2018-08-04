#if !defined(VOCAL_DEPRECATED_SIGNAL_SET_HXX)
#define VOCAL_DEPRECATED_SIGNAL_SET_HXX

/*
 * $Id: DeprecatedSignalSet.hxx,v 1.2 2007/03/01 20:04:30 lizhijie Exp $
 */


#if defined(VOCAL_USE_DEPRECATED)

    #if defined(VOCAL_WARN_DEPRECATED)

        #warning \
            Vocal::Process::SignalSet has been deprecated. \
            Use Vocal::Signals::SignalSet.

    #endif // defined(VOCAL_WARN_DEPRECATED)

    namespace Vocal {
    namespace Process {

        typedef Vocal::Signals::SignalSet   SignalSet;

    } // namespace Process
    } // namespace Vocal

#else

    #error \
        Vocal::Process::SignalSet no longer supported. \
        Use Vocal::Signals::SignalSet.

#endif // defined(VOCAL_USE_DEPRECATED)


#endif // !defined(VOCAL_DEPRECATED_SIGNAL_SET_HXX)
