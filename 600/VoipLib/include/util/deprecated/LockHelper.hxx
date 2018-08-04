#if !defined(VOCAL_DEPRECATED_LOCKHELPER_HXX)
#define VOCAL_DEPRECATED_LOCKHELPER_HXX
/*
 * $Id: LockHelper.hxx,v 1.2 2007/03/01 20:04:30 lizhijie Exp $
 */


#if defined(VOCAL_USE_DEPRECATED)


    #if defined(VOCAL_WARN_DEPRECATED)

        #warning \
            LockHelper has been deprecated. \
            Use Vocal::Threads::Lock.

    #endif // defined(VOCAL_WARN_DEPRECATED)


    #include "Lock.hxx"

    typedef Vocal::Threads::Lock    LockHelper;


#else

    #error \
        LockHelper no longer supported. \
        Use Vocal::Threads::Lock.

#endif // defined(VOCAL_USE_DEPRECATED)


#endif // !defined(VOCAL_DEPRECATED_LOCKHELPER_HXX)
