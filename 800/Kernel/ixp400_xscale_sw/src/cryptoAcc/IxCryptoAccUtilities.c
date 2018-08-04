/**
 * @file IxCryptoAccUtilities.c
 *
 * @date October-03-2002
 *
 * @brief Source file for the Utilities module.
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


/*
 * Put the user defined include files required.
 */
#include "IxCryptoAccUtilities_p.h"


/**
 * @fn ixCryptoUtilMbufOffsetToAddressConvert
 * @brief To convert offset to data block of mbuf into 32-bit address.
 *
 */
UINT32 
ixCryptoUtilMbufOffsetToAddressConvert (
    IX_OSAL_MBUF *pMbuf,
    INT32 offset,
    BOOL useDiffBuf)
{
    INT32  AccumPktLen;
    UINT32 offsetDiff;
    IX_OSAL_MBUF *pBuffer = pMbuf;
    
    /* Check if in-place or non in-place operation */    
    if (!useDiffBuf)
    {
        /* Get accumulated data length in chained mbuf */
        AccumPktLen = IX_OSAL_MBUF_MLEN (pBuffer);
        
        /* While offset > AccumPktLen, checking for mbuf header fields and 
         * length are done in IxCryptoAcc.c and IxCryptoAccPerform.c
         */
        while (offset >= AccumPktLen)
        {
            /* Traverse to next mbuf buffer */
            pBuffer = IX_OSAL_MBUF_NEXT_BUFFER_IN_PKT_PTR (pBuffer);
            
            /* Get accumulated data length in chained mbuf */
            AccumPktLen += IX_OSAL_MBUF_MLEN (pBuffer);
        }
        
        /* Get the difference between the mbuf data offset with the accumulated
         * packet length, in order to move the pointer to the place pointed
         * by offset
         */
        offsetDiff = IX_OSAL_MBUF_MLEN (pBuffer) + (offset - AccumPktLen);
        
        /* Return address of data pointer in mbuf */
        return (((UINT32) IX_OSAL_MBUF_MDATA (pBuffer)) + offsetDiff);
    }
    else /* non in-place operation */
    {
        return (((UINT32) IX_OSAL_MBUF_MDATA (pBuffer)) + offset);
    }
} /* end of ixCryptoUtilMbufOffsetToAddressConvert () function */                                        



/**
 * @fn ixCryptoUtilMbufToNpeFormatConvert
 * @brief To convert chained mbuf to NPE format, including endianess
 *        conversion (to Big Endian) and virtual address to physical address
 *        conversion.  And then flush the data to physical memory. Besides 
 *        that, shift the ixp_data buffer pointerin NPE shared structure to 
 *        right based on shiftOffset
 *
 */

IxCryptoNpeBuf*
ixCryptoUtilMbufToNpeFormatConvert (IX_OSAL_MBUF *pMbuf, UINT16 shiftOffset)
{
    IX_OSAL_MBUF *pChain = pMbuf;
    IX_OSAL_MBUF *pNext = NULL;
    IxCryptoNpeBuf *pNpeBuf = NULL;
   
    /* Check if the mbuf pointer is NULL, if NULL, return the pointer 
     * without doing anything 
     */
    if (NULL == pMbuf)
    {
        return NULL;
    } /* end of if (pMbuf) */
      
    /* Convert the address of the mbuf into physical adddress */
    pNpeBuf = (IxCryptoNpeBuf *) IX_CRYPTO_VIRTUAL_TO_PHYSICAL_TRANSLATION (
                  (UINT32) IX_CRYPTO_ACC_IX_NE_SHARED (pMbuf));
                            
    /* Convert the address of mbuf into network order (big Endian) */
    pNpeBuf = (IxCryptoNpeBuf *) IX_CRYPTO_CONVERT_WORD_TO_NETWORK_ORDER (
                  (UINT32) pNpeBuf);
   
    while (pChain) /* while (pChain != NULL) */
    {
        /* Flush the cache lines associated with cluster into external 
         * memory 
         */
        IX_CRYPTO_DATA_CACHE_FLUSH (
            IX_OSAL_MBUF_MDATA (pChain), 
            IX_OSAL_MBUF_MLEN (pChain));

        /* Extract next mbuf address */
        pNext = IX_OSAL_MBUF_NEXT_BUFFER_IN_PKT_PTR (pChain);

        /* Only convert next buffer address if it is not NULL */
        if (pNext)
        {
            /* Convert next buffer addr into physical address and network order;
             * then assign it into NPE shared structure (ix_ne). 
             */
            IX_CRYPTO_ACC_IX_NE_SHARED_NEXT (pChain) 
                = (IxCryptoNpeBuf *) IX_CRYPTO_VIRTUAL_TO_PHYSICAL_TRANSLATION (
                      (UINT32) IX_CRYPTO_ACC_IX_NE_SHARED (pNext));
            IX_CRYPTO_ACC_IX_NE_SHARED_NEXT (pChain)
                = (IxCryptoNpeBuf *)IX_CRYPTO_CONVERT_WORD_TO_NETWORK_ORDER (
                      (UINT32) IX_CRYPTO_ACC_IX_NE_SHARED_NEXT (pChain));
        } /* end of if (pNext) */
        
        /* Extract mdata buffer pointer from mbuf and shift the pointer to the
         * right according to the shiftOffset. Convert the address and assign
         * it into NPE shared structure.
         */
        IX_CRYPTO_ACC_IX_NE_SHARED_DATA (pChain) 
            = (INT8 *) (IX_OSAL_MBUF_MDATA (pChain) + shiftOffset);
        IX_CRYPTO_ACC_IX_NE_SHARED_DATA (pChain) 
            = (INT8 *) IX_CRYPTO_VIRTUAL_TO_PHYSICAL_TRANSLATION (
                  (UINT32) IX_CRYPTO_ACC_IX_NE_SHARED_DATA (pChain));
  
        /* Convert data pointer into network order (Big Endian) */
        IX_CRYPTO_ACC_IX_NE_SHARED_DATA (pChain)
            = (INT8 *) IX_CRYPTO_CONVERT_WORD_TO_NETWORK_ORDER (
                  (UINT32) IX_CRYPTO_ACC_IX_NE_SHARED_DATA (pChain));
                  
        /* Assign and convert buffer length into NPE shared structure */
        IX_CRYPTO_ACC_IX_NE_SHARED_LEN (pChain) 
            = IX_OSAL_MBUF_MLEN (pChain) & IX_CRYPTO_ACC_IX_NE_SHARED_LEN_MASK;
        IX_CRYPTO_ACC_IX_NE_SHARED_LEN (pChain) 
            = IX_CRYPTO_CONVERT_SHORT_TO_NETWORK_ORDER (
                IX_CRYPTO_ACC_IX_NE_SHARED_LEN (pChain));

        /*******************************
         * For future expansion 
         *
         * IX_CRYPTO_ACC_IX_NE_SHARED_PKT_LEN (pChain) 
         *   = IX_OSAL_MBUF_PKT_LEN (pChain) 
         *       & IX_CRYPTO_ACC_IX_NE_SHARED_PKT_LEN_MASK;
         * IX_CRYPTO_ACC_IX_NE_SHARED_PKT_LEN (pChain) 
         *   = IX_CRYPTO_CONVERT_SHORT_TO_NETWORK_ORDER (
         *       IX_CRYPTO_ACC_IX_NE_SHARED_PKT_LEN (pChain)));
         *
         *********************************/   

        /* Flush the cache lines associated with the mbuf header into 
         * external memory 
         */
        IX_CRYPTO_DATA_CACHE_FLUSH (pChain, sizeof(IX_OSAL_MBUF));

        /* Go to next mbuf */
        pChain  = pNext;
    }
   
    return pNpeBuf;
} /* end of ixCryptoUtilMbufToNpeFormatConvert () function */



/**
 * @fn ixCryptoUtilMbufFromNpeFormatConvert
 * @brief Conversion of addresses or control information in buffer is not needed
 *        as the addresses / control information never get converted. NPE 
 *        shared structure information also do no need to be converted back, as 
 *        the information is duplicated from the control structure in buffer. 
 *        This function needs to invalidate the cache.
 *
 */
IX_OSAL_MBUF*
ixCryptoUtilMbufFromNpeFormatConvert (IX_OSAL_MBUF *pMbuf)
{
    IX_OSAL_MBUF *pChain = pMbuf;
    
    /* Check if the mbuf pointer is NULL, if NULL, return the pointer 
     * without doing anything 
     */
    if (NULL == pMbuf)
    {
        return pMbuf;
    } /* end of if (pMbuf) */
    
    while (pChain) /* while (pChain != NULL) */
    {
        /* Invalidate the mbuf header prior to operating on it */
        IX_CRYPTO_DATA_CACHE_INVALIDATE (pChain, sizeof(IX_OSAL_MBUF));
        
        /* Invalidate the cluster if present in the cache */
        IX_CRYPTO_DATA_CACHE_INVALIDATE (
            IX_OSAL_MBUF_MDATA (pChain), 
            IX_OSAL_MBUF_MLEN (pChain));

        /* Go to the next buffer in the chain */
        pChain = IX_OSAL_MBUF_NEXT_BUFFER_IN_PKT_PTR (pChain);
   }
    
    return pMbuf;
} /* end of ixCryptoUtilMbufFromNpeFormatConvert () function */


/**
 * @fn ixCryptoUtilNpeCryptCfgGenerate
 *
 * @brief To generate Npe Crypt Cfg word that will be written to NPE.  
 */
void
ixCryptoUtilNpeCryptCfgGenerate (
    UINT32 *pNpeCryptCfgWord, 
    IxCryptoNpeCryptCfgWord *pNpeCryptCfg)
{
    *pNpeCryptCfgWord = (UINT32)(
    (pNpeCryptCfg->reserved0   << IX_CRYPTO_NPE_CRYPT_CFG_RESERVED0_LOC)  |
    (pNpeCryptCfg->desKeyWr    << IX_CRYPTO_NPE_CRYPT_CFG_DESKEYWR_LOC)   |
    (pNpeCryptCfg->desKeyRd    << IX_CRYPTO_NPE_CRYPT_CFG_DESKEYRD_LOC)   |
    (pNpeCryptCfg->desDataWr   << IX_CRYPTO_NPE_CRYPT_CFG_DESDATAWR_LOC)  |
    (pNpeCryptCfg->desDataRd   << IX_CRYPTO_NPE_CRYPT_CFG_DESDATARD_LOC)  |
    (pNpeCryptCfg->reserved1   << IX_CRYPTO_NPE_CRYPT_CFG_RESERVED1_LOC)  |
    (pNpeCryptCfg->cipherMode  << IX_CRYPTO_NPE_CRYPT_CFG_CIPHERMODE_LOC) |
    (pNpeCryptCfg->cryptAlgo   << IX_CRYPTO_NPE_CRYPT_CFG_CRYPTALGO_LOC)  |
    (pNpeCryptCfg->encrypt     << IX_CRYPTO_NPE_CRYPT_CFG_ENCRYPT_LOC)    |
    (pNpeCryptCfg->cryptMode   << IX_CRYPTO_NPE_CRYPT_CFG_CRYPTMODE_LOC)  |
    (pNpeCryptCfg->keyLength    << IX_CRYPTO_NPE_CRYPT_CFG_KEYLENGTH_LOC)
    );
}


/**
 * @fn ixCryptoUtilNpeHashCfgGenerate
 *
 * @brief To generate Npe Hash Cfg word that will be written to NPE.  
 */
void
ixCryptoUtilNpeHashCfgGenerate (
    UINT32 *pNpeHashCfgWord, 
    IxCryptoNpeHashCfgWord *pNpeHashCfg)
{
    *pNpeHashCfgWord = (UINT32)(
    (pNpeHashCfg->chnWr       << IX_CRYPTO_NPE_HASH_CFG_CHNWR_LOC)        |
    (pNpeHashCfg->chnRd       << IX_CRYPTO_NPE_HASH_CFG_CHNRD_LOC)        |
    (pNpeHashCfg->hdWr        << IX_CRYPTO_NPE_HASH_CFG_HDWR_LOC)         |
    (pNpeHashCfg->hdRd        << IX_CRYPTO_NPE_HASH_CFG_HDRD_LOC)         |
    (pNpeHashCfg->reserved    << IX_CRYPTO_NPE_HASH_CFG_RESERVED_LOC)     |
    (pNpeHashCfg->hashAlgo    << IX_CRYPTO_NPE_HASH_CFG_HASHALGO_LOC)     |
    (pNpeHashCfg->digestLength<< IX_CRYPTO_NPE_HASH_CFG_DIGESTLENGTH_LOC) |
    (pNpeHashCfg->CVLength    << IX_CRYPTO_NPE_HASH_CFG_CVLENGTH_LOC)     
    );
}
