#if !defined(ASSIST_DEPRECATED_VCONDITION_H)
#define ASSIST_DEPRECATED_VCONDITION_H
/*
* $Id: VCondition.h,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#if defined(ASSIST_USE_DEPRECATED)


static const char* const deprecated_VCondition_h_Version = 
    "$Id: VCondition.h,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $";


    #if defined(ASSIST_WARN_DEPRECATED)

        #warning \
            VCondition has been deprecated. \
            Use Assist::Threads::Condition.

    #endif // defined(ASSIST_WARN_DEPRECATED)


    #include "Condition.hxx"

    typedef Assist::Threads::Condition    VCondition;


#else

    #error \
        VCondition no longer supported. \
        Use Assist::Threads::Condition.

#endif // defined(ASSIST_USE_DEPRECATED)


#endif // !defined(ASSIST_DEPRECATED_VCONDITION_H)
