#if !defined(ASSIST_DEPRECATED_VRWLOCK_HXX)
#define ASSIST_DEPRECATED_VRWLOCK_HXX
/*
* $Id: VRWLock.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#if defined(ASSIST_USE_DEPRECATED)


static const char* const deprecated_VRWLock_hxx_Version = 
    "$Id: VRWLock.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $";


    #if defined(ASSIST_WARN_DEPRECATED)

        #warning \
            VRWLock has been deprecated. \
            Use Assist::Threads::RWMutex.

    #endif // defined(ASSIST_WARN_DEPRECATED)


    #include "RWMutex.hxx"

    class VRWLock : public Assist::Threads::RWMutex
    {
        public:
            VRWLock() : RWMutex() {}
            virtual ~VRWLock() {}
            void ReadLock() { readlock(); }
            void WriteLock() { writelock(); }
            void Unlock() { unlock(); }
    };


#else

    #error \
        LockHelper no longer supported. \
        Use Assist::Threads::RWMutex.

#endif // defined(ASSIST_USE_DEPRECATED)


#endif // !defined(ASSIST_DEPRECATED_VRWLOCK_HXX)
