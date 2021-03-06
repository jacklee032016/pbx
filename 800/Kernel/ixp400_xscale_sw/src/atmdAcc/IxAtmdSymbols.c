/**
 * @file IxAtmdSymbols.c
 *
 * @author Intel Corporation
 * @date 04-Oct-2002
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

#include <linux/module.h>
#include <IxAtmdAcc.h>
#include <IxAtmdAccCtrl.h>

EXPORT_SYMBOL(ixAtmdAccRxDispatcherRegister);
EXPORT_SYMBOL(ixAtmdAccRxDispatch);
EXPORT_SYMBOL(ixAtmdAccRxLevelQuery);
EXPORT_SYMBOL(ixAtmdAccRxQueueSizeQuery);
EXPORT_SYMBOL(ixAtmdAccPortTxFreeEntriesQuery);
EXPORT_SYMBOL(ixAtmdAccPortTxCallbackRegister);
EXPORT_SYMBOL(ixAtmdAccPortTxScheduledModeEnable);
EXPORT_SYMBOL(ixAtmdAccPortTxProcess);
EXPORT_SYMBOL(ixAtmdAccTxDoneDispatch);
EXPORT_SYMBOL(ixAtmdAccTxDoneLevelQuery);
EXPORT_SYMBOL(ixAtmdAccTxDoneQueueSizeQuery);
EXPORT_SYMBOL(ixAtmdAccTxDoneDispatcherRegister);
EXPORT_SYMBOL(ixAtmdAccUtopiaConfigSet);
EXPORT_SYMBOL(ixAtmdAccUtopiaStatusGet);
EXPORT_SYMBOL(ixAtmdAccPortEnable);
EXPORT_SYMBOL(ixAtmdAccPortDisable);
EXPORT_SYMBOL(ixAtmdAccPortDisableComplete);

EXPORT_SYMBOL(ixAtmdAccInit);
EXPORT_SYMBOL(ixAtmdAccShow);
EXPORT_SYMBOL(ixAtmdAccStatsShow);
EXPORT_SYMBOL(ixAtmdAccStatsReset);
EXPORT_SYMBOL(ixAtmdAccRxVcConnect);
EXPORT_SYMBOL(ixAtmdAccRxVcFreeReplenish);
EXPORT_SYMBOL(ixAtmdAccRxVcFreeLowCallbackRegister);
EXPORT_SYMBOL(ixAtmdAccRxVcFreeEntriesQuery);
EXPORT_SYMBOL(ixAtmdAccRxVcEnable);
EXPORT_SYMBOL(ixAtmdAccRxVcDisable);
EXPORT_SYMBOL(ixAtmdAccRxVcTryDisconnect);
EXPORT_SYMBOL(ixAtmdAccTxVcConnect);
EXPORT_SYMBOL(ixAtmdAccTxVcPduSubmit);
EXPORT_SYMBOL(ixAtmdAccTxVcTryDisconnect);

#endif /* __linux */
