#if !defined(ASSIST_DEPRECATED_SIGNAL_SET_HXX)
#define ASSIST_DEPRECATED_SIGNAL_SET_HXX
/*
* $Id: DeprecatedSignalSet.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#if defined(ASSIST_USE_DEPRECATED)

    #if defined(ASSIST_WARN_DEPRECATED)

        #warning \
            Assist::Process::SignalSet has been deprecated. \
            Use Assist::Signals::SignalSet.

    #endif // defined(ASSIST_WARN_DEPRECATED)

    namespace Assist {
    namespace Process {

        typedef Assist::Signals::SignalSet   SignalSet;

    } // namespace Process
    } // namespace Assist

#else

    #error \
        Assist::Process::SignalSet no longer supported. \
        Use Assist::Signals::SignalSet.

#endif // defined(ASSIST_USE_DEPRECATED)


#endif // !defined(ASSIST_DEPRECATED_SIGNAL_SET_HXX)
