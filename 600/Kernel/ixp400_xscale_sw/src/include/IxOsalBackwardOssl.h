/** 
 * This file is intended to provide backward 
 * compatibility for main osService/OSSL 
 * APIs. 
 *
 * It shall be phased out gradually and users
 * are strongly recommended to use IX_OSAL API.
 *
 * @par
 * IXP400 SW Release Crypto version 2.0
 * 
 * -- Intel Copyright Notice --
 * 
 * @par
 * Copyright 2002-2005 Intel Corporation All Rights Reserved.
 * 
 * @par
 * The source code contained or described herein and all documents
 * related to the source code ("Material") are owned by Intel Corporation
 * or its suppliers or licensors.  Title to the Material remains with
 * Intel Corporation or its suppliers and licensors.
 * 
 * @par
 * The Material is protected by worldwide copyright and trade secret laws
 * and treaty provisions. No part of the Material may be used, copied,
 * reproduced, modified, published, uploaded, posted, transmitted,
 * distributed, or disclosed in any way except in accordance with the
 * applicable license agreement .
 * 
 * @par
 * No license under any patent, copyright, trade secret or other
 * intellectual property right is granted to or conferred upon you by
 * disclosure or delivery of the Materials, either expressly, by
 * implication, inducement, estoppel, except in accordance with the
 * applicable license agreement.
 * 
 * @par
 * Unless otherwise agreed by Intel in writing, you may not remove or
 * alter this notice or any other notice embedded in Materials by Intel
 * or Intel's suppliers or licensors in any way.
 * 
 * @par
 * For further details, please see the file README.TXT distributed with
 * this software.
 * 
 * @par
 * -- End Intel Copyright Notice --
 */

#ifndef IX_OSAL_BACKWARD_OSSL_H
#define IX_OSAL_BACKWARD_OSSL_H


typedef IxOsalThread ix_ossl_thread_t;

typedef IxOsalSemaphore ix_ossl_sem_t;

typedef IxOsalMutex ix_ossl_mutex_t;

typedef IxOsalTimeval ix_ossl_time_t;


/* Map sub-fields for ix_ossl_time_t */
#define tv_sec secs
#define tv_nec nsecs


typedef IX_STATUS ix_error;

typedef UINT32 ix_ossl_thread_priority;

typedef UINT32 ix_uint32;


#define IX_OSSL_ERROR_SUCCESS IX_SUCCESS

#define IX_ERROR_SUCCESS IX_SUCCESS


typedef enum
{
    IX_OSSL_SEM_UNAVAILABLE = 0,
    IX_OSSL_SEM_AVAILABLE
} ix_ossl_sem_state;


typedef enum
{
    IX_OSSL_MUTEX_UNLOCK = 0,
    IX_OSSL_MUTEX_LOCK
} ix_ossl_mutex_state;


typedef IxOsalVoidFnVoidPtr ix_ossl_thread_entry_point_t;


#define	IX_OSSL_THREAD_PRI_HIGH 	90
#define IX_OSSL_THREAD_PRI_MEDIUM 	160
#define IX_OSSL_THREAD_PRI_LOW 		240


#define IX_OSSL_WAIT_FOREVER    IX_OSAL_WAIT_FOREVER

#define IX_OSSL_WAIT_NONE       IX_OSAL_WAIT_NONE

#define BILLION  IX_OSAL_BILLION

#define IX_OSSL_TIME_EQ(a,b)  IX_OSAL_TIME_EQ(a,b)

#define IX_OSSL_TIME_GT(a,b)  IX_OSAL_TIME_GT(a,b)

#define IX_OSSL_TIME_LT(a,b)  IX_OSAL_TIME_LT(a,b)

#define IX_OSSL_TIME_ADD(a,b)  IX_OSAL_TIME_ADD(a,b)

#define IX_OSSL_TIME_SUB(a,b)  IX_OSAL_TIME_SUB(a,b)


/* a is tick, b is timeval */
#define IX_OSSL_TIME_CONVERT_TO_TICK(a,b)  \
		 (a) = IX_OSAL_TIMEVAL_TO_TICKS(b)




PUBLIC IX_STATUS
ixOsalOsIxp400BackwardOsslThreadCreate (IxOsalVoidFnVoidPtr entryPoint,
					void *arg, IxOsalThread * ptrThread);

#define ix_ossl_thread_create(entryPoint, arg, ptrTid) \
		ixOsalOsIxp400BackwardOsslThreadCreate(entryPoint, arg, ptrTid)


/* void ix_ossl_thread_exit(ix_error retError, void* retObj) */
#define ix_ossl_thread_exit(retError, retObj)   \
		ixOsalThreadExit()


PUBLIC IX_STATUS ixOsalOsIxp400BackwardOsslThreadKill (IxOsalThread tid);

/* ix_error ix_ossl_thread_kill(tid) */
#define ix_ossl_thread_kill(tid) \
		ixOsalOsIxp400BackwardOsslThreadKill(tid)


PUBLIC IX_STATUS
ixOsalOsIxp400BackwardOsslThreadSetPriority (IxOsalThread tid,
					     UINT32 priority);


/* 
 * ix_error ix_ossl_thread_set_priority(ix_ossl_thread_t tid,
 *                             ix_ossl_thread_priority priority
 *                             ); 
 */

#define ix_ossl_thread_set_priority(tid, priority) \
		ixOsalOsIxp400BackwardOsslThreadSetPriority(tid, priority)


PUBLIC IX_STATUS ixOsalOsIxp400BackwardOsslTickGet (int *pticks);

#define ix_ossl_tick_get(pticks) \
		ixOsalOsIxp400BackwardOsslTickGet(pticks)

PUBLIC IX_STATUS ixOsalOsIxp400BackwardOsslThreadDelay (int ticks);

#define ix_ossl_thread_delay(ticks) ixOsalOsIxp400BackwardOsslThreadDelay(ticks)



/* ix_error ix_ossl_sem_init(int start_value, ix_ossl_sem_t* sid); */
/* Note sid is a pointer to semaphore */
#define ix_ossl_sem_init(value, sid) \
		ixOsalSemaphoreInit(sid, value)


PUBLIC IX_STATUS
ixOsalOsIxp400BackwardOsslSemaphoreWait (IxOsalSemaphore semaphore,
					 INT32 timeout);


/*
ix_error ix_ossl_sem_take(
                          ix_ossl_sem_t sid,
                          ix_uint32 timeout
                         );
*/

#define ix_ossl_sem_take( sid, timeout) \
		ixOsalOsIxp400BackwardOsslSemaphoreWait(sid, timeout)



PUBLIC IX_STATUS
ixOsalOsIxp400BackwardOsslSemaphorePost (IxOsalSemaphore sid);

/*ix_error ix_ossl_sem_give(ix_ossl_sem_t sid); */
#define ix_ossl_sem_give(sid) \
		ixOsalOsIxp400BackwardOsslSemaphorePost(sid);


PUBLIC IX_STATUS ixOsalOsIxp400BackwardSemaphoreDestroy (IxOsalSemaphore sid);

#define ix_ossl_sem_fini(sid) \
		ixOsalOsIxp400BackwardSemaphoreDestroy(sid)


PUBLIC IX_STATUS
ixOsalOsIxp400BackwardOsslMutexInit (ix_ossl_mutex_state start_state,
				     IxOsalMutex * pMutex);


/* ix_error ix_ossl_mutex_init(ix_ossl_mutex_state start_state, ix_ossl_mutex_t* mid); */
#define ix_ossl_mutex_init(start_state, pMutex) \
		ixOsalOsIxp400BackwardOsslMutexInit(start_state, pMutex)


PUBLIC IX_STATUS
ixOsalOsIxp400BackwardOsslMutexLock (IxOsalMutex mid, INT32 timeout);

/*
ix_error ix_ossl_mutex_lock(
                            ix_ossl_mutex_t mid, 
                            ix_uint32 timeout
                           );
*/
#define ix_ossl_mutex_lock(mid, timeout) \
		ixOsalOsIxp400BackwardOsslMutexLock(mid, timout)


PUBLIC IX_STATUS ixOsalOsIxp400BackwardOsslMutexUnlock (IxOsalMutex mid);

/* ix_error ix_ossl_mutex_unlock(ix_ossl_mutex_t mid); */
#define ix_ossl_mutex_unlock(mid) \
		ixOsalOsIxp400BackwardOsslMutexUnlock(mid)

PUBLIC IX_STATUS ixOsalOsIxp400BackwardOsslMutexDestroy (IxOsalMutex mid);

#define ix_ossl_mutex_fini(mid) \
		ixOsalOsIxp400BackwardOsslMutexDestroy(mid);

#define ix_ossl_sleep(sleeptime_ms) \
		ixOsalSleep(sleeptime_ms)

PUBLIC IX_STATUS ixOsalOsIxp400BackwardOsslSleepTick (UINT32 ticks);

#define ix_ossl_sleep_tick(sleeptime_ticks) \
		ixOsalOsIxp400BackwardOsslSleepTick(sleeptime_ticks)


PUBLIC IX_STATUS ixOsalOsIxp400BackwardOsslTimeGet (IxOsalTimeval * pTv);

#define ix_ossl_time_get(pTv)    \
		ixOsalOsIxp400BackwardOsslTimeGet(pTv)


typedef UINT32 ix_ossl_size_t;

#define ix_ossl_malloc(arg_size) \
		ixOsalMemAlloc(arg_size)

#define ix_ossl_free(arg_pMemory) \
		ixOsalMemFree(arg_pMemory)


#define ix_ossl_memcpy(arg_pDest, arg_pSrc,arg_Count) \
		ixOsalMemCopy(arg_pDest, arg_pSrc,arg_Count)

#define ix_ossl_memset(arg_pDest, arg_pChar, arg_Count) \
		ixOsalMemSet(arg_pDest, arg_pChar, arg_Count)


#endif /* IX_OSAL_BACKWARD_OSSL_H */
