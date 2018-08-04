/**
 * @file IxAtmdRxCfgIf_p.h
 *
 * @author Intel Corporation
 * @date 17 March 2002
 *
 * @brief IxAtmdAcc Rx Control
 *
 * This file defines the error counters used in the Atmd Rx code
 * and the initialisation functions
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

#ifndef IXATMDRXCFGIF_H
#define IXATMDRXCFGIF_H

#include "IxOsalTypes.h"

/**
* @brief RxCfgIf initialisation function
*
* @note - this function should be used once
*
* @return @li IX_STATUS
*/
IX_STATUS ixAtmdAccRxCfgIfInit (void);

/**
* @brief RxCfgIf stats display function
*
* @return none
*/
void ixAtmdAccRxCfgIfStatsShow (void);

/**
* @brief RxCfgIf stats reset function
*
* @return none
*/
void ixAtmdAccRxCfgIfStatsReset (void);

/**
* @brief  display Rx configuration
*
* @return none
*/
void
ixAtmdAccRxCfgIfChannelShow (IxAtmLogicalPort port);

#endif /* IXATMDRXCFGIF_H */


