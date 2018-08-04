/**
 * @file   IxCryptoAcc.h
 *
 * @date   2-OCT-2002
 *
 * @brief   Header file for the IXP400 Security component (IxCryptoAcc)
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


/**
 * @defgroup IxCryptoAcc IXP400 Security (IxCryptoAcc) API
 *
 * @brief IXP400 Security component Public API
 *
 * @{
 */

#ifndef IXCRYPTOACC_H
#define IXCRYPTOACC_H

/*
 * Put the user defined include files required
 */

#include "IxOsal.h"


/*
 * #defines and macros used in this file.
 */

#define IX_CRYPTO_ACC_MAX_CIPHER_KEY_LENGTH 32  /**<
                                                 * Max length (byte) of cipher
                                                 * key for DES (64 bit),
                                                 * 3DES (192 bit), AES (128, 192
                                                 * & 256 bit).
                                                 */
#define IX_CRYPTO_ACC_MAX_CIPHER_IV_LENGTH 16   /**<
                                                 * Max IV length in bytes.
                                                 */
#define IX_CRYPTO_ACC_MAX_AUTH_KEY_LENGTH 64    /**<
                                                 * Max length (byte) of
                                                 * authentication key for
                                                 * SHA1 and MD5.
                                                 * Key size > 64 bytes need to 
                                                 * be hashed to produce 
                                                 * shorter key by calling API 
                                                 * ixCryptoAccHashKeyGenerate().
                                                 * (SHA1 : 20 bytes, MD5 : 16 
                                                 * bytes by default according 
                                                 * to RFC2104.       
                                                 * If L <= key size <= 64 bytes 
                                                 * (L=16 for MD5 & 20 for SHA1) 
                                                 * authentication key can be 
                                                 * used for registration 
                                                 * directly.
                                                 * If key size < L, client will
                                                 * need to pad the key with 0s 
                                                 * to length L before calling
                                                 * ixCryptoAccCtxRegister() API.
                                                 */
#define IX_CRYPTO_ACC_MAX_AUTH_IV_LENGTH 20     /**<
                                                 * Max length (byte) of initial
                                                 * chaining variable for
                                                 * SHA1 (160 bit) and
                                                 * MD5 (128 bit).
                                                 */
#define  IX_CRYPTO_ACC_MAX_QUEUE_DEPTH 64       /**<
                                                 * Max queue depth supported
                                                 * by the Queue Manager.
                                                 */
#define  IX_CRYPTO_ACC_MAX_ACTIVE_SA_TUNNELS 1000  /**<
                                                    * Maximum active tunnels
                                                    * supported could be changed
                                                    * by the client based on the
                                                    * application's requirements.
                                                    * Number of active tunnels
                                                    * will not impact the
                                                    * performance but will have
                                                    * an impact on the memory
                                                    * needed to keep the
                                                    * crypto context information
                                                    * will depend on it. Overall
                                                    * memory requirement depends
                                                    * on the number of tunnels.
                                                    */


/*
 * Constants value for DES algorithm in unit bytes
 */

#define IX_CRYPTO_ACC_DES_KEY_64     8  /**<
                                         * DES key length in bytes
                                         */
#define IX_CRYPTO_ACC_DES_BLOCK_64   8  /**<
                                         * DES cipher block length in bytes
                                         */
#define IX_CRYPTO_ACC_DES_IV_64      8  /**<
                                         * DES initialization vector length 
                                         * in bytes
                                         */
#define IX_CRYPTO_ACC_3DES_KEY_192   24 /**<
                                         * 3DES key length in bytes
                                         */

/*
 * Constants value for AES algorithm in unit bytes
 */

#define IX_CRYPTO_ACC_AES_KEY_128    16 /**<
                                         * AES-128 key length in bytes
                                         */
#define IX_CRYPTO_ACC_AES_KEY_192    24 /**<
                                         * AES-192 key length in bytes
                                         */
#define IX_CRYPTO_ACC_AES_KEY_256    32 /**<
                                         * AES-256 key length in bytes
                                         */
#define IX_CRYPTO_ACC_AES_BLOCK_128  16 /**<
                                         * AES cipher block length in bytes
                                         */
#define IX_CRYPTO_ACC_AES_CBC_IV_128 16 /**<
                                         * AES initialization vector length 
                                         * in bytes for CBC mode
                                         */
#define IX_CRYPTO_ACC_AES_CTR_IV_128 16  /**<
                                         * AES initialization vector length 
                                         * in bytes for CTR mode.
                                         */

#define IX_CRYPTO_ACC_CCM_AAD_LEN_384   48 
                                        /**<
                                         * 48 Bytes of additional authenticated
                                         * data for CBC-MAC computation.
                                         * Currently only this value is 
                                         * supported.
                                         */

#define IX_CRYPTO_ACC_AES_CCM_IV_512    \
                  (IX_CRYPTO_ACC_AES_CTR_IV_128 + IX_CRYPTO_ACC_CCM_AAD_LEN_384)
                                        /**<
                                         * CCM Initialization vector has 2
                                         * components to it. First is the IV
                                         * for the CTR mode encryption and 
                                         * second is the additional authentication
                                         * data for the CBC-MAC generation.
					                     */

/*
 * Constants value for ARC4 algorithm in unit bytes
 */

#define IX_CRYPTO_ACC_ARC4_KEY_128 16    /**<
                                          * Key length for ARC4 algorithms.
                                          */
#define IX_CRYPTO_ACC_ARC4_BLOCK_8    1  /**<
                                          * ARC4 algorithm block size
                                          */                                           
                                          
/*
 * Constants value for SHA1 algorithm in unit bytes
 */

#define IX_CRYPTO_ACC_SHA1_KEY_160    20    /**<
                                             * SHA1 key length in bytes
                                             */
#define IX_CRYPTO_ACC_SHA1_DIGEST_160 20    /**< 
                                             * SHA1 message digest length
                                             * in bytes
                                             */


/*
 * Constants value for MD5 algorithm in unit bytes
 */

#define IX_CRYPTO_ACC_MD5_KEY_128     16    /**<
                                             * MD5 key length in bytes
                                             */
#define IX_CRYPTO_ACC_MD5_DIGEST_128  16    /**< 
                                             * MD5 message digest length
                                             * in bytes
                                             */

/*
 * Constants value for CBC MAC algorithm in unit bytes
 */
#define IX_CRYPTO_ACC_CCM_DIGEST_64   8     /**<
                                             * CCM digest length in bytes.
                                             * Currently only this value
                                             * (of 8 bytes)is supported for
                                             * CCM digest length.
                                             */
                                          

/*
 * Constants value for WEP CRC/IV algorithm in unit bytes
 */

#define IX_CRYPTO_ACC_WEP_CRC_DIGEST_32 4  /**<
                                            * Digest length of WEP ICV in bytes
                                            */
 
/*
 * Typedefs whose scope is limited to this file.
 */


/**
 * @enum IxCryptoAccCfg
 *
 * @brief Possible configuration definitions.
 *
 * @note Enums to define various possible configurations.
 *    
 */
typedef enum
{
    IX_CRYPTO_ACC_CFG_WEP_XSCALE_ACC_EN,  /**< Enable access to WEP-XScale
                                           * processing only. Access to WEP-NPE
                                           * and hardware accelerators are 
                                           * disabled
                                           */
    IX_CRYPTO_ACC_CFG_CRYPTO_NPE_ACC_EN,  /**< 
                                           * Enable access to Hardware 
                                           * accelerators (WEP XScale engine 
                                           * is also enabled). This is the 
                                           * default configuration for
                                           * the cryptoAcc component.
                                           */
    IX_CRYPTO_ACC_CFG_WEP_NPE_ACC_EN,     /**< Enable access to WEP NPE 
                                           * and XScale engine.
                                           */

    IX_CRYPTO_ACC_CFG_CRYPTO_WEP_NPE_ACC_EN,
                                         /**< Enable access to the Hardware 
                                          * accelerators, WEP - NPE and XScale
                                          * engines.
                                          */
    IX_CRYPTO_ACC_CFG_TYPE               /**< 
                                          * Maximum value for types of 
                                          * configurations.
                                          */
} IxCryptoAccCfg;



/**
 * @enum IxCryptoAccOperation
 *
 * @brief Cryptographic Operation Definitions  
 *
 */ 
typedef enum 
{
    IX_CRYPTO_ACC_OP_ENCRYPT = 0,       /**< Encrypt operation */
    IX_CRYPTO_ACC_OP_DECRYPT,           /**< Decrypt operation */
    IX_CRYPTO_ACC_OP_AUTH_CALC,         /**< 
                                         * Authentication calculation 
                                         * operation
                                         */
    IX_CRYPTO_ACC_OP_AUTH_CHECK,        /**<
                                         * Authentication verification 
                                         * operation
                                         */
    IX_CRYPTO_ACC_OP_ENCRYPT_AUTH,      /**<
                                         * Encryption followed by 
                                         * authentication calculation 
                                         * operation
                                         */
    IX_CRYPTO_ACC_OP_AUTH_DECRYPT,      /**<
                                         * Authentication verification 
                                         * followed by decryption
                                         */
    IX_CRYPTO_ACC_OP_TYPE_OF_OPERATION  /**< 
                                         * Maximum value for types of
                                         * operation
                                         */
} IxCryptoAccOperation;



/**
 * @enum IxCryptoAccCipherAlgo
 *
 * @brief Cipher Algorithm Definitions  
 *
 * @note 3DES and AES will be supported if not violating import/export rules.
 */
typedef enum
{
    IX_CRYPTO_ACC_CIPHER_NULL = 0,    /**< NULL encryption */
    IX_CRYPTO_ACC_CIPHER_DES,         /**< DES algorithm */
    IX_CRYPTO_ACC_CIPHER_3DES,        /**< Triple DES algorithm */
    IX_CRYPTO_ACC_CIPHER_AES,         /**< AES algorithm */
    IX_CRYPTO_ACC_CIPHER_ARC4,        /**< ARC4 Algorithm */
    IX_CRYPTO_ACC_CIPHER_ALGO_TYPE    /**< 
                                       * Maximum value for types of cipher 
                                       * algorithm
                                       */
} IxCryptoAccCipherAlgo;



/**
 * @enum IxCryptoAccCipherMode
 *
 * @brief Cipher Mode Definitions  
 *
 * @note CFB and OFB are not supported.
 */
typedef enum
{
    IX_CRYPTO_ACC_MODE_NULL = 0,    /**< NULL cipher mode */
    IX_CRYPTO_ACC_MODE_ECB,         /**< ECB mode of operation */
    IX_CRYPTO_ACC_MODE_CBC,         /**< CBC mode of operation */
    IX_CRYPTO_ACC_MODE_CTR,         /**< 
                                     * CTR mode of operation, only 
                                     * applicable to AES
                                     */
    IX_CRYPTO_ACC_MODE_CCM,         /**< CCM mode of operation, only 
                                     * applicable with AES cipher Algo. 
                                     */
    IX_CRYPTO_ACC_MODE_TYPE         /**< 
                                     * Maximum value for types of 
                                     * operation 
                                     */
} IxCryptoAccCipherMode;



/**
 * @enum IxCryptoAccAuthAlgo
 *
 * @brief Authentication Algorithm Definitions  
 *
 * @note Only two authentication algorithms are supported, SHA1 and MD5
 *       for IPSEC application, while WEP_CRC is only for WEP application.
 */
typedef enum
{
    IX_CRYPTO_ACC_AUTH_NULL = 0,    /**< NULL authentication */
    IX_CRYPTO_ACC_AUTH_SHA1,        /**< SHA1 algorithm */
    IX_CRYPTO_ACC_AUTH_MD5,         /**< MD5 algorithm */
    IX_CRYPTO_ACC_AUTH_CBC_MAC,     /**< CBC MAC algorithm only applicable 
                                     * along with CCM cipher mode
                                     */
    IX_CRYPTO_ACC_AUTH_WEP_CRC,     /**< WEP CRC algorithm */
    IX_CRYPTO_ACC_AUTH_TYPE         /**< 
                                     * Maximum value for types of 
                                     * authentication algorithm
                                     */
} IxCryptoAccAuthAlgo;



/**
 * @enum IxCryptoAccStatus
 *
 * @brief Status Definitions  
 *
 * @note These status will be used by the APIs to return to the client.
 */
typedef enum
{
    IX_CRYPTO_ACC_STATUS_SUCCESS = IX_SUCCESS,      /**< Success status */
    IX_CRYPTO_ACC_STATUS_FAIL = IX_FAIL,            /**< Fail status */
    IX_CRYPTO_ACC_STATUS_WAIT,                      /**< Wait status */
    IX_CRYPTO_ACC_STATUS_RETRY,                     /**< Retry status */
    IX_CRYPTO_ACC_STATUS_QUEUE_FULL,                /**< Queue full */
    IX_CRYPTO_ACC_STATUS_OPERATION_NOT_SUPPORTED,   /**< Invalid operation */
    IX_CRYPTO_ACC_STATUS_CIPHER_ALGO_NOT_SUPPORTED, /**<
                                                     * Invalid cipher algorithm
                                                     */
    IX_CRYPTO_ACC_STATUS_CIPHER_MODE_NOT_SUPPORTED, /**< 
                                                     * Invalid cipher mode of
                                                     * operation
                                                     */
    IX_CRYPTO_ACC_STATUS_CIPHER_INVALID_KEY_LEN,    /**<
                                                     * Invalid cipher key
                                                     *length
                                                     */
    IX_CRYPTO_ACC_STATUS_CIPHER_INVALID_IV_LEN,     /**< 
                                                     * Invalid IV length
                                                     */
    IX_CRYPTO_ACC_STATUS_CIPHER_INVALID_BLOCK_LEN,  /**<
                                                     * Invalid cipher block
                                                     * length
                                                     */
    IX_CRYPTO_ACC_STATUS_AUTH_ALGO_NOT_SUPPORTED,   /**< 
                                                     * Invalid authentication 
                                                     * algorithm
                                                     */
    IX_CRYPTO_ACC_STATUS_AUTH_INVALID_DIGEST_LEN,   /**<
                                                     * Invalid message digest
                                                     * length
                                                     */
    IX_CRYPTO_ACC_STATUS_AUTH_INVALID_KEY_LEN,      /**< 
                                                     * Invalid authentication 
                                                     * key length 
                                                     */
    IX_CRYPTO_ACC_STATUS_AUTH_FAIL,                 /**<  
                                                     * Authentication 
                                                     * verification failed 
                                                     */
    IX_CRYPTO_ACC_STATUS_CRYPTO_CTX_NOT_VALID,      /**< 
                                                     * Invalid crypto 
                                                     * context ID
                                                     */
    IX_CRYPTO_ACC_STATUS_EXCEED_MAX_TUNNELS ,       /**< 
                                                     * Exceed maximum number 
                                                     * of crypto contexts 
                                                     * allocation
                                                     */
    IX_CRYPTO_ACC_STATUS_AUTH_INVALID_AAD_LEN       /**<
                                                     * Invalid additional 
                                                     * authentication length
                                                     */
} IxCryptoAccStatus;



/**
 * @brief   Structure storing cipher configuration parameters required to
 *          perform security functionality.
 *
 * Structure is used to specify the cipher context required for using the 
 * @ref IxCryptoAcc interface.
 * @note    SPI (Security Parameter Index) is not needed for crypto context
 *          registration as CTR counter block construction is client's 
 *          responsibility
 */ 
typedef struct
{
    IxCryptoAccCipherAlgo cipherAlgo; /**< Cipher Algorithm - DES, 3DES, 
                                       * AES, ARC4
                                       */
    IxCryptoAccCipherMode cipherMode; /**< Cipher mode - ECB, CBC, CTR, CCM */
    UINT32 cipherKeyLen;  /**< Cipher key length in bytes */
    union 
    {
        UINT8 cipherKey[IX_CRYPTO_ACC_MAX_CIPHER_KEY_LENGTH]; /**< default
                                                               * key array
                                                               */ 
        UINT8 desKey[IX_CRYPTO_ACC_DES_KEY_64];         /**< DES key */
        UINT8 tripleDesKey[IX_CRYPTO_ACC_3DES_KEY_192]; /**< 3DES key */
        UINT8 aesKey128[IX_CRYPTO_ACC_AES_KEY_128];     /**< AES-128 key */
        UINT8 aesKey192[IX_CRYPTO_ACC_AES_KEY_192];     /**< AES-192 key */
        UINT8 aesKey256[IX_CRYPTO_ACC_AES_KEY_256];     /**< AES-256 key */
    } key;                          /**< Cipher key, key is not required 
                                     * for ARC4 during registration
                                     */
    UINT32 cipherBlockLen;          /**< 
                                     * Cipher block length in bytes.
                                     * (DES/3DES - 8 bytes, AES - 16 bytes)
                                     * (ARC4 - 1 byte)
                                     */
    UINT32 cipherInitialVectorLen;  /**< Length of IV in bytes */
} IxCryptoAccCipherCtx;



/**
 * @brief   Structure storing authentication configuration parameters required 
 *          to perform security functionality.
 *
 * Structure is used to specify the authentication context required for using  
 * the @ref IxCryptoAcc interface.
 */
typedef struct
{
    IxCryptoAccAuthAlgo authAlgo; /**< authentication algorithm -
                                   * MD5, SHA1, WEP_CRC
                                   */
    UINT32 authDigestLen; /**< Digest length in bytes */
    UINT32 authKeyLen;    /**< Hash key length in bytes */
    UINT32 aadLen;        /**< Additional Authentication Data (AAD) length 
                            * in bytes. This is the 16 bytes of initial block
                            * (called B0, in RFC 3610 and CCM Initial block 
                            * in 802.11i spec), followed by additional 
                            * authentication data (lengths- defined in 
                            * respective standards).
                            */
    union   
    {
        UINT8 authKey[IX_CRYPTO_ACC_MAX_AUTH_KEY_LENGTH]; /**< default hash
                                                           * key array
                                                           */
        UINT8 sha1Key[IX_CRYPTO_ACC_MAX_AUTH_KEY_LENGTH]; /**< SHA1 key */
        UINT8 md5Key[IX_CRYPTO_ACC_MAX_AUTH_KEY_LENGTH];  /**< MD5 key */
    } key; /**< Hash key, key is not required for WEP_CRC */      
} IxCryptoAccAuthCtx;   



/**
 * @brief   Structure storing configuration parameters required to
 *          perform security functionality.
 *
 * Structure is used to specify the crypto context (hardware accelerator
 * context) required for using the @ref IxCryptoAcc interface.
 *
 */ 
typedef struct
{
    IxCryptoAccOperation operation;     /**< Types of operation */
    IxCryptoAccCipherCtx cipherCtx;     /**< Cipher context */
    IxCryptoAccAuthCtx authCtx;         /**< Authentication context */
    BOOL useDifferentSrcAndDestMbufs;   /**< 
                                         *  If TRUE, data is read from srcMbuf, 
                                         *  result is written to destMbuf
                                         *  (non in-place operation). 
                                         *  If FALSE data is read from srcMbuf, 
                                         *  and written back to srcMbuf 
                                         *  (in-place operation). 
                                         *  Default is FALSE. Note that only
                                         *  the crypted/authenticated data that 
                                         *  is copied, not the entire source 
                                         *  mbuf.
                                         */ 
} IxCryptoAccCtx;



/**
 *
 * @ingroup IxCryptoAcc
 *
 * @brief  Cryptographic Context registration complete callback notification.
 *
 * This function is called to notify a client that the Crypto Context has 
 * been registered. This function returns status through the associated 
 * cryptoCtxId once the initial values needed are calculated by Network 
 * Processor Engine (NPE) and stored in Cryptographic Context Database (CCD).  
 *
 * The CryptoCtxId is valid until ixCryptoAccCtxUnregister is invoked.
 * 
 * If the callback function returns the IX_CRYPTO_ACC_STATUS_WAIT status, it 
 * indicates that registration is not complete yet, but the mbuf pointer 
 * needs to be freed by client. Client needs to wait for the next
 * completion indication. Registration complete successfully only if status 
 * IX_CRYPTO_ACC_STATUS_SUCCESS is received.
 *
 * @param   cryptoCtxId UINT32 [in] - This crypto context ID is provided when  
 *          client sends in request via ixCryptoAccCtxRegister API to register 
 *          crypto context. cryptoCtxId points to a struct consists of  
 *          cryptographic parameters required by the Network Processor Engine  
 *          (NPE) in CCD database.
 * @param   *pMbuf @ref IX_OSAL_MBUF [in] - Pointer to the mbuf (to be freed by 
 *          client). The client should free any mbuf that is not NULL.
 * @param   status @ref IxCryptoAccStatus [in] - Status (IxCryptoAccStatus) 
 *          reporting to client.
 *          
 * @note @li   IX_CRYPTO_ACC_STATUS_SUCCESS - registration is successful.
 * @note @li   IX_CRYPTO_ACC_STATUS_FAIL - registration failed.
 * @note @li   IX_CRYPTO_ACC_STATUS_WAIT - registration is not complete yet, 
 *             wait for next completion indication. NPE is busy in 
 *             calculating the initial variables needed for the registration.
 *
 */
typedef void (*IxCryptoAccRegisterCompleteCallback)(
    UINT32 cryptoCtxId,
    IX_OSAL_MBUF *pMbuf, 
    IxCryptoAccStatus status);


/**
 *
 * @ingroup IxCryptoAcc
 *
 * @brief  Hash key generation complete callback notification.
 *
 * This function is called to notify a client that the hash key has been 
 * generated. This function will return the status through the associated 
 * hashKeyId  once the key is calculated by Network Processor Engine (NPE).
 * 
 * hashKeyId becomes invalid once the notification callback is called.
 *
 * @param   hashKeyId UINT32 [in] - This hashKeyId is provided when client sends 
 *          in request to hash the key. 
 * @param   *pMbufHashKey @ref IX_OSAL_MBUF [in] - Pointer to the mbuf that 
 *          contains original key and generated hash key. Client will need to  
 *          copy the generated hash key from the mbuf into crypto context and  
 *          used it as hash key for crypto registration request. 
 *          This mbuf will be freed by client. 
 * @param   status @ref IxCryptoAccStatus [in] - Status (IxCryptoAccStatus) 
 *          reporting to client.
 *          
 * @note @li   IX_CRYPTO_ACC_STATUS_SUCCESS - hash key generation is successful.
 * @note @li   IX_CRYPTO_ACC_STATUS_FAIL - hash key generation failed.
 *
 */
typedef void (*IxCryptoAccHashKeyGenCompleteCallback)(
    UINT32 hashKeyId,
    IX_OSAL_MBUF *pMbufHashKey, 
    IxCryptoAccStatus status);
 
 
 /**
 *
 * @ingroup IxCryptoAcc
 *
 * @brief  Hardware accelerator service request complete callback notification.
 *
 * This function is called to notify a client that the cryptographic transaction
 * has been completed. The cryptoCtxId and status of completed operation are 
 * returned to the client through this callback function to indicate operation
 * which crypto context has been completed. The CryptoCtxId is obtained via 
 * ixCryptoAccCtxRegister, and this ID is unique to a particular IPSec 
 * tunnel.
 *
 * @param   cryptoCtxId UINT32 [in] - This crypto context ID is provided when 
 *          client sends in request via ixCryptoAccCtxRegister API to register 
 *          crypto context. cryptoCtxId points to a struct consists of cryptographic 
 *          parameters required by the Network Processor Engine (NPE) in CCD 
 *          database.
 * @param   *pSrcMbuf @ref IX_OSAL_MBUF [in] - Pointer to the source mbuf which     
 *          contains the data to be processed. It is also the output mbuf which   
 *          contains the processed data if UseDifferentSrcAndDestMbufs is FALSE.
 * @param   *pDestMbuf @ref IX_OSAL_MBUF [in] - Only used if   
 *          UseDifferentSrcAndDestMbufs is TRUE. Pointer to the output mbuf 
 *          which contains processed data.
 * @param   status @ref IxCryptoAccStatus [in] - Status reporting to the client 
 *          via IxCryptoAccStatus.
 *
 * @note  @li  IX_CRYPTO_ACC_STATUS_SUCCESS - Operation is completed successfully.
 * @note  @li  IX_CRYPTO_ACC_STATUS_FAIL - Operation failed.
 * @note  @li  IX_CRYPTO_ACC_STATUS_AUTH_FAIL - Authentication is unsuccessful.
 *             Note that when authentication fails, the content of the 
 *             destination mbuf (which is the same as the source mbuf if a 
 *             normal in-place operation is performed) will be undetermined.
 *
 */
typedef void (*IxCryptoAccPerformCompleteCallback)( 
    UINT32 cryptoCtxId,
    IX_OSAL_MBUF *pSrcMbuf, 
    IX_OSAL_MBUF *pDestMbuf,
    IxCryptoAccStatus status); 


/**
 *
 * @ingroup IxCryptoAcc
 *
 * @fn IxCryptoAccStatus ixCryptoAccConfig (IxCryptoAccCfg compCfg)
 *
 * @brief Selects which interfaces need to be initialized when crypto-access  
 *        init is called. 
 * 
 * This function will set the configuration bits for initializing the interfaces
 * to the Hardware Accelerator engines on NPE-C and WEP Encryption engine on the
 * WAN-NPE. If this function is not called then, only the Hardware Accelerator 
 * engine is initialized by default in the crypto-access init function.
 * This function should be called prior to call ixCyptoAccInit function.
 *
 * <PRE>
 * Consideration on using WEP encryption engine on Intel XScale(R) core & WAN-NPE
 * ------------------------------------------------------------------------------
 * 1. Enabling WEP encryption engine on Intel XScale(R) core
 *     Pros: 
 *       i.  Higher throughput compared to WEP running on WAN-NPE.
 *       ii. Still can process WEP request on Intel XScale(R) core if WEP engine  
 *           on WAN-NPE is not available due to hardware is not present or some 
 *           interfaces(e.g. ATM/HSS/DMA) on the NPE that is being used.
 *     Cons: 
 *       i.  Lesser memory headroom and CPU bandwidth on Intel XScale(R) core  
 *           for application stack.
 *     Note: By default WEP encryption engine on Intel XScale(R) core is always 
 *           enabled for all options. 
 *
 * 2. Enabling WEP encryption engine on WAN-NPE
 *     Pros: 
 *       i.  Can process WEP request via WEP engines on both Intel XScale(R) 
 *           core  and WAN-NPE.
 *     Cons: 
 *       i.  Whole WAN-NPE is dedicated for WEP processing, no other interfaces
 *           (such as ATM, HSS, DMA) can be enabled for this case.
 *       ii. Lower throughput rate compared to WEP on Intel XScale(R) core  
 *           due to NPE core running at lower frequency compared to 
 *           Intel XScale(R) core.
 *     Note: By default WEP encryption on WAN NPE is disabled.
 * </PRE>
 *
 * @param compCfg @ref IxCryptoAccCfg [in] - Indicates the appropriate interfaces 
 *                     to be initialized for cryptoAcc access component.
 *
 * @return @li  IX_CRYPTO_ACC_STATUS_SUCCESS - Setting the configuration was 
 *              successful.
 * @return @li  IX_CRYPTO_ACC_STATUS_FAIL- The configuration setting was invalid.
 * 
 *              
 * @li   Reentrant    : yes
 * @li   ISR Callable : yes
 *
 */
PUBLIC IxCryptoAccStatus
ixCryptoAccConfig (IxCryptoAccCfg compCfg);


/**
 *
 * @ingroup IxCryptoAcc
 *
 * @fn IxCryptoAccStatus ixCryptoAccInit (void)
 *
 * @brief Initialise the Security Access component
 *
 * This function will initialize all the internal resources of IxCryptoAcc
 * access component depending upon the configuration set by the ixCryptoAccConfig
 * function. When Hardware Accelerator engine is specified, it will check whether
 * the specified AES or DES coprocessor is present and issue a warning if not. 
 * When WEP engine is specified, it checks whether AAL5 coprocessor is present
 * or not. This function should be called before any request processing
 * functions are invoked on the component. This function is called only once.
 *
 * @return @li  IX_CRYPTO_ACC_STATUS_SUCCESS - successfully initialize the 
 *              component; a warning is issued if coprocessor is not present.
 * @return @li  IX_CRYPTO_ACC_STATUS_FAIL - Initialization failed for some 
 *              unspecified internal reasons.
 *              
 * @li   Reentrant    : no
 * @li   ISR Callable : no
 *
 */
PUBLIC IxCryptoAccStatus
ixCryptoAccInit (void);


/**
 *
 * @ingroup IxCryptoAcc
 *
 * @fn IxCryptoAccStatus ixCryptoAccCtxRegister (
    IxCryptoAccCtx *pAccCtx,
    IX_OSAL_MBUF *pMbufPrimaryChainVar,
    IX_OSAL_MBUF *pMbufSecondaryChainVar,
    IxCryptoAccRegisterCompleteCallback registerCallbackFn,
    IxCryptoAccPerformCompleteCallback performCallbackFn,
    UINT32 *pCryptoCtxId)
 *
 * @brief Crypto context registration. Cryptographic Context ID (cryptoCtxId)
 *        for the registered crypto context obtained from this registration
 *        request will be used in perform service requests.  
 *
 * This function is used to register all the required information 
 * (eg. key, cipher algorithm, etc) for hardware accelerator services with 
 * IxCryptoAcc component. Those information will be stored in CCD database.  
 * All the information will be converted into a cryptographic parameters 
 * structure to be shared with NPE. The structure will be associated with 
 * a unique crypto context ID (cryptoCtxId). cryptoCtxId is passed back to 
 * client for future reference in callback function and also for hardware 
 * accelerator service requests. 
 * Besides, two empty mbuf are required to be passed in through interface for 
 * the use of access component and NPE to compute the primary and secondary 
 * initial chaining variables. pMbufPrimaryChainVar and pMbufSecondaryChainVar 
 * mbuf pointers must be NULL if authentication or combined service is not 
 * selected. 
 * When initializing authentication context for WEP CRC (i.e when auth algo is
 * IX_CRYPTO_ACC_AUTH_WEP_CRC), variables authKeyLen and keys should be ignored.
 * The authDigestLen should be set to IX_CRYPTO_ACC_WEP_CRC_DIGEST_32.
 *
 * Client should send in perform service request based on the crypto context 
 * registered. 2 different applications are supported by calling
 * different perform functions as below:
 *
 * 1. If the crypto context is registered for crypto hardware 
 *    accelerator service (DES, 3DES, AES, MD5, SHA1), client could only 
 *    call the crypto hw accelerator perform function listed below:
 *          ixCryptoAccAuthCryptPerform ()
 *
 * 2. If the crypto context is registered for WEP services (WEP_ARC4, WEP_CRC), 
 *    client could only call the WEP perform functions listed below:
 *          ixCryptoAccNpeWepPerform ()
 *          ixCryptoAccXScaleWepPerform ()
 * 
 *
 * @note
 * 3 scenarios of crypto context registration depending on authentication key 
 * size (in bytes) (only applicable to HMAC-SHA1 and HMAC-MD5):
 * 1. If (L <= key size <= 64), then call this API (ixCryptoAccCtxRegister)
 *    directly. L = 16 for MD5 and L = 20 for SHA1. 
 * 2. If (key size > 64), then authentication key needs to be hashed to become
 *    shorter key first by calling another API ixCryptoAccHashKeyGenerate ().
 *    Please follow steps below for this case
 *    - Call ixCryptoAccHashKeyGenerate () to hash the authentication key
 *    - Wait for the callback from ixCryptoAccHashKeyGenerate (). Copy 
 *      generated authentication key from mbuf into IxCryptoAccCtx as 
 *      authentication key.
 *    - Call this API (ixCryptoAccCtxRegister)to register the context.
 * 3. If (key size < L), client MUST pad the authentication key with 0s to  
 *    become L bytes of key before calling this API (ixCryptoAccCtxRegister).
 *    
 * @param   *pAccCtx @ref IxCryptoAccCtx [in] - is a pointer to hardware  
 *          accelerator context. Information required in hardware accelerator  
 *          context, such as key and algorithm configuration can be extracted  
 *          from Security Association Database (SAD). 
 *          Important Note: When the crypt algorithm  is ARC4, the keys supplied
 *          during the registration process are ignored. The caller should pass 
 *          the per packet keys in the ixCryptoAccXScaleWepPerform function's
 *          pIV parameter.
 * @param   *pMbufPrimaryChainVar @ref IX_OSAL_MBUF [in] - a pointer to an empty   
 *          mbuf(must not be chained) for the use of access component to compute 
 *          primary chaining variables for SHA1/MD5. This mbuf structure is  
 *          allocated by the client (minimum size of the cluster required is 
 *          64 bytes), only if SHA1/MD5 is selected, and the mbuf pointer must  
 *          be NULL if SHA1/MD5 is not selected. After the NPE complete the 
 *          computation, the mbuf is returned separately through the client's
 *          registered callback.
 * @param   *pMbufSecondaryChainVar @ref IX_OSAL_MBUF [in] - a pointer to an empty 
 *          mbuf(must not be chained) for the use of access component to compute 
 *          secondary chaining variables for SHA1/MD5. This mbuf structure is  
 *          allocated by the client (minimum size of the cluster required is 
 *          64 bytes), only if SHA1/MD5 is selected, and the mbuf pointer must  
 *          be NULL if SHA1/MD5 is not selected. After the NPE complete the 
 *          computation, the mbuf is returned separately through the client's
 *          registered callback.
 * @param   registerCallbackFn @ref IxCryptoAccRegisterCompleteCallback [in] - 
 *          callback function pointer to return crypto context registration
 *          status to client when the registration is complete. 
 *          This cannot be NULL.
 * @param   performCallbackFn @ref IxCryptoAccPerformCompleteCallback [in] - 
 *          callback function pointer to return the processed buffer to the
 *          client with respect to the unique CryptoCtxId. This cannot be NULL,
 *          however this function will not be called if its the requested
 *          to be executed by the engine running on Intel XScale(R) core.
 * @param   *pCryptoCtxId UINT32 [inout] - Crypto Context ID returned by access 
 *          component for the crypto context registered. 
 *
 * @return @li  IX_CRYPTO_ACC_STATUS_SUCCESS - Registration parameters are valid
 * @return @li  IX_CRYPTO_ACC_STATUS_FAIL - Registration failed for some 
 *              unspecified internal reasons.
 * @return @li  IX_CRYPTO_ACC_STATUS_OPERATION_NOT_SUPPORTED - Invalid 
 *              operation requested by the client.
 * @return @li  IX_CRYPTO_ACC_STATUS_CIPHER_ALGO_NOT_SUPPORTED - Invalid 
 *              cipher algorithm requested by the client.
 * @return @li  IX_CRYPTO_ACC_STATUS_CIPHER_MODE_NOT_SUPPORTED - Invalid 
 *              cipher mode requested by the client.
 * @return @li  IX_CRYPTO_ACC_STATUS_CIPHER_INVALID_KEY_LEN - Invalid cipher 
 *              key length passed in by the client.
 * @return @li  IX_CRYPTO_ACC_STATUS_CIPHER_INVALID_BLOCK_LEN - Invalid cipher
 *              block length passed in by the client
 * @return @li  IX_CRYPTO_ACC_STATUS_CIPHER_INVALID_IV_LEN - Invalid IV 
 *              length passed in by the client.
 * @return @li  IX_CRYPTO_ACC_STATUS_AUTH_ALGO_NOT_SUPPORTED - Invalid 
 *              authentication algorithm requested by the client.
 * @return @li  IX_CRYPTO_ACC_STATUS_AUTH_INVALID_DIGEST_LEN - Invalid 
 *              authentication digest length.
 * @return @li  IX_CRYPTO_ACC_STATUS_AUTH_INVALID_KEY_LEN - Invalid 
 *              authentication key length.
 * @return @li  IX_CRYPTO_ACC_STATUS_EXCEED_MAX_TUNNELS - Exceed maximum 
 *              tunnels permitted.
 * @return @li  IX_CRYPTO_ACC_STATUS_QUEUE_FULL - Queue full status returned 
 *              and the registration request will be rejected.
 *              
 * @li   Reentrant    : yes
 * @li   ISR Callable : no
 *
 */ 
PUBLIC IxCryptoAccStatus 
ixCryptoAccCtxRegister (
    IxCryptoAccCtx *pAccCtx,
    IX_OSAL_MBUF *pMbufPrimaryChainVar,
    IX_OSAL_MBUF *pMbufSecondaryChainVar,
    IxCryptoAccRegisterCompleteCallback registerCallbackFn,
    IxCryptoAccPerformCompleteCallback performCallbackFn,
    UINT32 *pCryptoCtxId);



/**
 * 
 * @ingroup IxCryptoAcc
 *
 * @fn IxCryptoAccStatus ixCryptoAccCtxUnregister (UINT32 cryptoCtxId)
 *
 * @brief Unregister the crypto context from Cryptographic Context Database.
 *
 * This function is for freeing the particular crypto context (reference 
 *      through CryptoCtxId) from the Cryptographic Context Database.
 *
 * @param    cryptoCtxId UINT32 [in] - is pointer to crypto context that is required  
 *           to be freed, which will be supplied by the client. The 
 *           CryptoCtxId must be a valid Id.
 *
 * @return @li  IX_CRYPTO_ACC_STATUS_SUCCESS - successfully unregister the 
 *              crypto context.
 * @return @li  IX_CRYPTO_ACC_STATUS_FAIL - Unregistration failed for some 
 *              unspecified internal reasons, e.g. uninitialized.
 * @return @li  IX_CRYPTO_ACC_STATUS_CRYPTO_CTX_NOT_VALID - invalid crypto 
 *              context.
 * @return @li  IX_CRYPTO_ACC_STATUS_RETRY - retry the unregister operation as 
 *              there are still some ixCryptoAccAuthCryptPerform requests 
 *              associated with the cryptoCtxId pending on the NPE queue. 
 *              
 * @li   Reentrant    : yes
 * @li   ISR Callable : no
 *
 *
 * <B> Assumption: </B>
 * 
 * - It is client's responsibility to ensure no pending requests for the
 *   crypto context before it proceeds to unregister the crypto context.
 *
 */
PUBLIC IxCryptoAccStatus
ixCryptoAccCtxUnregister (UINT32 cryptoCtxId);



/**
 *
 * @ingroup IxCryptoAcc
 * 
 * @fn IxCryptoAccStatus ixCryptoAccAuthCryptPerform (
    UINT32 cryptoCtxId,
    IX_OSAL_MBUF *pSrcMbuf,
    IX_OSAL_MBUF *pDestMbuf,
    UINT16 authStartOffset,
    UINT16 authDataLen,    
    UINT16 cryptStartOffset,   
    UINT16 cryptDataLen,
    UINT16 icvOffset,
    UINT8  *pIV)
 * 
 * @brief Perform Authentication and Decryption/Encryption functionalities
 *
 * This function is called for authentication and decryption/encryption 
 * functionalities service request.
 * For a combined encryption and authentication request with cipher algorithms
 * as AES or DES and authentication algorithm as MD5 and SHA1 the following 
 * restriction must be met:
 * That the  crypted data must be a subset of the authenticated data.
 * The boundary relationship of 
 * [(authStartOffset + authDataLen) >= (cryptDataLen + cryptStartOffset) >=
 * (cryptStartOffset >= authStartOffset]
 * MUST BE SATISFIED.
 * There should not be any chained mbuf boundary within an ICV field.
 *
 * For performing ARC4 and WEP ICV computations call the ixCryptoAccNpeWepPerform  
 * or ixCryptoAccXScaleWepPerform functions. The ARC4 and WEP ICV operations are  
 * not supported by this function.
 * 
 * @param   cryptoCtxId UINT32 [in] - is the crypto context pointer to be   
 *          supplied by the client. This cryptoCtxId is obtained via  
 *          ixCryptoAccCtxRegister. The cryptoCtxId must be a valid Id.
 * @param   *pSrcMbuf @ref IX_OSAL_MBUF [in] - is a pointer to mbuf which contains    
 *          data to be processed. This mbuf structure is allocated by client.  
 *          Result of this request will be stored in the same mbuf and overwritten 
 *          the original data if UseDifferentSrcAndDestMbufs flag in 
 *          IxCryptoAccCtx is set to FALSE (in-place operation). Otherwise, if
 *          UseDifferentSrcAndDestMbufs flag is set to TRUE, the result will be
 *          written into destination mbuf (non in-place operation) and the 
 *          original data in this mbuf will remain unchanged.
 *          The same pointer is then returned to the client via registered 
 *          IxCryptoAccPerformCompleteCallback callback function.
 * @param   *pDestMbuf @ref IX_OSAL_MBUF [in] - only used if 
 *          UseDifferentSrcAndDestMbufs is TRUE. This is the buffer where the  
 *          result is written to. This mbuf structure is allocated by client.  
 *          The length of mbuf MUST be big enough to hold the result of operation.   
 *          The result of operation COULD NOT span into two or more different 
 *          mbufs, thus the mbuf supplied must be at least the length of expected  
 *          result. The same pointer is then returned to the client via registered 
 *          IxCryptoAccPerformCompleteCallback callback function.
 *          The data is written back at the 'authStartOffset',if it's a
 *          authentication only or a combined request. If it's a crypt 
 *          only request the data is written starting from 'cryptStartOffset'.
 * @param   authStartOffset UINT16 [in] - supplied by the client to indicate  
 *          the start of the payload to be authenticated. Ignored when 
 *          performing request with Cipher Mode set to CCM.
 * @param   authDataLen UINT16 [in] - supplied by the client to indicate  
 *          the length of the payload to be authenticated in Bytes. The
 *          maximum data length must not exceed 65471 bytes.Ignored when 
 *          performing request with Cipher Mode set to CCM.
 * @param   cryptStartOffset UINT16 [in] - supplied by the client to indicate 
 *          the start of the payload to be decrypted/encrypted.
 * @param   cryptDataLen UINT16 [in] - supplied by the client to indicate the  
 *          length of the payload to be decrypted/encrypted in Bytes. The 
 *          payload to be decrypted/encrypted must have the length that is 
 *          multiple of cipher block length in size.
 * @param   icvOffset UINT16 [in] - supplied by the client to indicate  
 *          the start of the ICV (Integrity Check Value) used for the
 *          authentication. This ICV field should not be split across 
 *          multiple mbufs in a chained mbuf.
 * @param   *pIV UINT8 [in] - Initialization Vector  supplied by the client 
 *          to be used for the decryption/encryption processes.
 *          For CCM mode of operation, this parameter points to the CTR-IV
 *          followed by the 16 bytes of initial block (called - B0, in RFC 3610
 *          and CCM Initial block in 802.11i spec), followed by additional 
 *          authentication data (lengths - defined in respective standards).
 *          
 * @return @li  IX_CRYPTO_ACC_STATUS_SUCCESS - Operation requested is 
 *              successfully enqueued to hardware accelerator for processing.
 * @return @li  IX_CRYPTO_ACC_STATUS_FAIL - Cryptographic process  failed for 
 *              some unspecified internal reasons.
 * @return @li  IX_CRYPTO_ACC_STATUS_QUEUE_FULL - Cryptographic queue is full.
 * @return @li  IX_CRYPTO_ACC_STATUS_CIPHER_INVALID_BLOCK_LEN - Invalid plaintext
 *              / ciphertext block size passed in by the client.
 * @return @li  IX_CRYPTO_ACC_STATUS_CRYPTO_CTX_NOT_VALID - Crypto context is 
 *              not registered.
 *
 * @note 
 * -  Client shall handle IP mutable fields.
 * -  Client shall pad the IP datagram to be a multiple of cipher block size, 
 *    using ESP trailer for encryption (RFC2406, explicit padding)
 * -  NPE shall pad the IP datagram to be a multiple of hashing block size, 
 *    specified by the authentication algorithm (RFC2402, implicit padding)
 * -  For authentication generation operation, client needs to clear the field
 *    which hold the authentication data (ICV) to zeroes. While for the 
 *    authentication verification operation, client needs to supply the 
 *    authentication data at the ICV field for NPE to verify the ICV value.
 *    If ICV is embedded in the payload, client DOES NOT need to move 
 *    the ICV to the front / the back of the payload and clear the original
 *    ICV field. cryptoAcc access component will handle this.
 * -  Client shall construct the AES CTR counter block (4 bytes NONCE + 8 bytes
 *    IV + 4 bytes counter). CTR counter block implementation should be based 
 *    on internet draft / RFC for IPSEC AES CTR mode recommendation, which can
 *    be found at IETF website, www.ietf.org. Counter block will be passed in
 *    as IV in this case.
 * -  Depending upon the operating system and task switching mechanism, the operation 
 *    might "seem" to complete before this function returns to the caller. However, the
 *    return status with value success only implies that the request was succesfully
 *    enqueued to the hardware accelerator. The call of performCallBackFn indicates 
 *    the completion of operation by the hardware engine.
 * 
 * <B> Assumption: </B>
 * 
 * - Different clients should NOT hold the same crypto context. All the requests
 *   for same crypto context should be initiated from same client (thread). 
 * - This function is reentrant, and the client could submit multiple requests
 *   for the same crypto context.
 *              
 * @li   Reentrant    : yes
 * @li   ISR Callable : yes
 *
 */ 
PUBLIC IxCryptoAccStatus
ixCryptoAccAuthCryptPerform (
    UINT32 cryptoCtxId,
    IX_OSAL_MBUF *pSrcMbuf,
    IX_OSAL_MBUF *pDestMbuf,
    UINT16 authStartOffset,
    UINT16 authDataLen,    
    UINT16 cryptStartOffset,   
    UINT16 cryptDataLen,
    UINT16 icvOffset,
    UINT8  *pIV);

                 
/**
 *
 * @ingroup IxCryptoAcc
 *
 * @fn IxCryptoAccStatus ixCryptoAccNpeWepPerform ( 
    UINT32 cryptoCtxId,
    IX_OSAL_MBUF *pSrcMbuf,
    IX_OSAL_MBUF *pDestMbuf,
    UINT16   startOffset,   
    UINT16   dataLen,
    UINT16   icvOffset,
    UINT8   *pKey)
 *
 * @brief Function to invoke ARC4 and WEP ICV computations on NPE.
 *
 * This function submits the request to the NPE to perform ARC4 and WEP ICV
 * operations. If the request was not submitted to the NPE, the function returns
 * failure and the  client has the ownership of the buffers.
 * If the function returns success, then request is in the request queue or is 
 * being executed by the NPE and the client should not assume the ownership of 
 * the buffers. When the processing of the request completes, the 
 * performCallBackFn is called with the buffer pointers submitted initially and
 * the client can then assume the ownership of the buffers.
 * When WEP ICV verification operation is requested, the computed ICV is written
 * out at the ICV location for in-place mode of operation.The supplied 
 * ICV is overwritten as a result. For non-inplace mode of operation, the ICV
 * is written out at the destination mbuf and the original ICV from the source
 * is left intact.
 * This behavior is true irrespective whether it's WEP ICV verify or a combined 
 * - ARC4 decrypt and WEP ICV request.
 * 
 * @param   cryptoCtxId UINT32 [in] - is the crypto context id to be supplied by  
 *          the client. This is obtained by calling ixCryptoAccCtxRegister 
 *          function.
 * @param   *pSrcMbuf @ref IX_OSAL_MBUF [in] - is a pointer to mbuf which    
 *          contains data to be processed. This mbuf structure is allocated by 
 *          client. Result of this request will be stored in the same mbuf and  
 *          overwritten the original data if UseDifferentSrcAndDestMbufs flag in 
 *          IxCryptoAccCtx is set to FALSE (in-place operation). Otherwise, if
 *          UseDifferentSrcAndDestMbufs flag is set to TRUE, the result will be
 *          written into destination mbuf (non in-place operation) and the 
 *          original data in this mbuf will remain unchanged.
 * @param   *pDestMbuf @ref IX_OSAL_MBUF [in] - only used if   
 *          UseDifferentSrcAndDestMbufs is TRUE. This is the buffer where the  
 *          result is written to. This mbuf structure is allocated by client. 
 *          The length of mbuf MUST be big enough to hold the result of operation.   
 *          The result of operation COULD NOT span into two or more different 
 *          mbufs, thus the mbuf supplied must be at least the length of   
 *          expected result. The data is written back starting at startOffset 
 *          in the pDestMbuf.
 * @param   startOffset UINT16 [in] - supplied by the client to indicate 
 *          the start of the payload to be decrypted/encrypted or authenticated.
 * @param   dataLen UINT16 [in] - supplied by the client to indicate the  
 *          length of the payload to be decrypted/encrypted in number of bytes.
 * @param   icvOffset UINT16 [in] - supplied by the client to indicate  
 *          the start of the ICV (Integrity Check Value) used for the
 *          authentication. This ICV field should not be split across 
 *          multiple mbufs in a chained mbuf.
 * @param   *pKey UINT8 [in] - Pointer to IX_CRYPTO_ACC_ARC4_KEY_128 bytes of per
 *           packet ARC4 keys. This pointer can be NULL if the request is WEP IV
 *           gen or verify only.
 *          
 * @return @li  IX_CRYPTO_ACC_STATUS_SUCCESS - The parameters check passed and 
 *              the request was submitted to the NPE.
 * @return @li  IX_CRYPTO_ACC_STATUS_FAIL - The request failed because there was  
 *              a parameter inconsistency or an invalid valid operation was
 *              requested.
 * @note
 * -  Depending upon the operating system and task switching mechanism, the operation 
 *    might "seem" to complete before this function returns to the caller. However, the
 *    return status with value success only implies that the request was succesfully
 *    enqueued to the hardware accelerator. The call of performCallBackFn indicates 
 *    the completion of operation by the hardware engine.
 *
 * @li   Reentrant    : yes
 * @li   ISR Callable : yes
 *
 */
PUBLIC IxCryptoAccStatus
ixCryptoAccNpeWepPerform (UINT32   cryptoCtxId,
    IX_OSAL_MBUF *pSrcMbuf,
    IX_OSAL_MBUF *pDestMbuf,
    UINT16   startOffset,   
    UINT16   dataLen,
    UINT16   icvOffset,
    UINT8   *pKey);
                 
                 
/**
 *
 * @ingroup IxCryptoAcc
 *
 * @fn IxCryptoAccStatus ixCryptoAccXScaleWepPerform (
    UINT32 cryptoCtxId,
    IX_OSAL_MBUF *pSrcMbuf,
    IX_OSAL_MBUF *pDestMbuf,
    UINT16 startOffset,   
    UINT16 dataLen,
    UINT16 icvOffset,
    UINT8  *pKey)
 *
 * @brief Function to support ARC4 and WEP ICV computations on Intel XScale(R) 
 *        core.
 *
 * This function executes ARC4 and WEP ICV algorithms on Intel XScale(R) core. 
 * The request is not forwarded to the NPE. The operation is executed on Intel 
 * XScale(R) core synchronously and therefore the perform done call back 
 * function would not be called on its return.
 * The caller can assume the ownership of the buffers when the function returns.
 * Restrictions: This function does not support non-in-place mode of operation.
 * When WEP ICV verification operation is requested, the computed ICV is written
 * out at the ICV location for in-place mode of operation.The supplied 
 * ICV is overwritten as a result.
 * This behavior is true irrespective whether it's WEP ICV verify or a combined 
 * - ARC4 decrypt and WEP ICV request.
 
 * @param   cryptoCtxId UINT32 [in] - is the crypto context id to be supplied by  
 *          the client. This is obtained by calling ixCryptoAccCtxRegister 
 *          function.
 * @param   *pSrcMbuf @ref IX_OSAL_MBUF [in] - is a pointer to mbuf which    
 *          contains data to be processed. This mbuf structure is allocated by  
 *          client. Result of this request will be stored in the same mbuf and  
 *          overwritten the original data if UseDifferentSrcAndDestMbufs flag in 
 *          IxCryptoAccCtx is set to FALSE (in-place operation). Otherwise, if
 *          UseDifferentSrcAndDestMbufs flag is set to TRUE, the result will be
 *          written into destination mbuf (non in-place operation) and the 
 *          original data in this mbuf will remain unchanged.
 * @param   *pDestMbuf @ref IX_OSAL_MBUF [in] - only used if   
 *          UseDifferentSrcAndDestMbufs is TRUE. This is the buffer where the  
 *          result is written to. This mbuf structure is allocated by client.  
 *          The length of mbuf MUST be big enough to hold the result of operation.   
 *          The result of operation COULD NOT span into two or more different  
 *          mbufs, thus the mbuf supplied must be at least the length of  
 *          expected result. The data is written back starting at startOffset in 
 *          the pDestMbuf. Non-in-place operations are not therefore this 
 *          parameter is ignored for now.
 * @param   startOffset UINT16 [in] - supplied by the client to indicate 
 *          the start of the payload to be decrypted/encrypted or authenticated.
 * @param   dataLen UINT16 [in] - supplied by the client to indicate the  
 *          length of the payload to be decrypted/encrypted in number of bytes.
 * @param   icvOffset UINT16 [in] - supplied by the client to indicate  
 *          the start of the ICV (Integrity Check Value) used for the
 *          authentication. This ICV field should not be split across 
 *          multiple mbufs in a chained mbuf.
 * @param   *pKey UINT8 [in] - Pointer to IX_CRYPTO_ACC_ARC4_KEY_128 bytes of per
 *          packet ARC4 keys. This pointer can be NULL if the request is WEP IV
 *          gen or verify only.
 *          
 * @note    This function operates synchronously and will not return status
 *          until the operation is completed.
 *
 * @return @li  IX_CRYPTO_ACC_STATUS_SUCCESS - The specified operation was
 *              performed successfully.If the operation type was WEP ICV verify,
 *              then this value means the verification was success as well.
 * @return @li  IX_CRYPTO_ACC_STATUS_FAIL - The request failed because there was  
 *              a parameter inconsistency or an invalid valid operation was
 *              requested.
 * @return @li  IX_CRYPTO_ACC_STATUS_AUTH_FAIL- The specified operation was 
 *              performed, but the WEP ICV verification failed.
 * @return @li  IX_CRYPTO_ACC_STATUS_CRYPTO_CTX_NOT_VALID - Crypto context is
 *              invalid.
 * 
 * @note Statistics collection for XScale WEP perform will only be enabled in
 *       in DEBUG mode. Interrupt locking mechanism is used in protecting the
 *       the critical section for statistic collection. 
 *
 * @li   Reentrant    : yes
 * @li   ISR Callable : yes
 *
 */
PUBLIC IxCryptoAccStatus
ixCryptoAccXScaleWepPerform (
    UINT32   cryptoCtxId,
    IX_OSAL_MBUF *pSrcMbuf,
    IX_OSAL_MBUF *pDestMbuf,
    UINT16   startOffset,   
    UINT16   dataLen,
    UINT16   icvOffset,
    UINT8   *pKey);


/**
 *
 * @ingroup IxCryptoAcc
 *
 * @fn IxCryptoAccStatus ixCryptoAccCtxCipherKeyUpdate (
    UINT32 cryptoCtxId,
    UINT8 *pCipherKey)
 *
 * @brief  Change keys for the specified registered context.
 * 
 * This function is called to change the key value of a previously registered 
 * context.
 * Key change for a registered context is only supported for CCM cipher mode.
 * This is done in order to quickly change keys for CCM mode, without going
 * through the process of context deregistration and registration. 
 * Changes to the key lengths are not allowed for a registered context.
 * This function should only be used if one is invoking cryptographic operations
 * using CCM as cipher mode. For contexts registered with other modes the
 * the client should unregister and  re-register a context for the particular 
 * security association in order to change keys and other parameters.
 * The client should make sure that there are no pending requests on the 
 * "cryptoCtxtId" for the key change to happen successfully. If there are pending
 * requests on this context the result of those operations are undefined.
 *
 * @param cryptoCtxId UINT32 [in] - The previously registered context id. This  
 *        context should be registered for CCM cipher mode. Keys updates for other
 *        types of registered cipher modes are not supported at this time.
 * @param *pCipherKey UINT8 [in] - The new key value. The number of bytes expected in 
 *        keys is the "cipherKeyLen" used during the registration of the context.
 *        
 * @return @li IX_CRYPTO_ACC_STATUS_SUCCESS : Keys were changed successfully.
 * @return @li IX_CRYPTO_ACC_STATUS_CRYPTO_CTX_NOT_VALID: cryptoCtxtId is not
 *             valid.
 * @return @li IX_CRYPTO_ACC_STATUS_OPERATION_NOT_SUPPORTED: Returned when the
 *             context is not registered for doing operations on NPE C or if the
 *             operation was anything other than CCM.
 *
 * @return @li IX_CRYPTO_ACC_STATUS_FAIL: If the component has not been
 *             initialized.
 * 
 * @li   Reentrant    : yes
 * @li   ISR Callable : yes
 *
 */
PUBLIC IxCryptoAccStatus 
ixCryptoAccCtxCipherKeyUpdate (
    UINT32 cryptoCtxId,
    UINT8 *pCipherKey);


/**
 *
 * @ingroup IxCryptoAcc
 *
 * @fn void ixCryptoAccShow (void)
 *
 * @brief API for printing statistics and status.
 *
 * This function is called by the client to print statistics, such as number 
 * of packets returned with operation fail, number of packets encrypted/
 * decrypted/authenticated while the function also print the current status of 
 * the queue, whether the queue is empty or full or current queue length.
 *
 * @return None
 *          
 * @li   Reentrant    : yes
 * @li   ISR Callable : no
 *
 */
PUBLIC void
ixCryptoAccShow (void);



/**
 *
 * @ingroup IxCryptoAcc
 *
 * @fn void ixCryptoAccShowWithId (UINT32 cryptoCtxId)
 *
 * @brief API for printing statistic and status.
 *
 * This function prints all the statistics provided by ixCryptoAccShow. In
 * addition the function will print out the contents of the Crypto Context
 * corresponding to the CryptoCtxId supplied through the API
 *
 * @param   cryptoCtxId UINT32 [in] - Crypto Context ID which has been registered
 *
 * @return  None
 *          
 * @li   Reentrant    : yes
 * @li   ISR Callable : no
 *
 */
PUBLIC void
ixCryptoAccShowWithId (UINT32 cryptoCtxId); 

/**
 *
 * @ingroup IxCryptoAcc
 *
 * @fn IxCryptoAccStatus ixCryptoAccCryptoServiceStop (void)
 * 
 * @brief Function to stop the crypto services.
 *
 * This function is called to stop the Hardware Accelerator services and 
 * (if enabled) WEP engine services.
 * All the requests pending in queues will be completed before the services are
 * shutdown. Any new requests issued after this call will be 
 * rejected. All the Crypto Contexts will be unregistered in this function call. 
 *
 * @return @li  IX_CRYPTO_ACC_STATUS_SUCCESS - Operation requested is successful 
 *              with all the pending requests are completed and CCD is cleared.
 * @return @li  IX_CRYPTO_ACC_STATUS_FAIL - Cryptographic stop request failed 
 *              for some unspecified internal reasons.
 *
 * @li   Reentrant    : no
 * @li   ISR Callable : no
 *
 */
PUBLIC IxCryptoAccStatus
ixCryptoAccCryptoServiceStop (void);


/**
 *
 * @ingroup IxCryptoAcc
 *
 * @fn IxCryptoAccStatus ixCryptoAccHashKeyGenerate (
    IxCryptoAccAuthAlgo hashAlgo,
    IX_OSAL_MBUF *pMbufHashKey,
    IxCryptoAccHashKeyGenCompleteCallback hashKeyCallbackFn,
    UINT16 hashKeyStartOffset,
    UINT16 hashKeyLen,
    UINT16 hashKeyDestOffset,
    UINT32 *pHashKeyId)
 *
 * @brief This function is used to generate authentication key needed in HMAC
 *        authentication if the authentication key is greater than 64 bytes.
 *        New authentication key of L bytes size will be generated in this 
 *        function (L = 20 for SHA1, L = 16 for MD5). Please refer to RFC2104 
 *        for more details on the key size. The authentication key is padded 
 *        (extended) so that its length (in bits) is congruent to 448, 
 *        modulo 512 (please refer to RFC1321). 
 *        This function has been extended to become a generic hashing function
 *        other than above mentioned functionality. It could be used to hash 
 *        any key size or data that are greater than 0 and less than 65399 
 *        bytes. <B>In order to map to extention of functioality, this function 
 *        has been aliased to a name @ref ixCryptoAccHashPerform. </B>
 *
 * @note  
 * 1. This API will need to be called if the authentication key > 64 bytes. If
 *    key size is >=L and <= 64 bytes, authentication key is used directly in 
 *    Crypto context for registration, no further hashing is needed.
 * 2. This function must be called prior the Crypto Context registration if 
 *    the key size > 64. Result from this function will be used as 
 *    authentication key in the Crypto Context registration.
 * 3. It is client's responsibility to ensure the mbuf is big enough to
 *    hold the result (generated authentication key). No checking on mbuf 
 *    length will be done.
 *
 * @param   hashAlgo @ref IxCryptoAccAuthAlgo [in] - is hashing algorithm to be 
 *          used in generating the hash key. hashAlgo used to generate the key 
 *          must be the same algorithm that will be used in crypto register and 
 *          crypto perform services.
 * @param   *pMbufHashKey @ref IX_OSAL_MBUF [in] - a pointer to an mbuf that  
 *          contains the authentication key to be hashed. This mbuf structure is
 *          allocated by the client and the mbuf pointer must not be NULL. 
 *          After the NPE complete the computation, the mbuf is returned
 *          separately through the client's registered callback. The 
 *          authentication key generated will be stored in this mbuf, pointed  
 *          by hashKeyDestOffset. This must NOT be a chained mbuf.
 * @param   hashKeyCallbackFn @ref IxCryptoAccHashKeyGenCompleteCallback [in] - 
 *          callback function pointer to be called when the hash key generation 
 *          operation is completed. This cannot be NULL.
 * @param   hashKeyStartOffset UINT16 [in] - offset to the mbuf mdata which contain
 *          the original hash key.
 * @param   hashKeyLen UINT16 [in] - key size
 * @param   hashKeyDestOffset UINT16 [in] - offset to the mbuf mdata to store the 
 *          generated result (authentication key)
 * @param   *pHashKeyId UINT32 [inout] - Hash Key ID returned by access component
 *          to identify hash key generation. 
 *
 * @return @li  IX_CRYPTO_ACC_STATUS_SUCCESS - Hash key generation parameters
 *              are valid
 * @return @li  IX_CRYPTO_ACC_STATUS_FAIL - Hash key generation failed.
 * @return @li  IX_CRYPTO_ACC_STATUS_AUTH_ALGO_NOT_SUPPORTED - Invalid 
 *              hash algorithm  requested by the client.
 * @return @li  IX_CRYPTO_ACC_STATUS_AUTH_INVALID_KEY_LEN - Invalid 
 *              hash key length (0 >= hashKeyLen or 65399 < hashKeyLen)
 * @return @li  IX_CRYPTO_ACC_STATUS_QUEUE_FULL - Queue full status returned 
 *              and the authentication key generation request will be rejected.
 *
 * @li   Reentrant    : yes
 * @li   ISR Callable : no
 *
 */ 
PUBLIC IxCryptoAccStatus
ixCryptoAccHashKeyGenerate (
    IxCryptoAccAuthAlgo hashAlgo,
    IX_OSAL_MBUF *pMbufHashKey,
    IxCryptoAccHashKeyGenCompleteCallback hashKeyCallbackFn,
    UINT16 hashKeyStartOffset,
    UINT16 hashKeyLen,
    UINT16 hashKeyDestOffset,
    UINT32 *pHashKeyId);

/**
 *
 * @ingroup IxCryptoAcc
 *
 * @fn IxCryptoAccStatus ixCryptoAccHashPerform (
    IxCryptoAccAuthAlgo hashAlgo,
    IX_OSAL_MBUF *pMbufHashKey,
    IxCryptoAccHashKeyGenCompleteCallback hashKeyCallbackFn,
    UINT16 hashKeyStartOffset,
    UINT16 hashKeyLen,
    UINT16 hashKeyDestOffset,
    UINT32 *pHashKeyId)
 *
 * @brief Alias name to ixCryptoAccHashKeyGenerate function
 */
#define ixCryptoAccHashPerform ixCryptoAccHashKeyGenerate 
                                /**< alias function name to 
                                 * @ref ixCryptoAccHashKeyGenerate 
                                 */
             
#endif /* ndef IXCRYPTOACC_H */
/**
 *@}
 */
