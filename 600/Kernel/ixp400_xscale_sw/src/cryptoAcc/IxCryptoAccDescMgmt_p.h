/**
 * @file IxCryptoAccDescMgmt_p.h
 *
 * @date October-03-2002
 *
 * @brief Private header file for Descriptors Management module
 *
 *
 * Design Notes:
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


#ifndef IxCryptoAccDescMgmt_p_H
#define IxCryptoAccDescMgmt_p_H


/*
 * Put the user defined include files required.
 */
#include "IxCryptoAcc_p.h"


/*
 * #defines for function return types, etc.
 */

/* Maximum numbers of queue descriptor created in the descriptor pool. 278 
 * descriptors are created in the pool. This number ONLY be changed to a 
 * bigger number and not smaller. This is because the queue depth is 64 
 * entries per queue, 4 queues are used for this access component and we  
 * need to have extra 16 descriptors for NPE as NPE may hold up to 16 
 * descriptor at the same time. Furthermore, 6 descriptors are reserved for 
 * XScale processing contexts
 */
#define MAX_DESCRIPTORS_NUM_IN_POOL   278

#define IX_CRYPTO_Q_DESC_ADDR_WORD_ALIGNED_SIZE   3   
                                                /**< Extra bytes needed in 
                                                 * memory allocation to ensure 
                                                 * the queue descriptor address
                                                 * is word aligned
                                                 */ 

/* Q Descriptor size for IxCryptoAcc software component */
#define IX_CRYPTO_Q_DESC_SIZE (((sizeof (IxCryptoQDescriptor) +       \
                                  (IX_OSAL_CACHE_LINE_SIZE - 1)) /  \
                                  IX_OSAL_CACHE_LINE_SIZE) *        \
                                  IX_OSAL_CACHE_LINE_SIZE )

/* Additional Authentication Data size */
#define IX_CRYPTO_Q_DESC_AAD_SIZE ((( (IX_CRYPTO_ACC_CCM_MAX_AAD_LEN ) +  \
                                  (IX_OSAL_CACHE_LINE_SIZE - 1)) /  \
                                  IX_OSAL_CACHE_LINE_SIZE)       *  \
                                  IX_OSAL_CACHE_LINE_SIZE )

/**
 * @fn ixCryptoDescMgmtInit
 *
 * @brief Initialize descriptor management module. Pool of descriptors 
 *        shared across NPE and IxCryptoAcc software component is allocated
 *        and initialized.
 *
 * @return IxCryptoAccStatus
 *             - IX_CRYPTO_ACC_STATUS_SUCCESS
 *             - IX_CRYPTO_ACC_STATUS_FAIL
 *
 */
IxCryptoAccStatus
ixCryptoDescMgmtInit (void);



/**
 * @fn ixCryptoDescMgmtQDescriptorGet
 *
 * @brief Get queue descriptor from the descriptor pool.
 *
 * @param IxCryptoQDescriptor** [out] pQDescriptor - Q descriptor pointer
 * 
 * @return IxCryptoAccStatus
 *             - IX_CRYPTO_ACC_STATUS_SUCCESS
 *             - IX_CRYPTO_ACC_STATUS_FAIL
 *
 */
IxCryptoAccStatus
ixCryptoDescMgmtQDescriptorGet (IxCryptoQDescriptor **pQDescriptor);



/**
 * @fn ixCryptoDescMgmtQDescriptorRelease
 *
 * @brief Release queue descriptor previously allocated back to the 
 *        descriptor pool
 *
 * @param IxCryptoQDescriptor* [in] pQDescriptor - Q descriptor pointer
 *
 * @return IxCryptoAccStatus
 *             - IX_CRYPTO_ACC_STATUS_SUCCESS
 *             - IX_CRYPTO_ACC_STATUS_FAIL
 *
 */
IxCryptoAccStatus
ixCryptoDescMgmtQDescriptorRelease (IxCryptoQDescriptor *pQDescriptor);



/**
 * @fn ixCryptoDescMgmtShow
 *
 * @brief To show the number of descriptors that have been used, and the pool
 *        usage.
 *
 * @return none
 *
 */
void
ixCryptoDescMgmtShow (void);



/**
 * @fn ixCryptoDescMgmtDescPoolFree
 *
 * @brief To free the memory allocated to descriptor pool through malloc 
 *        function. This function will only be called in 
 *        ixCryptoAccCryptoServiceStop function.
 *
 * @return none
 *
 */
void
ixCryptoDescMgmtDescPoolFree (void);



/**
 * @fn ixCryptoDescMgmtAllQDescriptorInPoolCheck
 *
 * @brief This inline function is used to check whether all the allocated 
 *        descriptors have been pushed back to the descriptors pool. If all 
 *        descriptors are unused, it indicates that there is no pending task 
 *        in the queues.
 *
 * @return IxCryptoAccStatus
 *             - IX_CRYPTO_ACC_STATUS_SUCCESS
 *             - IX_CRYPTO_ACC_STATUS_FAIL
 *
 * Remove INLINE keyword to resolve cannot inline warning (VxWorks). SCR1421
 *
 */
IxCryptoAccStatus
ixCryptoDescMgmtAllQDescriptorInPoolCheck (void);


#endif /* def IxCryptoAccDescMgmt_p_H */
