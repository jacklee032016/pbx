#if !defined(VOCAL_DEPRECATED_EVENT_HXX)
#define VOCAL_DEPRECATED_EVENT_HXX
/*
 * $Id: DeprecatedEvent.hxx,v 1.2 2007/03/01 20:04:30 lizhijie Exp $
 */


#if defined(VOCAL_USE_DEPRECATED)

    #if defined(VOCAL_WARN_DEPRECATED)

        #warning \
            Vocal::Signaling::Event has been deprecated. \
            Use Vocal::Services::Event.

    #endif // defined(VOCAL_WARN_DEPRECATED)

    namespace Vocal {
    namespace Signaling {

        typedef Vocal::Services::Event   Event;

    } // namespace Process
    } // namespace Vocal

#else

    #error \
        Vocal::Signaling::Event no longer supported. \
        Use Vocal::Services::Event.

#endif // defined(VOCAL_USE_DEPRECATED)


#endif // !defined(VOCAL_DEPRECATED_LOCK_HXX)
