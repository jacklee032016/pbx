#if !defined(ASSIST_DEPRECATED_WRITELOCKHELPER_HXX)
#define ASSIST_DEPRECATED_WRITELOCKHELPER_HXX
/*
* $Id: WriteLockHelper.hxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#if defined(ASSIST_USE_DEPRECATED)


static const char* const deprecated_WriteLockHelper_hxx_Version = 
    "$Id: WriteLockHelper.hxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $";


    #if defined(ASSIST_WARN_DEPRECATED)

        #warning \
            WriteLockHelper has been deprecated. \
            Use Assist::Threads::WriteLock.

    #endif // defined(ASSIST_WARN_DEPRECATED)


    #include "Lock.hxx"

    typedef Assist::Threads::WriteLock    WriteLockHelper;


#else

    #error \
        WriteLockHelper no longer supported. \
        Use Assist::Threads::WriteLock.

#endif // defined(ASSIST_USE_DEPRECATED)


#endif // !defined(ASSIST_DEPRECATED_WRITELOCKHELPER_H)
