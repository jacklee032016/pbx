/**
 * @file ethUtil.c
 *
 * @brief Utility functions
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


#include "IxFeatureCtrl.h"
#include "IxEthDB_p.h"

IX_ETH_DB_PUBLIC
IxEthDBStatus ixEthDBSingleEthNpeCheck(IxEthDBPortId portID)
{
    /* If not IXP42X A0 stepping, proceed to check for existence of coprocessors */ 
    if ((IX_FEATURE_CTRL_SILICON_TYPE_A0 != 
        (ixFeatureCtrlProductIdRead() & IX_FEATURE_CTRL_SILICON_STEPPING_MASK))
        || (IX_FEATURE_CTRL_DEVICE_TYPE_IXP42X != ixFeatureCtrlDeviceRead ()))
    {  
        if ((portID == 0) && 
            (ixFeatureCtrlComponentCheck(IX_FEATURECTRL_ETH0) ==
             IX_FEATURE_CTRL_COMPONENT_DISABLED))
        {
	    return IX_ETH_DB_FAIL;
        }

        if ((portID == 1) &&
            (ixFeatureCtrlComponentCheck(IX_FEATURECTRL_ETH1) ==
             IX_FEATURE_CTRL_COMPONENT_DISABLED))
        {
	    return IX_ETH_DB_FAIL;	    
        }

        if ((portID == 2) &&
            (ixFeatureCtrlComponentCheck(IX_FEATURECTRL_NPEA_ETH) ==
             IX_FEATURE_CTRL_COMPONENT_DISABLED))
        {
	    return IX_ETH_DB_FAIL;	    
        }
    }
    
    return IX_ETH_DB_SUCCESS;
}

IX_ETH_DB_PUBLIC
BOOL ixEthDBCheckSingleBitValue(UINT32 value)
{
#if (CPU != SIMSPARCSOLARIS) && !defined (__wince)
    UINT32 shift;
    
    /* use the count-leading-zeros XScale instruction */
    __asm__ ("clz %0, %1\n" : "=r" (shift) : "r" (value));
    
    return ((value << shift) == 0x80000000UL);
    
#else
	
    while (value != 0)
    {
        if (value == 1) return TRUE;
        else if ((value & 1) == 1) return FALSE;

        value >>= 1;
    }
    
    return FALSE;

#endif
}

const char *mac2string(const unsigned char *mac)
{
  static char str[19];
  
  if (mac == NULL)
  {
    return NULL;
  }  

  sprintf(str, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

  return str;
}
