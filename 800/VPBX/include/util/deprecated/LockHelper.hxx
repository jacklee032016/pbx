#if !defined(ASSIST_DEPRECATED_LOCKHELPER_HXX)
#define ASSIST_DEPRECATED_LOCKHELPER_HXX
/*
* $Id: LockHelper.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#if defined(ASSIST_USE_DEPRECATED)

    #if defined(ASSIST_WARN_DEPRECATED)

        #warning \
            LockHelper has been deprecated. \
            Use Assist::Threads::Lock.

    #endif // defined(ASSIST_WARN_DEPRECATED)


    #include "Lock.hxx"

    typedef Assist::Threads::Lock    LockHelper;


#else

    #error \
        LockHelper no longer supported. \
        Use Assist::Threads::Lock.

#endif // defined(ASSIST_USE_DEPRECATED)


#endif // !defined(ASSIST_DEPRECATED_LOCKHELPER_HXX)
