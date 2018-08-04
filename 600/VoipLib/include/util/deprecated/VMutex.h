#if !defined(VOCAL_DEPRECATED_VMUTEX_H)
#define VOCAL_DEPRECATED_VMUTEX_H
/*
 * $Id: VMutex.h,v 1.2 2007/03/01 20:04:30 lizhijie Exp $
 */

#if defined(VOCAL_USE_DEPRECATED)

    #if defined(VOCAL_WARN_DEPRECATED)

        #warning \
            VMutex has been deprecated. \
            Use Vocal::Threads::Mutex.

    #endif // defined(VOCAL_WARN_DEPRECATED)


    #include "Mutex.hxx"

    typedef Vocal::Threads::Mutex    VMutex;


#else

    #error \
        VMutex no longer supported. \
        Use Vocal::Threads::Mutex.

#endif // defined(VOCAL_USE_DEPRECATED)


#endif // !defined(VOCAL_DEPRECATED_VMUTEX_H)
