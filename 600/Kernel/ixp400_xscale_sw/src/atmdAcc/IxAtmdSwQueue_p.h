/**
 * @file IxAtmdSwQueue_p.h
 *
 * @author Intel Corporation
 * @date 17 March 2002
 *
 * @brief IxAtmdAcc Sw Queue
 *
 * The macros in this module implements a sw ring buffer
 * for time crititical applications.
 *
 * The implementation creates a queue of pointers
 * (to any specified type).
 *
 * The queue provides three indices tail, mid, and head.
 * One, two, or all three indices may be used to realise
 * for example, a stack, a simple FIFO queue, or any queue application
 * where entries must be retained for acknowledgment.
 *
 * Design Notes:
 * The implementation of this module focuses on performance.
 *
 * The queue size must be a power of 2 to allow the
 * wraping of the queue pointers to be implemented as an
 * efficient masking operation.
 *
 * The desired queue size is an initialisation parameter.
 * If the requested queue size is not a power of 2, then
 * the actual queue size will be rounded up.
 *
 * User code that needs to reference the queue size should
 * take care to query the queue to obtain the actual rounded
 * up queue size, and not to use the requested queue size.
 *
 * Queue members are volatile because they can be accessed by
 * many threads at the same time. Volatile prevents the compiler
 * to optimize and kkep a copy of the memory location into
 * a register. This way, information is always retrievd from
 * memory.when it is needed.
 *
 * Queue counters are always incremented. When accessing a queue entry
 * a mask is applied to find the correct indexed element (the queue
 * size is a power of 2 and
 *
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

#ifndef IX_ATMDACC_SW_QUEUE_H
#define IX_ATMDACC_SW_QUEUE_H

#include "IxOsal.h"

/**
* @def IX_ATMDACC_SWQ_TYPE
* @brief Define the type of a software queue
*/
#define IX_ATMDACC_SWQ_TYPE(TYPE) \
typedef struct \
{ \
    TYPE **content; \
    unsigned int size; \
    unsigned int mask; \
    volatile unsigned int tail; \
    volatile unsigned int head; \
    volatile unsigned int mid; \
    unsigned int dynamicAllocation; \
} \
IxAtmdAccSwQueueOfType_ ## TYPE

/**
* @def IX_ATMDACC_SWQ_INIT
* @brief Initialise a Queue of a specified type and size
*
* @note  - The requested size will be adjusted to the nearest larger
*        size that is a power of 2.
*
* @note  - User code that needs to reference the queue size should
*        take care to query the queue to obtain the actual rounded
*        up queue size, and not to use the requested queue size.
*
* @note  - After initialisation use IX_ATMDACC_SWQ_SIZE to obtain
*        the actual size.
*
* @note  - Memory for the queue is allocated and initialised.
*        After initialisation use IX_ATMDACC_SWQ_INITIALISED to check
*        that the queue was initialised correctly.
*
* @note  - Input parameters are:-
*
*        @li queue - a pointer to a queue
*        @li TYPE  - the specific queue entry type
*        @li sz    - the requested queue size
*/
#define IX_ATMDACC_SWQ_INIT(queue,TYPE,requestedSize) do { \
            (queue)->size     = ixAtmdAccUtilQueueSizeConvert(requestedSize); \
            (queue)->mask     = (queue)->size - 1; \
            (queue)->head     = 0; \
            (queue)->tail     = 0; \
            (queue)->mid      = 0; \
            (queue)->dynamicAllocation = 1; \
            (queue)->content = (TYPE **) malloc ((queue)->size * sizeof (TYPE *)); \
            if((queue)->content != 0) \
            { \
                ixOsalMemSet ((queue)->content, 0, (queue)->size * sizeof (TYPE *)); \
            } \
        } while(0)

/**
* @def IX_ATMDACC_SWQ_STATIC_INIT
* @brief Initialise a Queue of a specified type and size, using a preallocated
*        buffer
*
* @note  - The requested size will be adjusted to the nearest larger
*        size that is a power of 2.
*
* @note  - User code that needs to reference the queue size should
*        take care to query the queue to obtain the actual rounded
*        up queue size, and not to use the requested queue size.
*
* @note  - After initialisation use IX_ATMDACC_SWQ_SIZE to obtain
*        the actual size.
*
* @note  - Memory for the queue is allocated from a buffer passed as parameter
*        After initialisation use IX_ATMDACC_SWQ_INITIALISED to check
*        that the queue was initialised correctly.
*
* @note  - Input parameters are:-
*
*        @li queue - a pointer to a queue
*        @li TYPE  - the specific queue entry type
*        @li sz    - the requested queue size
*/
#define IX_ATMDACC_SWQ_STATIC_INIT(queue,TYPE,requestedSize,queueBuffer) do { \
            (queue)->size     = ixAtmdAccUtilQueueSizeConvert(requestedSize); \
            (queue)->mask     = (queue)->size - 1; \
            (queue)->head     = 0; \
            (queue)->tail     = 0; \
            (queue)->mid      = 0; \
            (queue)->dynamicAllocation = 0; \
            (queue)->content = (TYPE **)queueBuffer; \
            ixOsalMemSet ((queue)->content, 0, (queue)->size * sizeof (TYPE *)); \
        } while(0)

/**
* @def IX_ATMDACC_SWQ_INITIALISED
* @brief Check that the Initialisation of a Queue was successfull
*/
#define IX_ATMDACC_SWQ_INITIALISED(queue) \
    ((queue)->content != 0)

/**
* @def IX_ATMDACC_SWQ_RELEASE_RESOURCES
* @brief Release any memory allocated to a Queue
*/
#define IX_ATMDACC_SWQ_RELEASE_RESOURCES(queue) do { \
    if ((queue)->dynamicAllocation) ixOsalMemFree((queue)->content); \
    (queue)->dynamicAllocation = 0; \
    (queue)->content = 0; \
    } while(0)

/**
* @def IX_ATMDACC_SWQ_RESET
* @brief Reset the queue to initail empty values
*/
#define IX_ATMDACC_SWQ_RESET(queue) do { \
            (queue)->head = 0; \
            (queue)->tail = 0; \
            (queue)->mid = 0; \
        } while(0)

/**
* @def IX_ATMDACC_SWQ_ENTRY_IDXSET
* @brief Set an entry ditrectly at the specified index in a queue
*/
#define IX_ATMDACC_SWQ_ENTRY_IDXSET(queue,index,entry) \
    (queue)->content[(index)] = (entry)

/**
* @def IX_ATMDACC_SWQ_ENTRY_IDXGET
* @brief return an entry directly from the specified index in a queue
*/
#define IX_ATMDACC_SWQ_ENTRY_IDXGET(queue,index) \
    ((queue)->content[(index)])

/**
* @def IX_ATMDACC_SWQ_SIZE
* @brief return the size of a queue
*/
#define IX_ATMDACC_SWQ_SIZE(queue) \
    ((queue)->size)

/**
* @def IX_ATMDACC_SWQ_TAIL
* @brief Get the Tail pointer of a Queue
*/
#define IX_ATMDACC_SWQ_TAIL(queue) \
        ((queue)->tail & (queue)->mask)

/**
* @def IX_ATMDACC_SWQ_TAIL_INDEX
* @brief Get the index element after the TAIL element of a Queue
*/
#define IX_ATMDACC_SWQ_TAIL_INDEX(queue, index) \
        (((queue)->tail + (index)) & (queue)->mask)

/**
* @def IX_ATMDACC_SWQ_TAIL_INCR
* @brief Increment the Tail pointer of a Queue
*/
#define IX_ATMDACC_SWQ_TAIL_INCR(queue) \
        (queue)->tail++

/**
* @def IX_ATMDACC_SWQ_TAIL_DECR
* @brief Decrement the Tail pointer of a Queue
*/
#define IX_ATMDACC_SWQ_TAIL_DECR(queue) \
        (queue)->tail--

/**
* @def IX_ATMDACC_SWQ_HEAD
* @brief Get the Head pointer of a Queue
*/
#define IX_ATMDACC_SWQ_HEAD(queue) \
        ((queue)->head & (queue)->mask)

/**
* @def IX_ATMDACC_SWQ_HEAD_INCR
* @brief Increment the Head pointer of a Queue
*/
#define IX_ATMDACC_SWQ_HEAD_INCR(queue) \
        (queue)->head++

/**
* @def IX_ATMDACC_SWQ_HEAD_ADVANCE
* @brief Advance the Head pointer of a Queue
*/
#define IX_ATMDACC_SWQ_HEAD_ADVANCE(queue,numberOfEntries) \
        (queue)->head += (numberOfEntries)

/**
* @def IX_ATMDACC_SWQ_MID
* @brief Get the mid pointer of a queue
*/
#define IX_ATMDACC_SWQ_MID(queue) \
        ((queue)->mid & (queue)->mask)

/**
* @def IX_ATMDACC_SWQ_MID_INCR
* @brief Increment the mid pointer of a queue
*/
#define IX_ATMDACC_SWQ_MID_INCR(queue) \
        (queue)->mid++

/**
* @def IX_ATMDACC_SWQ_MID_DECR
* @brief Decrement the mid pointer of a queue
*/
#define IX_ATMDACC_SWQ_MID_DECR(queue) \
        (queue)->mid--

/**
* @def IX_ATMDACC_SWQ_TAIL_ENTRY_GET
* @brief Get the entry at the tail of a Queue
*/
#define IX_ATMDACC_SWQ_TAIL_ENTRY_GET(queue) \
        (queue)->content[IX_ATMDACC_SWQ_TAIL(queue)]

/**
* @def IX_ATMDACC_SWQ_TAIL_ENTRY_GET_INDEX
* @brief Get the entry n elements after the tail of a Queue
*        (0 is the current tail element)
*/
#define IX_ATMDACC_SWQ_TAIL_ENTRY_GET_INDEX(queue, index) \
        (queue)->content[IX_ATMDACC_SWQ_TAIL_INDEX((queue), (index))]

/**
* @def IX_ATMDACC_SWQ_HEAD_ENTRY_GET
* @brief Get the entry at the head of a Queue
*/
#define IX_ATMDACC_SWQ_HEAD_ENTRY_GET(queue) \
        (queue)->content[IX_ATMDACC_SWQ_HEAD(queue)]

/**
* @def IX_ATMDACC_SWQ_MID_ENTRY_GET
* @brief Get the entry at the mid of a Queue
*/
#define IX_ATMDACC_SWQ_MID_ENTRY_GET(queue) \
        (queue)->content[IX_ATMDACC_SWQ_MID(queue)]

/**
* @brief Convert the requested queue size to a sw queue size
*/
unsigned int
ixAtmdAccUtilQueueSizeConvert (unsigned int requestedQueueSize);

/**
* @brief Convert a number to the nearest larger number that is a power of 2
*/
unsigned int
ixAtmdAccUtilNearestPowerOfTwoGet (unsigned int val);

#endif /* IX_ATMDACC_SW_QUEUE_H */


