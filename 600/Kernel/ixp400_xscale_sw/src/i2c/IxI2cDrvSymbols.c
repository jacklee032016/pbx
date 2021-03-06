/**
 * @file IxFeatureCtrlSymbols.c
 *
 * @author Intel Corporation
 * @date 29-Jan-2003
 *
 * @brief This file declares exported symbols for linux kernel module builds.
 *
 * @version $Revision: 1.1.1.1 $
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

#if __linux
#ifdef __ixp46X
#include <linux/module.h>
#include "IxI2cDrv.h"

EXPORT_SYMBOL(ixI2cDrvInit);
EXPORT_SYMBOL(ixI2cDrvUninit);
EXPORT_SYMBOL(ixI2cDrvSlaveAddrSet);
EXPORT_SYMBOL(ixI2cDrvBusScan);
EXPORT_SYMBOL(ixI2cDrvWriteTransfer);
EXPORT_SYMBOL(ixI2cDrvReadTransfer);
EXPORT_SYMBOL(ixI2cDrvSlaveAddrAndGenCallDetectedCheck);
EXPORT_SYMBOL(ixI2cDrvSlaveOrGenDataReceive);
EXPORT_SYMBOL(ixI2cDrvSlaveDataTransmit);
EXPORT_SYMBOL(ixI2cDrvSlaveOrGenCallBufReplenish);
EXPORT_SYMBOL(ixI2cDrvStatsGet);
EXPORT_SYMBOL(ixI2cDrvStatsReset);
EXPORT_SYMBOL(ixI2cDrvShow);
EXPORT_SYMBOL(ixI2cDrvDelayTypeSelect);
#endif /* __ixp46X */
#endif /* __linux */
