#if !defined(ASSIST_DEPRECATED_VMUTEX_H)
#define ASSIST_DEPRECATED_VMUTEX_H
/*
* $Id: VMutex.h,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#if defined(ASSIST_USE_DEPRECATED)

    #if defined(ASSIST_WARN_DEPRECATED)

        #warning \
            VMutex has been deprecated. \
            Use Assist::Threads::Mutex.

    #endif // defined(ASSIST_WARN_DEPRECATED)


    #include "Mutex.hxx"

    typedef Assist::Threads::Mutex    VMutex;


#else

    #error \
        VMutex no longer supported. \
        Use Assist::Threads::Mutex.

#endif // defined(ASSIST_USE_DEPRECATED)


#endif // !defined(ASSIST_DEPRECATED_VMUTEX_H)
