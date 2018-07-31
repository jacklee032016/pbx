#if !defined(ASSIST_DEPRECATED_READLOCKHELPER_HXX)
#define ASSIST_DEPRECATED_READLOCKHELPER_HXX
/*
* $Id: ReadLockHelper.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#if defined(ASSIST_USE_DEPRECATED)


static const char* const deprecated_ReadLockHelper_hxx_Version = 
    "$Id: ReadLockHelper.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $";


    #if defined(ASSIST_WARN_DEPRECATED)

        #warning \
            ReadLockHelper has been deprecated. \
            Use Assist::Threads::ReadLock.

    #endif // defined(ASSIST_WARN_DEPRECATED)


    #include "Lock.hxx"

    typedef Assist::Threads::ReadLock    ReadLockHelper;


#else

    #error \
        ReadLockHelper no longer supported. \
        Use Assist::Threads::ReadLock.

#endif // defined(ASSIST_USE_DEPRECATED)


#endif // !defined(ASSIST_DEPRECATED_READLOCKHELPER_HXX)
