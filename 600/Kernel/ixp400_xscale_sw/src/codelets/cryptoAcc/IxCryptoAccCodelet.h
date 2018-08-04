/**
 * @file IxCryptoAccCodelet.h
 *
 * @date October-31-2002
 *
 * @brief This is the header file for the Crypto Access Component Codelet.
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
 * @defgroup Codelets IXP400 Codelets
 *
 * @brief IXP400 codelets
 *
 * @{
 */

/**
 * @ingroup Codelets
 *
 * @defgroup IxCryptoAccCodelet IXP400 Crypto Access Codelet
 *           (IxCryptoAccCodelet) API
 *
 * @brief IXP400 Crypto Access Codelet API
 *
 * The codelet demonstrates how the Security Hardware Accelerator API
 * can be used for performing various cryptographic operations.
 * For demonstration purposes, various cryptographic operations (called services
 * in this codelet) are preconfigured in an array. Users can specify
 * a particular service with a "service index" from the list below. 
 * Various services shown are:
 *
 * <PRE>
 *    - Encryption and decryption, using DES ECB
 *    - Authentication calculation and check, using MD5
 *    - Combined encryption and decryption, using DES CBC and SHA1
 *    - CCM for 802.11i
 *    - ARC4 with WEP CRC computations on XScale
 *    - ARC4 with WEP CRC computations on NPE.
 * </PRE>
 *
 * The codelet shows how to initialize the required components (like Qmgr, NPE's)
 * and the Security Hardware Acclerator. This initialization
 * is done only once for the very first request. The components remain enabled
 * till "codelet stop" is requested. Every invocation of the codelet
 * for a specific service results in the context registration followed by
 * execution of the operation. Once the operation completes, the contexts 
 * are un-registered. If the operation completes successfully performance 
 * statistics are printed. If there is an error, statistics are not printed
 * and an error message is displayed.
 * The codelet shows the following :
 *
 * <PRE>
 *       - The codelet demonstrates how the Network Processor Engine (NPE) 
 * is initialised, how the NPE image is downloaded to the NPE, and 
 * and how the NPE is started.
 *
 *       - The codelet demonstrates how the Queue Manager is initialised to
 * polling or interrupt mode.
 *
 *       - The codelet shows how to set up the callback functions and 
 * how to register crypto contexts with cryptoAcc access component.
 *
 *       - The codelet demonstrates how the registration should be done for
 * the different operations using the register API.
 *
 *       - The codelet demonstrates how the perform API can be used after
 * successful registration.
 *
 *       - The codelet demonstrates how to use XScaleWepPerform function
 * and NpeWepPerform function
 *
 *       - The codelet demonstrates the use of the unregister API in the
 * event of re-starting the above mentioned operation.
 *
 *       - The codelet measures the performance of the Security Hardware
 * Accelerator for each of the operations listed above.
 * 
 * </PRE>
 * 
 * Configuration parameters for Performance benchmarking are
 *
 * <B>Default Performance Sampling Size</B>
 * <PRE>
 *    - Packets per Sampling     : 1,000
 *    - Number of Samples        : 20
 *    - Total Packets            : 20,000
 * </PRE>
 *
 * <B>Default mbuf Pool Setting</B>
 * <PRE>
 *    - Number of mbufs in pool     : 20
 *    - Min of mbufs needed in pool : 2 (for crypto context registration 
 *                                    purpose)
 * </PRE>
 *
 * User may choose to run different operation with different packet lengths. 
 * The performance rate of selected operation with selected packet length 
 * will be displayed after throughput rate has been captured for a predefined 
 * performance sampling size.If an error occurs any time in the codelet, the  
 * performance numbers will not be displayed and error message will be
 * printed.
 *
 * <B>Notes for Performance Benchmarking:</B>
 * <PRE>
 *       - Packets are generated in XScale with random payload. No explicit
 * verification of payload will be carried out in the codelet.
 *
 *       - Encrypted packets are sent to NPE again for decryption and same
 * applies to authentication operation. Packet payload is verified implicitly
 * through this feedback system.
 *
 *       - Throughput rate is captured from the point where packet is sent to
 * NPE for processing to the point where NPE completes the encryption and/or
 * authentication operation and notifies XScale through callback.
 *
 *       - No external packet generator or benchmarking devices involved in this
 * codelet. Throughput rate is captured using XScale timestamp.
 *
 *       - Time taken for codelet to complete the operation depends on packet
 * length selected. Larger packet will take more time to complete.
 * </PRE>
 *
 * <B> VxWorks User Guide </B><BR>
 * ixCryptoAccCodeletMain() function is used as the enry point of execution
 * for cryptoAcc Codelet. <BR>
 * It allows user to enter selection for various operations described below
 * with different packet length.
 *
 * <PRE>
 *  <I> Usage :
 *      >ixCryptoAccCodeletMain (serviceIndex, packetLen)
 *      Where serviceIndex should be one of the following:
 *
 * 	         -1 : Stops the codelet.
 *            0 : Lists the set of services demonstrated.
 *            1 : Encryption and Decryption using DES(ECB)
 *            2 : Encryption and Decryption using DES(CBC)
 *            3 : Encryption and Decryption using 3DES(ECB)
 *            4 : Encryption and Decryption using 3DES(CBC)
 *            5 : Encryption and Decryption using AES(ECB)
 *            6 : Encryption and Decryption using AES(CBC)
 *            7 : Encryption and Decryption using AES(CTR)
 *            8 : Encryption and Decryption using AES-CCM
 *            9 : Encryption and Decryption using ARC4 on XScale
 *            10 : Encryption and Decryption using ARC4 on WAN-NPE
 *            11 : Authentication calculation and verification using MD5
 *            12 : Authentication calculation and verification using SHA1
 *            13 : Authentication calculation and verification WEP CRC on XScale
 *            14 : Authentication calculation and verification WEP CRC on WAN-NPE
 *            15 : A combined mode of operation using DES(ECB) + MD5
 *            16 : A combined mode of operation using DES(CBC) + MD5
 *            17 : A combined mode of operation using DES(ECB) + SHA1
 *            18 : A combined mode of operation using DES(CBC) + SHA1
 *            19 : A combined mode of operation using 3DES(ECB) + MD5
 *            20 : A combined mode of operation using 3DES(CBC) + MD5
 *            21 : A combined mode of operation using 3DES(ECB) + SHA1
 *            22 : A combined mode of operation using 3DES(CBC) + SHA1
 *            23 : A combined mode of operation using AES(ECB) + MD5
 *            24 : A combined mode of operation using AES(CBC) + MD5
 *            25 : A combined mode of operation using AES(CTR) + MD5
 *            26 : A combined mode of operation using AES(ECB) + SHA1
 *            27 : A combined mode of operation using AES(CBC) + SHA1
 *            28 : A combined mode of operation using AES(CTR) + SHA1
 *            29 : A combined mode of operation using ARC4 + WEP CRC on XScale
 *            30 : A combined mode of operation using ARC4 + WEP CRC on WAN-NPE
 *
 *      Where packetLen:
 *           Packet length ranges from 64 bytes to 65456 bytes, if cipher
 *           algorithm is DES/3DES, packet length must be multiple of 8-byte
 *           (cipher block length); while AES algorithm must have packet length
 *           that is multiple of 16-byte in size. For ARC4 algorithm block length
 *           there is no such restriction (because block size is 1 byte).
 *           If cipher mode is CCM, there is no restriction on the packet length
 *           being a multiple of cipher block length.
 *
 * </I>
 * </PRE>
 *
 * <B> Linux User Guide </B><BR>
 * The ixCryptoAccCodeletMain() is the entry point of cryptoAcc codelet. <BR>
 * The selected operation will be executed when user issues 'insmod' at command
 * prompt.
 *
 * <PRE>
 * <I>  Usage :
 *      >insmod ixp400_codelets_cryptoAcc.o serviceIndex=<x> packetLen=<y>
 *      Where x should be one of the following numbers:
 *
 *           -1 : Stops the codelet.
 *             0 : Lists all set of services demonstrated.
 *            1 : Encryption and Decryption using DES(ECB)
 *            2 : Encryption and Decryption using DES(CBC)
 *            3 : Encryption and Decryption using 3DES(ECB)
 *            4 : Encryption and Decryption using 3DES(CBC)
 *            5 : Encryption and Decryption using AES(ECB)
 *            6 : Encryption and Decryption using AES(CBC)
 *            7 : Encryption and Decryption using AES(CTR)
 *            8 : Encryption and Decryption using AES-CCM
 *            9 : Encryption and Decryption using ARC4 on XScale
 *            10 : Encryption and Decryption using ARC4 on WAN-NPE
 *            11 : Authentication calculation and verification using MD5
 *            12 : Authentication calculation and verification using SHA1
 *            13 : Authentication calculation and verification WEP CRC on XScale
 *            14 : Authentication calculation and verification WEP CRC on WAN-NPE
 *            15 : A combined mode of operation using DES(ECB) + MD5
 *            16 : A combined mode of operation using DES(CBC) + MD5
 *            17 : A combined mode of operation using DES(ECB) + SHA1
 *            18 : A combined mode of operation using DES(CBC) + SHA1
 *            19 : A combined mode of operation using 3DES(ECB) + MD5
 *            20 : A combined mode of operation using 3DES(CBC) + MD5
 *            21 : A combined mode of operation using 3DES(ECB) + SHA1
 *            22 : A combined mode of operation using 3DES(CBC) + SHA1
 *            23 : A combined mode of operation using AES(ECB) + MD5
 *            24 : A combined mode of operation using AES(CBC) + MD5
 *            25 : A combined mode of operation using AES(CTR) + MD5
 *            26 : A combined mode of operation using AES(ECB) + SHA1
 *            27 : A combined mode of operation using AES(CBC) + SHA1
 *            28 : A combined mode of operation using AES(CTR) + SHA1
 *            29 : A combined mode of operation using ARC4 + WEP CRC on XScale
 *            30 : A combined mode of operation using ARC4 + WEP CRC on WAN-NPE
 *
 *   Where y:
 *           Packet length ranges from 64 bytes to 65456 bytes, if cipher
 *           algorithm is DES/3DES, packet length must be multiple of 8-byte
 *           (cipher block length); while AES algorithm must have packet length
 *           that is multiple of 16-byte in size. For ARC4 algorithm block length
 *           there is no such restriction (because block size is 1 byte).
 *           If cipher mode is CCM, there is no restriction on the packet length   
 *           being a multiple of cipher block length.                     
 *
 * </I>
 * </PRE>
 *
 * @{
 */
#ifndef IxCryptoAccCodelet_H
#define IxCryptoAccCodelet_H

#include "IxOsal.h"
#include "IxNpeDl.h"

#define INLINE __inline__

/**************************************************************************
 *        USER SETTING - CryptoAcc Configuration and NPE BUILD ID's.
 **************************************************************************/

/**
 * @ingroup IxCryptoAccCodelet
 *
 * @def  IX_CRYPTOACC_CODELET_CRYPTOACC_CFG
 *
 * @brief Selects the interface's (to the NPE) to be initialized by the codelet.
 *        Use one of the values defined in IxCryptoAccCfg.
 *        IX_CRYPTO_ACC_CFG_CRYPTO_WEP_NPE_ACC_EN: Enable's access to the Crypto
 *        NPE and WEP NPE.
 */
#define IX_CRYPTOACC_CODELET_CRYPTOACC_CFG  (IX_CRYPTO_ACC_CFG_CRYPTO_WEP_NPE_ACC_EN)


/**
 * @ingroup IxCryptoAccCodelet
 *
 * @def IX_CRYPTOACC_CODELET_NPEC_IMAGE_ID
 *
 * @brief The build ID for NPE C image, default is
 *        IX_NPEDL_NPEIMAGE_NPEC_CRYPTO_AES_CCM_ETH 
 *
 * @note Please refer to IxNpeDl.h for NPE image definition
 *
 */
#define IX_CRYPTOACC_CODELET_NPEC_IMAGE_ID IX_NPEDL_NPEIMAGE_NPEC_CRYPTO_AES_CCM_ETH


/**
 * @ingroup IxCryptoAccCodelet
 *
 * @def IX_CRYPTOACC_CODELET_NPEA_IMAGE_ID
 * 
 * @brief The build ID for NPE A image, default is IX_NPEDL_NPEIMAGE_NPEA_WEP
 *
 * @note Please refer to IxNpeDl.h for NPE image definition
 *
 */
#define IX_CRYPTOACC_CODELET_NPEA_IMAGE_ID  IX_NPEDL_NPEIMAGE_NPEA_WEP


/**************************************************************************
 *                 PERFORMANCE BENCHMARKING SETTING
 **************************************************************************/

/**
 * @ingroup IxCryptoAccCodelet
 *
 * @def PERFORMANCE_WINDOW_SIZE
 *
 * @brief Number of crypto packets need to be sent in for performance 
 *        benchmarking (sampling size), 1 unit represents 1000 of packets 
 *
 */
#define PERFORMANCE_WINDOW_SIZE                        20

/**
 * @ingroup IxCryptoAccCodelet
 *
 * @def IX_CRYPTOACC_CODELET_CRYPTO_MBUF_POOL_SIZE
 *
 * @brief Size of MBuf pool, ie number of buffers to circulate 
 *
 */
#define IX_CRYPTOACC_CODELET_CRYPTO_MBUF_POOL_SIZE     20


/**************************************************************************
*                           SYSTEM SETTING 
**************************************************************************/

/**
 * @ingroup IxCryptoAccCodelet
 *  
 * @def   IX_CRYPTOACC_CODELET_BATCH_LEN
 *
 * @brief Number of packets per sampling for performance benchmarking 
 *
 */
#define IX_CRYPTOACC_CODELET_BATCH_LEN              1000

/**
 * @ingroup IxCryptoAccCodelet
 * 
 * @def   IX_CRYPTOACC_CODELET_QMGR_DISPATCHER_PRIORITY
 *
 * @brief Recommended priority of queue manager dispatch loop 
 *
 */
#define IX_CRYPTOACC_CODELET_QMGR_DISPATCHER_PRIORITY    150 

/**
 * @ingroup IxCryptoAccCodelet
 * 
 * @def   IX_CRYPTOACC_CODELET_QMGR_DISPATCHER_THREAD_STACK_SIZE
 *
 * @brief Recommended stack size for queue manager dispatcher thread 
 *
 */
#define IX_CRYPTOACC_CODELET_QMGR_DISPATCHER_THREAD_STACK_SIZE 10240

/**
 * @ingroup IxCryptoAccCodelet
 *
 * @def   IX_CRYPTOACC_CODELET_XSCALE_TICK
 *
 * @brief The XScale tick is 66 MHz
 *
 */
#define IX_CRYPTOACC_CODELET_XSCALE_TICK            66

/**
 * @ingroup IxCryptoAccCodelet
 * 
 * @def   IX_CRYPTOACC_CODELET_REGISTER_WAIT_TIME
 *
 * @brief The codelet register wait time (in ms) in a for loop 
 *
 */
#define IX_CRYPTOACC_CODELET_REGISTER_WAIT_TIME     500

/**
 * @ingroup IxCryptoAccCodelet
 *
 * @def   IX_CRYPTOACC_CODELET_PERFORM_WAIT_TIME
 * 
 * @brief The codelet perform wait time (in ms) in a for loop 
 *
 */
#define IX_CRYPTOACC_CODELET_PERFORM_WAIT_TIME      500

/**
 * @ingroup IxCryptoAccCodelet
 * 
 * @def   IX_CRYPTOACC_CODELET_MAX_TIME_WAIT
 *
 * @brief Maximum time (in ms) to wait before exiting the program
 *
 */
#define IX_CRYPTOACC_CODELET_MAX_TIME_WAIT          200

/**
 * @ingroup IxCryptoAccCodelet
 *
 * @def   IX_CRYPTOACC_CODELET_MIN_CRYPTO_PKT_SIZE
 *
 * @brief The minimum packet size 
 *
 */
#define IX_CRYPTOACC_CODELET_MIN_CRYPTO_PKT_SIZE    64

/**
 * @ingroup IxCryptoAccCodelet
 *
 * @def   IX_CRYPTOACC_CODELET_MAX_CRYPTO_PKT_SIZE
 *
 * @brief The maximum packet size 
 *
 */
#define IX_CRYPTOACC_CODELET_MAX_CRYPTO_PKT_SIZE    65456

/**
 * @ingroup IxCryptoAccCodelet
 *
 * @def IX_CRYPTOACC_CODELET_QMGR_DISPATCHER_MODE
 * 
 * @brief QMgr Dispatcher Mode, whether in interrupt (TRUE) or poll (FALSE) mode
 *
 * Note: <BR>
 * 1. QMgr dispatcher can be run in both poll mode and interrupt mode in 
 *    vxWorks platform. When QMgr dispatcher is running in interrupt mode, the 
 *    display of packet rate for Ethernet software loopback on the windshell
 *    is very slow if the crypto packet length chosen is very small. This is
 *    due to the time taken by NPE to service crypto packet is very fast and 
 *    QMgr callbacks with higher priority are triggered frequently and blocked 
 *    the task of displaying.
 *
 * 2. In linux platform, QMgr Dispatcher is NOT advisible to run in 
 *    poll mode. This is due to the task scheduling in poll mode is not fair 
 *    enough between multiple threads that cause the performance rate to drop 
 *    tremendously. Thus the performance rate displayed in Linux platform  
 *    using poll mode is not accurate. Performance rate is more accurate when
 *    QMgr dispatcher is running in interrupt mode.
 */
#ifdef __linux
/* use interrupts for performances */
#define IX_CRYPTOACC_CODELET_QMGR_DISPATCHER_MODE          (TRUE)
#elif __wince
#define IX_CRYPTOACC_CODELET_QMGR_DISPATCHER_MODE          (TRUE)
#else
/* use polled mode for performances */
#define IX_CRYPTOACC_CODELET_QMGR_DISPATCHER_MODE          (FALSE)
#endif


/**************************************************************************
 *                           Function Prototype 
 **************************************************************************/
        
/**
 * @ingroup IxCryptoAccCodelet
 *
 * @fn ixCryptoAccCodeletMain (
           INT32  srvIndex,
           UINT32 packetLen)
 *
 * @brief  This is the entry point function to the codelet to choose the 
 *         service for the codelet and packet length to be used.
 *         This is the main function of the codelet where crypto contexts
 *         registration and crypto perform services are done. Based on the
 *         selected operation and selected packet length, packets are sent to
 *         cryptoAcc for processing.
 *
 * @param srvIndex INT32 [in] - type of service to be invoked
 *                   choose from one of the following pre-configured values.
 * <PRE>
 *      -1 : Stops the codelet.
 *       0 : Lists all set of services demonstrated.
 *       1 : Encryption and Decryption using DES(ECB)
 *       2 : Encryption and Decryption using DES(CBC)
 *       3 : Encryption and Decryption using 3DES(ECB)
 *       4 : Encryption and Decryption using 3DES(CBC)
 *       5 : Encryption and Decryption using AES(ECB)
 *       6 : Encryption and Decryption using AES(CBC)
 *       7 : Encryption and Decryption using AES(CTR)
 *       8 : Encryption and Decryption using AES-CCM
 *       9 : Encryption and Decryption using ARC4 on XScale
 *       10 : Encryption and Decryption using ARC4 on WAN-NPE
 *       11 : Authentication calculation and verification using MD5
 *       12 : Authentication calculation and verification using SHA1
 *       13 : Authentication calculation and verification WEP CRC on XScale
 *       14 : Authentication calculation and verification WEP CRC on WAN-NPE
 *       15 : A combined mode of operation using DES(ECB) + MD5
 *       16 : A combined mode of operation using DES(CBC) + MD5
 *       17 : A combined mode of operation using DES(ECB) + SHA1
 *       18 : A combined mode of operation using DES(CBC) + SHA1
 *       19 : A combined mode of operation using 3DES(ECB) + MD5
 *       20 : A combined mode of operation using 3DES(CBC) + MD5
 *       21 : A combined mode of operation using 3DES(ECB) + SHA1
 *       22 : A combined mode of operation using 3DES(CBC) + SHA1
 *       23 : A combined mode of operation using AES(ECB) + MD5
 *       24 : A combined mode of operation using AES(CBC) + MD5
 *       25 : A combined mode of operation using AES(CTR) + MD5
 *       26 : A combined mode of operation using AES(ECB) + SHA1
 *       27 : A combined mode of operation using AES(CBC) + SHA1
 *       28 : A combined mode of operation using AES(CTR) + SHA1
 *       29 : A combined mode of operation using ARC4 + WEP CRC on XScale
 *       30 : A combined mode of operation using ARC4 + WEP CRC on WAN-NPE
 * </PRE>
 * @param packetLen UINT32 [in] - packet length
 *
 * @return  IX_STATUS
 *          @li IX_SUCCESS - codelet runs successfully
 *          @li IX_FAIL    - codelet fails
 */
IX_STATUS ixCryptoAccCodeletMain (
               INT32  srvIndex,
               UINT32 packetLen);

#endif /* IxCryptoAccCodelet_H */

/** @} defgroup IxCryptoAccCodelet*/

/** @} defgroup Codelet*/

