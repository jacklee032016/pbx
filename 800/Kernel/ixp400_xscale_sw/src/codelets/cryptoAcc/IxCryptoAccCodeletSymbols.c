/**
 * @file IxCryptoAccCodeletSymbols.c
 *
 * @date 23-Apr-2003
 *
 * @brief This file declares exported symbols for linux kernel module builds.
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

#ifdef __linux

#include <linux/config.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <asm/semaphore.h>

#include "IxTypes.h"
#include "IxCryptoAccCodelet.h"

EXPORT_SYMBOL(ixCryptoAccCodeletMain);

UINT32 serviceIndex;
UINT32 packetLen;


MODULE_PARM(serviceIndex, "i");
MODULE_PARM_DESC(serviceIndex,
                 "0 : Lists all set of services demonstrated."
                 "1 : Encryption and Decryption using DES(ECB)"
                 "2 : Encryption and Decryption using DES(CBC)"
                 "3 : Encryption and Decryption using 3DES(ECB)"
                 "4 : Encryption and Decryption using 3DES(CBC)"
                 "5 : Encryption and Decryption using AES(ECB)"
                 "6 : Encryption and Decryption using AES(CBC)"
                 "7 : Encryption and Decryption using AES(CTR)"
                 "8 : Encryption and Decryption using AES-CCM"
                 "9 : Encryption and Decryption using ARC4 on XScale"
                 "10 : Encryption and Decryption using ARC4 on WAN-NPE"
                 "11 : Authentication calculation and verification using MD5"
                 "12 : Authentication calculation and verification using SHA1"
                 "13 : Authentication calculation and verification WEP CRC on XScale"
                 "14 : Authentication calculation and verification WEP CRC on WAN-NPE"
                 "15 : A combined mode of operation using DES(ECB) + MD5"
                 "16 : A combined mode of operation using DES(CBC) + MD5"
                 "17 : A combined mode of operation using DES(ECB) + SHA1"
                 "18 : A combined mode of operation using DES(CBC) + SHA1"
                 "19 : A combined mode of operation using 3DES(ECB) + MD5"
                 "20 : A combined mode of operation using 3DES(CBC) + MD5"
                 "21 : A combined mode of operation using 3DES(ECB) + SHA1"
                 "22 : A combined mode of operation using 3DES(CBC) + SHA1"
                 "23 : A combined mode of operation using AES(ECB) + MD5"
                 "24 : A combined mode of operation using AES(CBC) + MD5"
                 "25 : A combined mode of operation using AES(CTR) + MD5"
                 "26 : A combined mode of operation using AES(ECB) + SHA1"
                 "27 : A combined mode of operation using AES(CBC) + SHA1"
                 "28 : A combined mode of operation using AES(CTR) + SHA1"
                 "29 : A combined mode of operation using ARC4 + WEP CRC on XScale"
                 "30 : A combined mode of operation using ARC4 + WEP CRC on WAN-NPE");

MODULE_PARM(packetLen, "i");
MODULE_PARM_DESC(packetLen,
         "Packet length ranges from 64 bytes to 65456 bytes, if cipher\n"
         "algorithm is DES/3DES, packet length must be multiple of 8-byte\n"
         "(cipher block length); while AES algorithm must have packet length\n"
         "that is multiple of 16-bytecin size.\n");
         

static int __init CryptoInitModule(void)
{
    printk("Loading CryptoAcc Codelet...\n");
    if (IX_SUCCESS != ixCryptoAccCodeletMain (serviceIndex, packetLen))
        return -1;
    else
        return 0;
}

static void __exit CryptoExitModule(void)
{
    printk("Unloading CryptoAcc Codelet\n");
    ixCryptoAccCodeletMain (-1, 0);
}

module_init(CryptoInitModule);
module_exit(CryptoExitModule);


#endif
