#if !defined(VOCAL_DEPRECATED_VCONDITION_H)
#define VOCAL_DEPRECATED_VCONDITION_H

/*
 * $Id: VCondition.h,v 1.2 2007/03/01 20:04:30 lizhijie Exp $
 */


#if defined(VOCAL_USE_DEPRECATED)


    #if defined(VOCAL_WARN_DEPRECATED)

        #warning \
            VCondition has been deprecated. \
            Use Vocal::Threads::Condition.

    #endif // defined(VOCAL_WARN_DEPRECATED)


    #include "Condition.hxx"

    typedef Vocal::Threads::Condition    VCondition;


#else

    #error \
        VCondition no longer supported. \
        Use Vocal::Threads::Condition.

#endif // defined(VOCAL_USE_DEPRECATED)


#endif // !defined(VOCAL_DEPRECATED_VCONDITION_H)
