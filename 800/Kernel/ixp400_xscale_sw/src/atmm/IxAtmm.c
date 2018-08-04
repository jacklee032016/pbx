/** 
 * @file    IxAtmm.c
 * @author Intel Corporation
 * @date    13-SEP-2000
 *
 * @brief   API of the IXP425 demo ATM Manager component (IxAtmm)
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
 * Put the user defined include files required
 */
#include "IxOsal.h"
#include "IxAtmTypes.h"
#include "IxAtmm.h"
#include "IxAtmSch.h"
#include "IxAtmmUtopiaCfg_p.h"
#include "IxAtmmDataPath_p.h"
#include "IxFeatureCtrl.h"

/*
 * #defines and macros used in this file.
 */
#define IX_ATMM_MAX_NUM_VC_CHANGE_CBS   32
#define IX_ATMM_MAX_LOOPBACK_PORTS  1

#define IX_ATMM_BITS_PER_BYTE       8
#define IX_ATMM_BYTES_PER_ATM_CELL  53
#define IX_ATMM_BITS_PER_ATM_CELL   (IX_ATMM_BYTES_PER_ATM_CELL * IX_ATMM_BITS_PER_BYTE)

#define IX_ATMM_MIN_SCH_TABLE_CELLS 5

#define IX_ATMM_MAX_RX_TX_VCS  (IX_ATM_MAX_NUM_AAL_OAM_TX_VCS + IX_ATM_MAX_NUM_AAL_OAM_RX_VCS)

#define IX_ATMM_MAX_4UTOPIA_PORTS  4
#define IX_ATMM_MAX_8UTOPIA_PORTS  8
#define IX_ATMM_MAX_16UTOPIA_PORTS 16
#define IX_ATMM_MAX_24UTOPIA_PORTS 24

/*
 * Mutex macros
 */
#define IX_ATMM_LOCK()      (void)ixOsalMutexLock (&ixAtmmMutex, IX_OSAL_WAIT_FOREVER)
#define IX_ATMM_UNLOCK()    (void)ixOsalMutexUnlock (&ixAtmmMutex)
/*
 * Typedefs whose scope is limited to this file.
 */

/*
 * Port related info
 */
typedef struct
{
    BOOL initialized;
    unsigned txPortRate;
    unsigned rxPortRate;
} AtmmPortInfo;


/* AtmmVcInfo structure stores the current  attached VCs.
 * vcId 0 - (IX_ATM_MAX_NUM_AAL_OAM_TX_VCS - 1) represent  transmit VCs.  
 * vcId IX_ATM_MAX_NUM_AAL_OAM_TX_VCS - (IX_ATMM_MAX_RX_TX_VCS - 1) represent 
 * receive VCs.
 */
typedef struct
{
    BOOL inUse;
    IxAtmmVc vcData;
    IxAtmLogicalPort port;
} AtmmVcInfo;

/*
 * Variable declarations global to this file only. Externs are followed by
 * static variables.
 */
static IxOsalMutex ixAtmmMutex;
static BOOL ixAtmmInitDone = FALSE;

static IxAtmmVcChangeCallback vcChangeCbTable[IX_ATMM_MAX_NUM_VC_CHANGE_CBS];

static AtmmVcInfo ixAtmmVcInfo[IX_ATMM_MAX_RX_TX_VCS]; /* Tx + Rx */
static AtmmPortInfo ixAtmmPortInfo[IX_UTOPIA_MAX_PORTS];

/* N.B: Global to allow unit tests to reset */
BOOL ixAtmmUtopiaInitDone = FALSE;

/*
 * Function definitons
 */
PRIVATE void
clearTables (void);

PRIVATE BOOL
portValidate (IxAtmLogicalPort port);

PRIVATE IX_STATUS
checkVcNotAlreadyRegistered (IxAtmLogicalPort port, 
			     IxAtmmVc *vcDesc, 
			     IxAtmSchedulerVcId *vcId);

PRIVATE IX_STATUS
validateVC(IxAtmmVc *vcDesc);

PRIVATE IX_STATUS
setupTxVc (IxAtmLogicalPort port, IxAtmmVc *vcDesc, IxAtmSchedulerVcId *newVcId);

PRIVATE IX_STATUS
getRxVcId (IxAtmSchedulerVcId *newVcId);

PRIVATE void
callCallbacks (IxAtmmVcChangeEvent eventType, 
	       IxAtmLogicalPort port, 
	       IxAtmmVc *vcDesc);

/***********************************************************************
 * Function Name: clearTables
 * Parameters   : None
 * Description  : Initialises global data structures
 * Return Value : void
 ***********************************************************************/
PRIVATE void
clearTables (void)
{
    int i;

    /* Initialise all VCs to empty */
    for (i=0; i<IX_ATMM_MAX_RX_TX_VCS; i++)
    {
	ixAtmmVcInfo[i].inUse = FALSE;
    }

    /* Initialise VC Change table */
    for (i=0; i<IX_ATMM_MAX_NUM_VC_CHANGE_CBS; i++)
    {
	vcChangeCbTable[i] = NULL;
    }

    /* Initialize Port Init table */
    for (i=0; i<IX_UTOPIA_MAX_PORTS; i++)
    {
	ixAtmmPortInfo[i].initialized = FALSE;
    }
}

PRIVATE BOOL
portValidate (IxAtmLogicalPort port)
{
    if ((port < IX_UTOPIA_PORT_0) ||
	(port >= IX_UTOPIA_MAX_PORTS) || 
	!ixAtmmPortInfo[port].initialized)
    {
	return FALSE;
    }
    
    return TRUE;
}

PUBLIC IX_STATUS
ixAtmmInit (void)
{
    if (ixAtmmInitDone)
    {
	return IX_FAIL;
    }

    clearTables ();    

    /*
     * With this callback there is no circular dependancy
     * between the IxAtmm main component and the IxAtmmDataPath
     * sub component.
     */
    ixAtmmVcQueryCallbackRegister ( ixAtmmVcQuery );

    /* Initialize the mutex for the ixAtmmVcInfo & vcChange table */
    if (ixOsalMutexInit (&ixAtmmMutex) != IX_SUCCESS)
    {
	return IX_FAIL;
    }
    
    ixAtmmInitDone = TRUE;
    
    return IX_SUCCESS;
}

PUBLIC IX_STATUS
ixAtmmUtopiaInit (unsigned numPorts,
		  IxAtmmPhyMode phyMode,
		  IxAtmmPortCfg portCfgs[],
		  IxAtmmUtopiaLoopbackMode loopbackMode)
{
    IX_STATUS retval;
    IxFeatureCtrlReg ctrlRegister;

    if (!ixAtmmInitDone)
    {
	return IX_FAIL;
    }

    /* Check is already configured */
    if (ixAtmmUtopiaInitDone)
    {
	return IX_ATMM_RET_ALREADY_INITIALIZED;
    }

    /* Check silicon type. 
     * Do not perform feature checkings only if
     * IXP42X - A0 silicon. 
     */
    if ((IX_FEATURE_CTRL_SILICON_TYPE_A0 != 
        (ixFeatureCtrlProductIdRead() & IX_FEATURE_CTRL_SILICON_STEPPING_MASK))
        || (IX_FEATURE_CTRL_DEVICE_TYPE_IXP42X != ixFeatureCtrlDeviceRead ()))
    {
	/* Read the hardware capability register */
	ctrlRegister = ixFeatureCtrlHwCapabilityRead(); 

	/* Shift 16bits to the left and mask for bit 16 and 17 */
	ctrlRegister = (ctrlRegister >> IX_FEATURECTRL_UTOPIA_PHY_LIMIT) & 0x3; 
	
	switch(ctrlRegister)
	{
	    case IX_FEATURECTRL_REG_UTOPIA_32PHY:
		/* Verify the numPort is valid */
		/* Note: Currently can support only 24 utopia ports */
		if (numPorts > IX_ATMM_MAX_24UTOPIA_PORTS)
		{
		    printf("Unable to support more than 24 ports\n");
		    return IX_FAIL;
		}
		break;
	    case IX_FEATURECTRL_REG_UTOPIA_16PHY:
		/* Verify the numPort is valid */
		if (numPorts > IX_ATMM_MAX_16UTOPIA_PORTS)
		{
		    printf("Unable to support more than 16 ports\n");
		    return IX_FAIL;
		}
		break;
	    case IX_FEATURECTRL_REG_UTOPIA_8PHY:
		/* Verify the numPort is valid */
		if (numPorts > IX_ATMM_MAX_8UTOPIA_PORTS)
		{
		    printf("Unable to support more than 8 ports\n");
		    return IX_FAIL;
		}
		break;
	    case IX_FEATURECTRL_REG_UTOPIA_4PHY:
		/* Verify the numPort is valid */
		if (numPorts > IX_ATMM_MAX_4UTOPIA_PORTS)
		{
		    printf("Unable to support more than 4 ports\n");
		    return IX_FAIL;
		}
		break;
	    default:
		printf("Invalid Control Register value\n");
		return IX_FAIL;
	}
    }

    retval = ixAtmmUtopiaCfgInit (numPorts, phyMode, portCfgs, loopbackMode);

    if (retval == IX_SUCCESS)
    {
	ixAtmmUtopiaInitDone = TRUE;
    }

    return retval;
}

PUBLIC IX_STATUS
ixAtmmPortInitialize (IxAtmLogicalPort port, 
		      unsigned txPortRate, 
		      unsigned rxPortRate)
{
    IX_STATUS retval;
    unsigned portRateCells;

    /* Should only happen after UTOPIA init */
    if (!ixAtmmUtopiaInitDone)
    {
	return IX_FAIL;
    }

    /* Check input parameters */
    if ((port < IX_UTOPIA_PORT_0) || (port >= IX_UTOPIA_MAX_PORTS))
    {
	return IX_ATMM_RET_INVALID_PORT;
    }
    
    if ((txPortRate == 0) || (rxPortRate == 0))
    {
	return IX_FAIL;
    }
    
    if (ixAtmmPortInfo[port].initialized)
    {
	return IX_ATMM_RET_ALREADY_INITIALIZED;
    }
    
    /* Initialise AtmSch with the transmit port rate (cells per second) */
    portRateCells = (unsigned) txPortRate / IX_ATMM_BITS_PER_ATM_CELL;
    
    /* Setup the scheduler for this port
     * Min cells to schedule is over the Q low threshold
     * so that schedule tables generated is big enough to push
     * the Tx Q over the theshold
     */
    retval = ixAtmSchPortModelInitialize (port, 
					  portRateCells,
					  IX_ATMM_MIN_SCH_TABLE_CELLS);

    if (retval != IX_SUCCESS)
    {
	/*
	 * N.B. Not rolling back run time init
	 */
	return IX_FAIL;
    }

    /* 
     * Do run time initialization of ATMM 
     * This will check if run time init has occurred already
     * etc.
     */
    retval = ixAtmmDataPathSetup (port);
    if (retval != IX_SUCCESS)
         
    {
	return IX_FAIL;
    }

    ixAtmmPortInfo[port].txPortRate = txPortRate;
    ixAtmmPortInfo[port].rxPortRate = rxPortRate;
    ixAtmmPortInfo[port].initialized = TRUE;
    
    return IX_SUCCESS;
}

PUBLIC IX_STATUS
ixAtmmPortQuery (IxAtmLogicalPort port, 
		 unsigned *txPortRate, 
		 unsigned *rxPortRate)
{
    if ((txPortRate == NULL) || (rxPortRate == NULL))
    {
        return IX_ATMM_RET_INVALID_PARAM_PTR;
    }

    if (!portValidate (port))
    {
	return IX_ATMM_RET_INVALID_PORT;
    }

    *txPortRate = ixAtmmPortInfo[port].txPortRate;
    *rxPortRate = ixAtmmPortInfo[port].rxPortRate;

    return IX_SUCCESS;
}

PUBLIC IX_STATUS
ixAtmmPortModify (IxAtmLogicalPort port, 
		  unsigned txPortRate, 
		  unsigned rxPortRate)
{
    IX_STATUS retval;
    unsigned portRateCells;

    if (!portValidate (port))
    {
	return IX_ATMM_RET_INVALID_PORT;
    }

    if ((txPortRate == 0) || (rxPortRate == 0))
    {
	return IX_FAIL;
    }
    
    /* Notify AtmSch of the new transmit port rate (cells per second) */
    portRateCells = (unsigned) txPortRate / IX_ATMM_BITS_PER_ATM_CELL;

    IX_ATMM_LOCK();

    retval = ixAtmSchPortRateModify (port, portRateCells);

    if (retval != IX_SUCCESS) 
    {
	IX_ATMM_UNLOCK();
	return IX_FAIL;
    }

    /* Store port rates */
    ixAtmmPortInfo[port].txPortRate = txPortRate;
    ixAtmmPortInfo[port].rxPortRate =  rxPortRate;

    IX_ATMM_UNLOCK();

    return IX_SUCCESS;
}

/************************************************************************
 * Function Name: checkVcNotAlreadyRegistered
 * Parameters   : In: port - Port to register this VC
 *                In: vcDesc - VC descriptor for VC to be verified
 *                Out: vcId - Set to vcId of existing VC if the VC has 
 *                            been previously registered.  
 *                            Set to -1 otherwise.
 * Description  : Validates whether a given VC descriptor has
 *                already been registered in the system
 * Return Value : IX_SUCCESS, IX_ATMM_RET_ALREADY_ACTIVATED
 ************************************************************************/
PRIVATE IX_STATUS
checkVcNotAlreadyRegistered (IxAtmLogicalPort port, IxAtmmVc *vcDesc, IxAtmSchedulerVcId *vcId)
{
    int i;

    *vcId = -1;
    for (i=0; i<IX_ATMM_MAX_RX_TX_VCS; i++)
    {
	if (port == ixAtmmVcInfo[i].port)
	{
	    if ( (ixAtmmVcInfo[i].inUse) &&
		 (ixAtmmVcInfo[i].vcData.vpi == vcDesc->vpi) &&
		 (ixAtmmVcInfo[i].vcData.vci == vcDesc->vci) &&
		 (ixAtmmVcInfo[i].vcData.direction == vcDesc->direction) )
	    {
		*vcId = i;
		return IX_ATMM_RET_VC_CONFLICT;
	    }
	}
    }
    return IX_SUCCESS;
}


/************************************************************************
 * Function Name: validateVC
 * Parameters   : In: vcDesc - VC descriptor for VC to be validated
 * Description  : Verifies whether a given VC descriptor is a valid
 *                one in the system
 * Return Value : IX_SUCCESS, IX_ATMM_INVALID_VC_DESCRIPTOR, 
 *                IX_ATMM_VC_CONFLICT
 ************************************************************************/
PRIVATE IX_STATUS
validateVC(IxAtmmVc *vcDesc)
{
    if (vcDesc == NULL)
    {
	return IX_ATMM_RET_INVALID_VC_DESCRIPTOR;
    }

    if ( (vcDesc->vpi > 0xff) ||   /* VPI is an 8 bit number at UNI */
	 (vcDesc->vci > 0xffff) )  /* VCI is a 16 bit number */
    {
	return IX_ATMM_RET_INVALID_VC_DESCRIPTOR;
    }
    
    if ( (vcDesc->direction != IX_ATMM_VC_DIRECTION_TX) &&
	 (vcDesc->direction != IX_ATMM_VC_DIRECTION_RX) )
    {
	return IX_ATMM_RET_INVALID_VC_DESCRIPTOR;
    }
    
    switch (vcDesc->trafficDesc.atmService)
    {
	case IX_ATM_CBR:
	case IX_ATM_RTVBR:
	case IX_ATM_VBR:
	case IX_ATM_UBR:
	case IX_ATM_ABR:
	    break;
	default:
	    return IX_ATMM_RET_INVALID_VC_DESCRIPTOR;
    }

    return IX_SUCCESS;
}

/************************************************************************
 * Function Name: setupTxVc
 * Parameters   : In: port - port to add VC on
 *                In: vcDesc - VC descriptor for VC to be added
 *                Out: newVcId - ptr to vcId allocated to new VC
 * Description  : Adds a new Tx VC to the system (via the ixAtmSch 
 *                component) and returns it's vcId.
 * Return Value : IX_SUCCESS, IX_ATMM_PORT_CAPACITY_IS_FULL,
 *                IX_ATMM_INVALID_VC_DESCRIPTOR
 ************************************************************************/
PRIVATE IX_STATUS
setupTxVc (IxAtmLogicalPort port, IxAtmmVc *vcDesc, IxAtmSchedulerVcId *newVcId)
{
    IX_STATUS retval;

    /* Transmit VCs must be registered with the scheduler,
     * N.B. ATMSCH generates the vcId
     */
    retval = ixAtmSchVcModelSetup (port, &vcDesc->trafficDesc, newVcId);

    if (retval == IX_ATMSCH_RET_NOT_ADMITTED) 
    {
	return IX_ATMM_RET_PORT_CAPACITY_IS_FULL;
    }
    if (retval == IX_FAIL) 
    {
	return IX_ATMM_RET_INVALID_VC_DESCRIPTOR;
    }

    /* Check ATMSCH generated a VcId in the correct range */
    IX_OSAL_ASSERT((*newVcId >= 0) && (*newVcId < IX_ATM_MAX_NUM_AAL_OAM_TX_VCS ));

    /* If the VC is already in use then ATMM & ATMSCH are inconsistent */
    IX_OSAL_ASSERT(ixAtmmVcInfo[*newVcId].inUse == FALSE);
    
    return IX_SUCCESS;
}

/************************************************************************
 * Function Name: getRxVcId
 * Parameters   : Out: newVcId - ptr to vcId allocated to new VC
 * Description  : Finds the next available Rx vcId and returns it
 * Return Value : IX_SUCCESS, IX_ATMM_PORT_CAPACITY_IS_FULL
 ************************************************************************/
PRIVATE IX_STATUS
getRxVcId (IxAtmSchedulerVcId *newVcId)
{
    int i;

    *newVcId = -1;
	    
    for (i=IX_ATM_MAX_NUM_AAL_OAM_TX_VCS ; i<IX_ATMM_MAX_RX_TX_VCS; i++)
    {
	if (ixAtmmVcInfo[i].inUse == FALSE)
	{
	    *newVcId = i;
	    return IX_SUCCESS;
	}
    }
    return IX_ATMM_RET_PORT_CAPACITY_IS_FULL;
}

/************************************************************************
 * Function Name: callCallbacks
 * Parameters   : In: eventType - Indicates whether VC has been added 
 *                                or removed
 *                In: port to callback on
 *                In: vcDesc - VC Descriptor of modified VC
 * Description  : Invokes all registered callbacks with indication of the
 *                change to the system profile.
 * Return Value : void
 ************************************************************************/
PRIVATE void
callCallbacks (IxAtmmVcChangeEvent eventType, 
	       IxAtmLogicalPort port, 
	       IxAtmmVc *vcDesc)
{
    int i;
    IxAtmmVcChangeCallback callback = NULL;  

    for (i=0; i<IX_ATMM_MAX_NUM_VC_CHANGE_CBS; i++)
    {
	callback = vcChangeCbTable[i];
	if (callback != NULL)
	{
	    /* 
	     * Call the client supplied callback to inform 
	     * of the newly changed VC.  We return the original
	     * VC description supplied by the client.  It is the 
	     * responsibility of the client supplied callback that 
	     * this not be modified.
	     */
	    callback (eventType, port, vcDesc);
	}
    }
}

PUBLIC IX_STATUS
ixAtmmVcRegister (IxAtmLogicalPort port, 
		  IxAtmmVc *vcDesc, 
		  IxAtmSchedulerVcId *vcId)
{    
    IxAtmSchedulerVcId newVcId;
    IX_STATUS retval = IX_SUCCESS;

    if (!portValidate (port))
    {
	return IX_ATMM_RET_INVALID_PORT;
    }

    retval = validateVC(vcDesc);
    if (retval != IX_SUCCESS) 
    {
	return retval;
    }

    if (vcId == NULL)
    {
        return IX_ATMM_RET_INVALID_PARAM_PTR;
    }

    IX_ATMM_LOCK();

    /* Check that the VC hasn't been registered already. */
    retval = checkVcNotAlreadyRegistered (port, vcDesc, vcId);

    if (retval == IX_SUCCESS)
    {
	if (vcDesc->direction == IX_ATMM_VC_DIRECTION_TX) 
	{ 
	    retval = setupTxVc (port, vcDesc, &newVcId);
	}
	else
	{
	    retval = getRxVcId (&newVcId);
	}
    }

    if (retval == IX_SUCCESS) 
    {
	/* Store the VC Details */
        ixAtmmVcInfo[newVcId].port = port;
	ixAtmmVcInfo[newVcId].vcData = *vcDesc;
	*vcId = newVcId;
	ixAtmmVcInfo[newVcId].inUse = TRUE;
    }

    IX_ATMM_UNLOCK();

    if (retval == IX_SUCCESS)
    {
	callCallbacks (IX_ATMM_VC_CHANGE_EVENT_REGISTER, port, vcDesc);
    }
    return retval;
}

PUBLIC IX_STATUS
ixAtmmVcDeregister (IxAtmLogicalPort port, IxAtmSchedulerVcId vcId)
{
    IX_STATUS retval = IX_SUCCESS;
    IxAtmmVc *vcRemoved = NULL;

    if (!portValidate (port))
    {
	return IX_ATMM_RET_INVALID_PORT;
    }

    IX_ATMM_LOCK();
    
    if ( (vcId < 0) || 
	 (vcId > (IX_ATMM_MAX_RX_TX_VCS - 1) ) ||
	 !ixAtmmVcInfo[vcId].inUse || 
	 (ixAtmmVcInfo[vcId].port != port))
    {
	retval = IX_FAIL;
    }
    else
    {
	if (ixAtmmVcInfo[vcId].vcData.direction == IX_ATMM_VC_DIRECTION_TX)
	{
	    retval = ixAtmSchVcModelRemove (port, vcId);
	    
	    /* Should always succeed */
	    IX_OSAL_ASSERT(retval == IX_SUCCESS);
	}
	vcRemoved = &ixAtmmVcInfo[vcId].vcData;
	ixAtmmVcInfo[vcId].inUse = FALSE;
    }

    IX_ATMM_UNLOCK();

    if (retval == IX_SUCCESS)
    {
	callCallbacks (IX_ATMM_VC_CHANGE_EVENT_DEREGISTER, port, vcRemoved);
    }
    return retval;
}

PUBLIC IX_STATUS
ixAtmmVcQuery (IxAtmLogicalPort port, 
	       unsigned vpi, 
	       unsigned vci, 
	       IxAtmmVcDirection direction, 
	       IxAtmSchedulerVcId *vcId, 
	       IxAtmmVc *vcDesc)
{
    int i;
    IxAtmSchedulerVcId vcIdFound = -1;
    IX_STATUS retval;

    if (!portValidate (port))
    {
	return IX_ATMM_RET_INVALID_PORT;
    }

    if ((vcDesc == NULL) || (vcId == NULL))
    {
        return IX_ATMM_RET_INVALID_PARAM_PTR;
    }

    IX_ATMM_LOCK();
    
    for (i=0; i<IX_ATMM_MAX_RX_TX_VCS; i++)
    {
	if ( (ixAtmmVcInfo[i].inUse) && 
	     (ixAtmmVcInfo[i].port == port) &&
	     (ixAtmmVcInfo[i].vcData.direction == direction) &&
	     (ixAtmmVcInfo[i].vcData.vci == vci) &&
	     (ixAtmmVcInfo[i].vcData.vpi == vpi))
	{
	    vcIdFound = i;
	    break;
	}
    }

    if (vcIdFound == -1)
    {
	retval = IX_ATMM_RET_NO_SUCH_VC;
    }
    else
    {
	*vcId = vcIdFound;
	*vcDesc = ixAtmmVcInfo[vcIdFound].vcData;
	retval = IX_SUCCESS;
    }

    IX_ATMM_UNLOCK();

    return retval;
}

PUBLIC IX_STATUS
ixAtmmVcIdQuery (IxAtmLogicalPort port, IxAtmSchedulerVcId vcId, IxAtmmVc *vcDesc)
{
    IX_STATUS retval = IX_SUCCESS;

    if (!portValidate (port))
    {
	return IX_ATMM_RET_INVALID_PORT;
    }

    if (vcDesc == NULL)
    {
        return IX_ATMM_RET_INVALID_PARAM_PTR;
    }

    IX_ATMM_LOCK();
    
    if ( (vcId < 0) || (vcId > (IX_ATMM_MAX_RX_TX_VCS - 1) ) )
    {
	retval = IX_ATMM_RET_INVALID_VC_ID;
    }
    if ( (retval == IX_SUCCESS) &&
	 (!ixAtmmVcInfo[vcId].inUse || 
	  (ixAtmmVcInfo[vcId].port != port) ))
    {
	retval = IX_ATMM_RET_NO_SUCH_VC;
    }
    if (retval == IX_SUCCESS)
    {
	*vcDesc = ixAtmmVcInfo[vcId].vcData;
	retval = IX_SUCCESS;
    }
    IX_ATMM_UNLOCK();

    return retval;
}

PUBLIC IX_STATUS
ixAtmmVcChangeCallbackRegister (IxAtmmVcChangeCallback callback)
{
    int i;
    
    /* Check if at least one port initialized */
    if (!ixAtmmPortInfo[IX_UTOPIA_PORT_0].initialized)
    {
	return IX_FAIL;
    }
    
    IX_ATMM_LOCK();
    for (i=0; i<IX_ATMM_MAX_NUM_VC_CHANGE_CBS; i++)
    {
	if (vcChangeCbTable[i] == NULL)
	{
	    vcChangeCbTable[i] = callback;
	    break;
	}
    }
    IX_ATMM_UNLOCK();

    if (i == IX_ATMM_MAX_NUM_VC_CHANGE_CBS)
    {
	return IX_FAIL;
    }

    return IX_SUCCESS;
}

PUBLIC IX_STATUS
ixAtmmVcChangeCallbackDeregister (IxAtmmVcChangeCallback callback)
{
    int i;

    /* Check if at least one port initialized */
    if (!ixAtmmPortInfo[IX_UTOPIA_PORT_0].initialized)
    {
	return IX_FAIL;
    }

    IX_ATMM_LOCK();
    for (i=0; i<IX_ATMM_MAX_NUM_VC_CHANGE_CBS; i++)
    {
	if (vcChangeCbTable[i] == callback)
	{
	    vcChangeCbTable[i] = NULL;
	    break;
	}
    }
    IX_ATMM_UNLOCK();

    /* specified callback is not registered */
    if (i == IX_ATMM_MAX_NUM_VC_CHANGE_CBS)
    {
	return IX_FAIL;
    }
    return IX_SUCCESS;
}
