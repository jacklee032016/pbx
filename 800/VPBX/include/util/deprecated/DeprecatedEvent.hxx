#if !defined(ASSIST_DEPRECATED_EVENT_HXX)
#define ASSIST_DEPRECATED_EVENT_HXX
/*
* $Id: DeprecatedEvent.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#if defined(ASSIST_USE_DEPRECATED)

    #if defined(ASSIST_WARN_DEPRECATED)

        #warning \
            Assist::Signaling::Event has been deprecated. \
            Use Assist::Services::Event.

    #endif // defined(ASSIST_WARN_DEPRECATED)

    namespace Assist {
    namespace Signaling {

        typedef Assist::Services::Event   Event;

    } // namespace Process
    } // namespace Assist

#else

    #error \
        Assist::Signaling::Event no longer supported. \
        Use Assist::Services::Event.

#endif // defined(ASSIST_USE_DEPRECATED)


#endif // !defined(ASSIST_DEPRECATED_LOCK_HXX)
