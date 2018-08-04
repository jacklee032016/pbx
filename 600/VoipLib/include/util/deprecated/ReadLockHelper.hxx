#if !defined(VOCAL_DEPRECATED_READLOCKHELPER_HXX)
#define VOCAL_DEPRECATED_READLOCKHELPER_HXX

/*
 * $Id: ReadLockHelper.hxx,v 1.2 2007/03/01 20:04:30 lizhijie Exp $
 */


#if defined(VOCAL_USE_DEPRECATED)


    #if defined(VOCAL_WARN_DEPRECATED)

        #warning \
            ReadLockHelper has been deprecated. \
            Use Vocal::Threads::ReadLock.

    #endif // defined(VOCAL_WARN_DEPRECATED)


    #include "Lock.hxx"

    typedef Vocal::Threads::ReadLock    ReadLockHelper;


#else

    #error \
        ReadLockHelper no longer supported. \
        Use Vocal::Threads::ReadLock.

#endif // defined(VOCAL_USE_DEPRECATED)


#endif // !defined(VOCAL_DEPRECATED_READLOCKHELPER_HXX)
