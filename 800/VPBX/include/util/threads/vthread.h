#if !defined(ASSIST_VTHREAD_H)
#define ASSIST_VTHREAD_H
/*
* $Id: vthread.h,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#if 0
#if defined(_WIN32)

	#if 0
    // this is not valid until someone finishes forming a native thread port
    // for win32.  right now, we are still using posix threads on win32
    #if !defined(ASSIST_USE_WIN32_THREADS)
    #define ASSIST_USE_WIN32_THREADS
    #endif
	#endif

    // we can replace this with win32 threads
    #if !defined(ASSIST_USE_POSIX_THREADS)
    #define ASSIST_USE_POSIX_THREADS
    #endif

#elif defined(__vxworks)

    #if !defined(ASSIST_USE_VXWORKS_THREADS)
    #define ASSIST_USE_VXWORKS_THREADS
    #endif

#else 

    #if !defined(ASSIST_USE_POSIX_THREADS)
    #define ASSIST_USE_POSIX_THREADS
    #endif

#endif


#if defined(ASSIST_USE_WIN32_THREADS)

    #include "win32/vthread.h"

#elif defined(ASSIST_USE_VXWORKS_THREADS)

    #include "vxworks/vthread.h"
    
#elif defined(ASSIST_USE_UNIX_THREADS)

    #include "unix/vthread.h"
    
#elif defined(ASSIST_USE_POSIX_THREADS)

    #include "posix/vthread.h"
    
#else

    #error Threading unknown

#endif
#endif

    #include "posix/vthread.h"

#endif // !defined(ASSIST_VTHREAD_H)
