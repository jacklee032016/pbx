#if !defined(ASSIST_DEPRECATED_SIGNAL_ACTION_HXX)
#define ASSIST_DEPRECATED_SIGNAL_ACTION_HXX
/*
* $Id: DeprecatedSignalAction.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#if defined(ASSIST_USE_DEPRECATED)

    #if defined(ASSIST_WARN_DEPRECATED)

        #warning \
            Assist::Process::SignalAction has been deprecated. \
            Use Assist::Signals::SignalAction.

    #endif // defined(ASSIST_WARN_DEPRECATED)

    namespace Assist {
    namespace Process {

        typedef Assist::Signals::SignalAction   SignalAction;

    } // namespace Process
    } // namespace Assist

#else

    #error \
        Assist::Process::SignalAction no longer supported. \
        Use Assist::Signals::SignalAction.

#endif // defined(ASSIST_USE_DEPRECATED)


#endif // !defined(ASSIST_DEPRECATED_SIGNAL_ACTION_HXX)
