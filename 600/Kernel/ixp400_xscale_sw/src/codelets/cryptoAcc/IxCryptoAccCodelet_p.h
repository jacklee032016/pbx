/**
 * @file IxCryptoAccCodelet_p.h
 *
 * @date Jun-8-2003
 *
 * @brief This is private header file for the Crypto Access Component Codelet
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


#ifndef IxCryptoAccCodelet_p_H
#define IxCryptoAccCodelet_p_H

#include "IxOsal.h" 
#include "IxNpeDl.h"
#include "IxCryptoAcc.h"

/** Global variables */
extern BOOL forwardRegisterCBCalled;    /**< callback flag */
extern BOOL reverseRegisterCBCalled;    /**< callback flag */
extern IxCryptoAccRegisterCompleteCallback registerCB; /**< register callback 
                                                        * function
                                                        */
extern IxCryptoAccPerformCompleteCallback performCB;   /**< perform callback 
                                                        * function
                                                        */

#define IX_CRYPTOACC_CODELET_MAX_STR_SIZE    128       /**< Maximum size of info
                                                        * string. 
                                                        */


/**
 * @brief  Structure holding various configuration parameters for 
 *         demonstration purposes.
 * 
 * This structure is only defined and used by this codelet. It holds
 * various parameters that are required to register a context and invoke
 * "perform" requests on the cryptoAcc component.
 */ 
typedef struct {

    IxCryptoAccOperation  frwdOperation ; /**< Opcode for forward operation */
    IxCryptoAccOperation  revOperation ;  /**< Opcode for reverse operation */
    IxCryptoAccCipherAlgo cipherAlgo;     /**< Cipher Algorithm */
    IxCryptoAccCipherMode cipherMode;     /**< Cipher Mode */ 
    UINT32 cipherKeyLen;                  /**< Cipher Key Length */
    UINT32 cipherBlockLen;		  /**< Cipher Algorithm's Block Length */
    UINT32 cipherInitialVectorLen;        /**< Initial vector length */
    IxCryptoAccAuthAlgo  authAlgo;        /**< Authentication Algorithm */   
    UINT32 authKeyLen;                    /**< Authentication Key Length */
    UINT32 authDigestLen;                 /**< Authentication Digest Length */
    UINT32 aadLen;                        /**< Additional Authentication Data
                                           *  Length (used in CCM mode of
                                           *  operation).
                                           */
    BOOL useDifferentSrcAndDestMbufs;     /**< Use different source and
                                           *  destination buffers.
	                                   */
    BOOL invokeXScaleWepPerform;          /**< Perform this request using
                                           * XScaleWepPerform function
                                           * (note only ARC4 and WEP CRC
                                           * requests are valid .
                                           */
    UINT8 infoString[IX_CRYPTOACC_CODELET_MAX_STR_SIZE]; 
                                           /**< Information about the type of
                                            * request.
                                            */
}IxCryptoCodeletServiceParam;


/**************************************************************************
 *                           Function Prototype 
 **************************************************************************/

/**
 * @fn  ixCryptoAccCodeletInit (IxCryptoAccCfg ixCryptoAccCodeletCfg)
 * @brief This function initialises the neccessary components.
 *
 * @note The initialisation sequence is as follows:
 * <UL>
 *  <LI> Initialise Network Processor Engine(s) (NPE)
 *  <LI> Initialise Queue manager
 *  <LI> Start NPE(s)
 *  <LI> Initialise Crypto Access component
 *  <LI> Start Queue manager dispatcher. 
 * </UL>  
 *
 * @param "IxCryptoAccCfg ixCryptoAccCodeletCfg" - Specifies the interfaces
 *        to the engines that needs to be initialized.
 *
 * @return @li IX_SUCCESS - Successful initialization.
 *         @li IX_FAIL - Initialization failed.
 */
PRIVATE IX_STATUS ixCryptoAccCodeletInit (IxCryptoAccCfg ixCryptoAccCodeletCfg);

/**
 * @def IX_CRYPTO_ACC_CODELET_DRV_DMA_MALLOC
 *
 * @brief To allocate the memory
 *
 * @note Example of usage: IX_CRYPTO_ACC_CODELET_DRV_DMA_MALLOC(size)
 *
 * @param "UINT32 [in] size" - size of memory block to be freed.
 *
 * @return None
 *
 */
#define IX_CRYPTO_ACC_CODELET_DRV_DMA_MALLOC(size) \
            IX_OSAL_CACHE_DMA_MALLOC(size)

/**
 * @def IX_CRYPTO_ACC_CODELET_DRV_DMA_FREE
 *
 * @brief To free the memory allocated through IX_CRYPTO_ACC_DRV_DMA_MALLOC
 *        function.
 *
 * @note Example of usage: IX_CRYPTO_ACC_CODELET_DRV_DMA_FREE(pData, size)
 *
 * @param "UINT32 [in] *pData" - memory block pointer to be freed.
 *
 * @return None
 *
 */
#define IX_CRYPTO_ACC_CODELET_DRV_DMA_FREE(pData) \
            IX_OSAL_CACHE_DMA_FREE(pData)


/**
 * @fn ixCryptoAccCodeletBufIsQEmpty (void)
 *
 * @brief This inline function is used to check whether the buffer
 *        queue is empty.
 *
 * @param None
 *
 * @return BOOL
 *             - TRUE - the queue is full.
 *             - FALSE - the queue is not full.
 */
INLINE BOOL 
ixCryptoAccCodeletBufIsQEmpty (void);


/**
 * @fn ixCryptoAccCodeletBufIsQFull(void)
 *
 * @brief This inline function is used to check whether the buffer
 *        queue is full.
 *
 * @param None
 *
 * @return BOOL
 *             - TRUE - the queue is full.
 *             - FALSE - the queue is not full.
 */
INLINE BOOL 
ixCryptoAccCodeletBufIsQFull (void);


/**
 * @fn ixCryptoAccCodeletCryptoMbufToQTailAdd (IX_OSAL_MBUF *pMbufToAdd)
 *
 * @brief This function adds a mbuf to the tail of the queue.  
 *
 * @param "IX_OSAL_MBUF* [in] pMbufToAdd" - Pointer of the mbuf to be added  
 *                    to the tail of the queue.
 *
 * @return None.
 */
void ixCryptoAccCodeletCryptoMbufToQTailAdd (IX_OSAL_MBUF *pMbufToAdd);


/**
 * @fn ixCryptoAccCodeletCryptoMbufFromQHeadRemove(IX_OSAL_MBUF **pMbufToRem)
 *
 * @brief This function removes a mbuf from the head of the queue.
 *
 * @param "IX_OSAL_MBUF** [in] pMbufToRem" - Pointer to a mbuf pointer to be
 *                     removed from the head of the queue.
 *
 * @return None.
 */
void ixCryptoAccCodeletCryptoMbufFromQHeadRemove (IX_OSAL_MBUF **pMbufToRem);


/**
 * @fn ixCryptoAccCodeletCryptoMemPoolInit(UINT32 pktLen)
 *
 * @brief This function initialises mbuf pool for the Crypto
 *        Access Codelet.
 *
 * @param "UINT32 [in] pktLen" - packet length size for mbuf
 *
 * @return @li IX_SUCCESS - Successful mbuf pool initialisation.
 *         @li IX_FAIL - Mbuf pool initialisation failed.
 */
IX_STATUS ixCryptoAccCodeletCryptoMemPoolInit (UINT32 pktLen);


/**
 * @fn IX_STATUS ixCryptoAccCodeletCryptoMemPoolFree (UINT32 pktLen)
 *
 * @brief Free all resources in Crypto Access Codelet MBUF pool
 *
 * @param "UINT32 [in] pktLen" - packet length size for mbuf
 *
 * @return None
 */
void ixCryptoAccCodeletCryptoMemPoolFree (UINT32 pktLen);
 
 
/**
 * @fn ixCryptoAccCodeletNpeInit(IxNpeDlNpeId npeId, IxNpeDlBuildId buildId)
 *
 * @brief This function is to download the latest crypto NPE image into NPE.
 *
 * @param "IxNpeDlNpeId [in] npeId" - The ID of the NPE which the microcode 
 *                       downloaded to.
 * @param "IxNpeDlBuildId [in] buildId" - The build ID for image which indicate 
 *                       functionality
 *
 * @return @li IX_SUCCESS - Successful NPE initialisation.
 *         @li IX_FAIL - NPE initialisation failed.
 */
IX_STATUS ixCryptoAccCodeletNpeInit (
                        IxNpeDlNpeId npeId,
                        IxNpeDlBuildId buildId);


/**
 * @fn ixCryptoAccCodeletDispatcherPoll(void)
 *
 * @brief This function polls the queues when poll mode set in the 
 *        function ixCryptoAccCodeletDispatcherStart.
 *
 * @param None
 * 
 * @return None
 */
void ixCryptoAccCodeletDispatcherPoll (void);


/**
 * @fn ixCryptoAccCodeletDispatcherStart(BOOL useInterrupt)
 *
 * @brief  This function starts the Queue manager dispatcher.
 *
 * @param "BOOL [in] useInterrupt" - start in interrupt or poll mode. TRUE is 
 *                                   interrupt mode while FALSE is poll mode.
 * 
 * @return  @li IX_SUCCESS - Dispatcher successfully started.
 *          @li IX_FAIL - Error starting dispatcher.
 */
IX_STATUS ixCryptoAccCodeletDispatcherStart (BOOL useInterrupt);


/**
 * @fn ixCryptoAccCodeletRegisterCtx(
 *         INT32 srvIndex,
 *         IxCryptoAccCtx *pForwardCtx,
 *         IxCryptoAccCtx *pReverseCtx,
 *         UINT32 *pForwardCtxId,
 *         UINT32 *pReverseCtxId)
 *
 * @brief  This function registers forward and reverse context for the 
 *         specified service index.
 *
 * @param "INT32 [in] srvIndex" - Index into the array defining various 
 *                                configured services.
 * @param "IxCryptoAccCtx* [in] pForwardCtx" - pointer to Crypto Context for
 *                                            forward direction, e.g encryption,
 *                                            auth_calc.
 * @param "IxCryptoAccCtx* [in] pReverseCtx" - pointer to Crypto Context for 
 *                                            reverse direction, e.g decryption,
 *                                            auth_check.
 * @param "UINT32* [in] pForwardCtxId" - pointer to Crypto Context ID
 * @param "UINT32* [in] pReverseCtxId" - pointer to Crypto Context ID
 * 
 * @return @li IX_SUCCESS - Register successfully
 *         @li IX_FAIL - Error in registering
 */
IX_STATUS ixCryptoAccCodeletRegisterCtx (
            INT32  srvIndex,
            IxCryptoAccCtx *pForwardCtx,
            IxCryptoAccCtx *pReverseCtx,
            UINT32 *pForwardCtxId,
            UINT32 *pReverseCtxId);


/**
 * @fn ixCryptoAccCodeletPeform(UINT32 pktLen)
 *
 *
 * @brief This function invokes the "appropriate" perform function
 *        of the cryptoAcc component. XScaleWepPerform function is invoked
 *        if the invokeXScaleWepPerform flag is set for the service index.
 *        This function uses the global variable "codeletSrvIndex", 
 *        set by the codeletMain  to access the requested service's parameters.
 *
 * @param  "UINT32 pktLen  [in] pktLen"   - Packet length.
 * @return @li IX_SUCCESS - If the operation invocation was successful. 
 *         @li IX_FAIL    - If operation failed for some reason.
 */
IX_STATUS ixCryptoAccCodeletPerform (UINT32 pktLen);



/**
 * @fn ixCryptoAccCodeletUnregisterCtx (void)
 *
 * @brief  This function unregisters all the contexts registered.
 *
 * @param None
 * 
 * @return None
 */
void ixCryptoAccCodeletUnregisterCtx (void);


/**
 * @fn ixCryptoAccCodeletXScaleWepPerform ( IX_OSAL_MBUF *pMbuf ,UINT32 pktLen)
 * @brief This codelet function calls ixCryptoAccXScaleWepPerform function
 *        to perform ARC4 or WEP_CRC processing on XScale.
 * @param  IX_OSAL_MBUF [in] *pMbuf : pointer to the mbuf 
 * 
 * @param  UINT32 [in] pktLen: length of data to be worked on.
 * @return  @li IX_SUCCESS - All invocations of ixCryptoAccXScaleWepPerform
 *                           were succesful.
 *          @li IX_FAIL - There was some kind of error while processing the
 *                        request. This should not happen, if the codelet
 *                        has been configured correctly.
 *                        
 */
PRIVATE IX_STATUS ixCryptoAccCodeletXScaleWepPerform( 
                    IX_OSAL_MBUF *pMbuf,
                    UINT32 pktLen);

/**
 * @fn ixCryptoAccCodeletDispatcherStop (BOOL useInterrupt)
 *
 * @brief Stop QMgr dispatcher thread if QMgr dispatcher runs in poll mode 
 *        or unbind QMgr dispatcher from interrupt if it runs in interrupt mode.
 *
 * @param "BOOL [in] useInterrupt" - start in interrupt or poll mode. TRUE is 
 *                                   interrupt mode while FALSE is poll mode.
 * 
 * @return  @li IX_SUCCESS - Dispatcher successfully stopped.
 *          @li IX_FAIL - Error stopping dispatcher.
 */
IX_STATUS ixCryptoAccCodeletDispatcherStop (BOOL useInterrupt);


/**
 * @fn ixCryptoAccCodeletUsageShow (void)
 *
 * @brief Display user guide for Linux platform or VxWorks platform
 *
 * @param None
 *
 * @return None
 */
void ixCryptoAccCodeletUsageShow (void);

#endif /* IxCryptoAccCodelet_p_H */


