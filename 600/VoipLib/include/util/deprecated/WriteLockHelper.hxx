#if !defined(VOCAL_DEPRECATED_WRITELOCKHELPER_HXX)
#define VOCAL_DEPRECATED_WRITELOCKHELPER_HXX

/*
 * $Id: WriteLockHelper.hxx,v 1.2 2007/03/01 20:04:30 lizhijie Exp $
 */


#if defined(VOCAL_USE_DEPRECATED)


    #if defined(VOCAL_WARN_DEPRECATED)

        #warning \
            WriteLockHelper has been deprecated. \
            Use Vocal::Threads::WriteLock.

    #endif // defined(VOCAL_WARN_DEPRECATED)


    #include "Lock.hxx"

    typedef Vocal::Threads::WriteLock    WriteLockHelper;


#else

    #error \
        WriteLockHelper no longer supported. \
        Use Vocal::Threads::WriteLock.

#endif // defined(VOCAL_USE_DEPRECATED)


#endif // !defined(VOCAL_DEPRECATED_WRITELOCKHELPER_H)
