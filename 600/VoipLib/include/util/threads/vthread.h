#if !defined(VOCAL_VTHREAD_H)
#define VOCAL_VTHREAD_H
/*
 * $Id: vthread.h,v 1.2 2007/03/01 20:04:30 lizhijie Exp $
 */

#if defined(_WIN32)

#if 0
    // this is not valid until someone finishes forming a native thread port
    // for win32.  right now, we are still using posix threads on win32
    #if !defined(VOCAL_USE_WIN32_THREADS)
    #define VOCAL_USE_WIN32_THREADS
    #endif
#endif
    // we can replace this with win32 threads
    #if !defined(VOCAL_USE_POSIX_THREADS)
    #define VOCAL_USE_POSIX_THREADS
    #endif

#elif defined(__vxworks)

    #if !defined(VOCAL_USE_VXWORKS_THREADS)
    #define VOCAL_USE_VXWORKS_THREADS
    #endif

#else 

    #if !defined(VOCAL_USE_POSIX_THREADS)
    #define VOCAL_USE_POSIX_THREADS
    #endif

#endif


#if defined(VOCAL_USE_WIN32_THREADS)

    #include "win32/vthread.h"

#elif defined(VOCAL_USE_VXWORKS_THREADS)

    #include "vxworks/vthread.h"
    
#elif defined(VOCAL_USE_UNIX_THREADS)

    #include "unix/vthread.h"
    
#elif defined(VOCAL_USE_POSIX_THREADS)

    #include "posix/vthread.h"
    
#else

    #error Threading unknown

#endif


#endif // !defined(VOCAL_VTHREAD_H)
