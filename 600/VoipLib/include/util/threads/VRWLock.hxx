#if !defined(VOCAL_DEPRECATED_VRWLOCK_HXX)
#define VOCAL_DEPRECATED_VRWLOCK_HXX
/*
 * $Id: VRWLock.hxx,v 1.2 2007/03/01 20:04:30 lizhijie Exp $
 */


#if defined(VOCAL_USE_DEPRECATED)


    #if defined(VOCAL_WARN_DEPRECATED)

        #warning \
            VRWLock has been deprecated. \
            Use Vocal::Threads::RWMutex.

    #endif // defined(VOCAL_WARN_DEPRECATED)


    #include "RWMutex.hxx"

    class VRWLock : public Vocal::Threads::RWMutex
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
        Use Vocal::Threads::RWMutex.

#endif // defined(VOCAL_USE_DEPRECATED)


#endif // !defined(VOCAL_DEPRECATED_VRWLOCK_HXX)
