/**
* @file ixAtmdTxCfgIf.c
*
 * @author Intel Corporation
* @date 17 March 2002
*
* @brief ATM TX configuration and management
*
* Client interface for Tx configuration
*
* Design Notes:
*    All function share the same local lock
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
* Sytem defined include files
*/

/*
* Put the user defined include files required.
*/

#include "IxOsal.h"

#include "IxAtmdAccCtrl.h"
#include "IxAtmdAssert_p.h"
#include "IxAtmdTxCfgInfo_p.h"
#include "IxAtmdTxCfgIf_p.h"
#include "IxAtmdPortMgmt_p.h"
#include "IxAtmdUtil_p.h"
#include "IxAtmdNpe_p.h"

/**
*
* @struct IxAtmdAccTxStats
*
* @brief Tx ststs
*
* This structure is used to collect stats informations abour tx activity
*
*/
typedef struct
{
    /* tx control counters */
    unsigned int connectCount;
    unsigned int disconnectCount;
    unsigned int connectDenied;
    unsigned int disconnectDenied;
} IxAtmdAccTxStats;

static IxAtmdAccTxStats ixAtmdAccTxStats;

static BOOL ixAtmdAccTxCfgInitDone = FALSE;

/* ------------------------------------------------
tx lock get and release
*/
static IxOsalMutex txControlLock;

#define IX_ATMDACC_TX_LOCK_INIT() \
ixOsalMutexInit (&txControlLock)
#define IX_ATMDACC_TX_LOCK_GET() \
(void)ixOsalMutexLock (&txControlLock, IX_OSAL_WAIT_FOREVER)
#define IX_ATMDACC_TX_LOCK_RELEASE() \
(void)ixOsalMutexUnlock (&txControlLock)

/* function prototype */
PRIVATE BOOL
ixAtmdAccTxPortIsEnabledCheck(IxAtmLogicalPort port);

PRIVATE BOOL
ixAtmdAccTxPortIsDisabledCheck(IxAtmLogicalPort port);

PRIVATE IX_STATUS
ixAtmdAccTxPortStateChange(IxAtmLogicalPort port, IxAtmdAccPortState requestedState);

PRIVATE void
ixAtmdAccTxPortSetupNotify(unsigned int numPort);

PRIVATE IX_STATUS
ixAtmdAccTxVcConnectParamsValidate (IxAtmLogicalPort port,
                                    unsigned int vpi,
                                    unsigned int vci,
                                    IxAtmdAccAalType aalServiceType,
                                    IxAtmdAccTxVcBufferReturnCallback txDoneCallback,
                                    IxAtmConnId * connIdPtr);

PRIVATE IX_STATUS
ixAtmdAccTxVcConnectPerform (IxAtmLogicalPort port,
                             unsigned int vpi,
                             unsigned int vci,
                             IxAtmdAccAalType aalServiceType,
                             IxAtmdAccUserId userId,
                             IxAtmdAccTxVcBufferReturnCallback txDoneCallback,
                             IxAtmConnId * connIdPtr);

PUBLIC IX_STATUS
ixAtmdAccPortTxScheduledModeEnablePararmsValidate (IxAtmLogicalPort port,
                                                   IxAtmdAccTxVcDemandUpdateCallback demandUpdate,
                                                   IxAtmdAccTxVcDemandClearCallback demandClear,
                                                   IxAtmdAccTxSchVcIdGetCallback vcIdGet);

/* ------------------------------------------------------
* definition of private functions
*/

/* ------------------------------------------------------
* Query the if the port is enabled safely with locks
*/
PRIVATE BOOL
ixAtmdAccTxPortIsEnabledCheck(IxAtmLogicalPort port)
{
    BOOL result;
    IX_ATMDACC_TX_LOCK_GET ();
    result = ixAtmdAccTxPortEnabledQuery(port);
    IX_ATMDACC_TX_LOCK_RELEASE ();
    return result;
}

/* ------------------------------------------------------
* Query the if the port is disabled safely with locks
*/
PRIVATE BOOL
ixAtmdAccTxPortIsDisabledCheck(IxAtmLogicalPort port)
{
    BOOL result;
    IX_ATMDACC_TX_LOCK_GET ();
    result = ixAtmdAccTxPortDisabledQuery(port);
    IX_ATMDACC_TX_LOCK_RELEASE ();
    return result;
}

/* ------------------------------------------------------
* Change the port state, safely within locks
*/
PRIVATE IX_STATUS
ixAtmdAccTxPortStateChange(IxAtmLogicalPort port, IxAtmdAccPortState requestedState)
{
    IX_STATUS returnStatus;
    IX_ATMDACC_TX_LOCK_GET ();
    returnStatus = ixAtmdAccTxPortStateChangeHandler(port, requestedState);
    IX_ATMDACC_TX_LOCK_RELEASE ();
    return returnStatus;
}

/* ------------------------------------------------------
* Set the number of ports configured in the system
*/
PRIVATE void
ixAtmdAccTxPortSetupNotify(unsigned int numPort)
{
    IX_ATMDACC_TX_LOCK_GET ();
    ixAtmdAccTxPortSetupNotifyHandler(numPort);
    IX_ATMDACC_TX_LOCK_RELEASE ();
    return;
}

/* -------------------------------------------
*   Initialise the tx subcomponent
*/
IX_STATUS
ixAtmdAccTxCfgIfInit (void)
{
    IX_STATUS returnStatus = IX_SUCCESS;

    /* reset statistics counters */
    ixAtmdAccTxCfgIfStatsReset ();

    /* initialise tx data structures */
    if((ixAtmdAccTxCfgInitDone == FALSE)
        && (ixAtmdAccTxCfgInfoInit () == IX_SUCCESS) &&
        (ixOsalMutexInit (&txControlLock) == IX_SUCCESS))
    {
        /* register port state interface to port management */
        ixAtmdAccPortStateHandlersRegister(
            ixAtmdAccTxPortSetupNotify,
            ixAtmdAccTxPortStateChange,
            ixAtmdAccTxPortIsEnabledCheck,
            ixAtmdAccTxPortIsDisabledCheck);

        /* initialisae a security flag */
        ixAtmdAccTxCfgInitDone = TRUE;
    }
    else
    {
        returnStatus = IX_FAIL;
    } /* end of if-else(powerOf2) */
    return returnStatus;
}

/* -----------------------------------------------------
*   display tx statistics
*/
void
ixAtmdAccTxCfgIfStatsShow (void)
{
    IxAtmdAccTxStats statsSnapshot;
    
    if(ixAtmdAccTxCfgInitDone)
    {
        /* get a stats snapshot */
        IX_ATMDACC_TX_LOCK_GET ();
        
        statsSnapshot = ixAtmdAccTxStats;
        
        IX_ATMDACC_TX_LOCK_RELEASE ();
        
        printf ("AtmdTx\n");
        printf ("Tx Connect ok ................. : %10u\n",
            statsSnapshot.connectCount);
        printf ("Tx Disconnect ok .............. : %10u\n",
            statsSnapshot.disconnectCount);
        printf ("Tx Connect denied ............. : %10u (should be 0)\n",
            statsSnapshot.connectDenied);
        printf ("Tx Disconnect denied .......... : %10u\n",
            statsSnapshot.disconnectDenied);
        
        ixAtmdAccTxCfgInfoStatsShow ();
    }
}

/* -----------------------------------------------------
*   display tx configuration
*/
void
ixAtmdAccTxCfgIfPortShow (IxAtmLogicalPort port)
{
    if(ixAtmdAccTxCfgInitDone)
    {
        IX_ATMDACC_TX_LOCK_GET ();
        
        ixAtmdAccTxCfgInfoPortShow (port);
        
        IX_ATMDACC_TX_LOCK_RELEASE ();
    }
}

/* -----------------------------------------------------
*   display tx configuration
*/
void
ixAtmdAccTxCfgIfChannelShow (IxAtmLogicalPort port)
{
    if(ixAtmdAccTxCfgInitDone)
    {
        IX_ATMDACC_TX_LOCK_GET ();
        
        ixAtmdAccTxCfgInfoChannelShow (port);
        
        IX_ATMDACC_TX_LOCK_RELEASE ();
    }
}

/* -----------------------------------------------------
*   reset tx statistics
*/
void
ixAtmdAccTxCfgIfStatsReset (void)
{
    /* initialise the error counters */
    ixOsalMemSet(&ixAtmdAccTxStats, 0, sizeof(ixAtmdAccTxStats));

    /* initialise the stats of the dependant component */
    ixAtmdAccTxCfgInfoStatsReset();
}

/* -----------------------------------------------
*   validate inputs for ixAtmdAccTxVcConnect
*/
PRIVATE IX_STATUS
ixAtmdAccTxVcConnectParamsValidate (IxAtmLogicalPort port,
                                    unsigned int vpi,
                                    unsigned int vci,
                                    IxAtmdAccAalType aalServiceType,
                                    IxAtmdAccTxVcBufferReturnCallback txDoneCallback,
                                    IxAtmConnId * connIdPtr)
{
    /* Sanity check input params */
    if ((port >= IX_UTOPIA_MAX_PORTS)    ||
        (port < IX_UTOPIA_PORT_0)        ||
        (vpi > IX_ATM_MAX_VPI)           ||
        (vci > IX_ATM_MAX_VCI)           ||
        (txDoneCallback == NULL)         ||
        (connIdPtr == NULL))
    {
        return IX_FAIL;
    }

    
    /* Service specific checks */
    switch(aalServiceType)
    {    
    case IX_ATMDACC_AAL5:
    case IX_ATMDACC_AAL0_48:
    case IX_ATMDACC_AAL0_52:
        if ((vpi  == IX_ATMDACC_OAM_TX_VPI)   &&
            (vci  == IX_ATMDACC_OAM_TX_VCI))
        {
            return IX_FAIL;
        }
        break;
    case IX_ATMDACC_OAM:
        if ((vpi  != IX_ATMDACC_OAM_TX_VPI)   ||
            (vci  != IX_ATMDACC_OAM_TX_VCI))
        {
            return IX_FAIL;
        }
        break;
    default:
        return IX_FAIL;
        break;
    }
    return IX_SUCCESS;
}

/* ---------------------------------------------------
*   do the real function of ixAtmdAccTxVcConnect
*/
PRIVATE IX_STATUS
ixAtmdAccTxVcConnectPerform (IxAtmLogicalPort port,
                             unsigned int vpi,
                             unsigned int vci,
                             IxAtmdAccAalType aalServiceType,
                             IxAtmdAccUserId userId,
                             IxAtmdAccTxVcBufferReturnCallback txDoneCallback,
                             IxAtmConnId * connIdPtr)
{
    IxAtmSchedulerVcId schedulerId;
    IX_STATUS returnStatus =IX_FAIL;

    /* check that the port has been configured and is UP */
    if (ixAtmdAccTxPortEnabledQuery (port))
    {
        /* check if this VC is already in use */
        if (!ixAtmdAccTxCfgVcConfigured (port, vpi, vci))
        {
            /* allocate a channel and get a connection Id for this connection */
            if(ixAtmdAccTxCfgFreeChannelGet (aalServiceType, connIdPtr) != IX_SUCCESS)
            {
                returnStatus = IX_ATMDACC_BUSY;
            }
            else
            {
                /* get scheduler id for this VC, using the user scheduler callback */
                if(ixAtmdAccTxCfgSchVcIdGet (port,
                    vpi,
                    vci,
                    *connIdPtr,
                    &schedulerId) == IX_SUCCESS)
                {
                    /* configure the channel */
                    returnStatus = ixAtmdAccTxCfgChannelSet (*connIdPtr,
                        schedulerId,
                        port,
                        vpi,
                        vci,
                        aalServiceType,
                        userId,
                        txDoneCallback);
                    if(returnStatus != IX_SUCCESS)
                    {
                        /* roll back by clearing the requested scheduler demand */
                        ixAtmdAccTxCfgVcDemandCancel (*connIdPtr, port, schedulerId);
                    }
                }
            }
        } /* end of if(ixAtmdAccTxCfgVcConfigured) */
    } /* end of if(ixAtmdAccTxPortEnabledQuery) */

    return returnStatus;
}

/* ---------------------------------------------------
*   establish a connection on this port, vpi and vci
*/
PUBLIC IX_STATUS
ixAtmdAccTxVcConnect (IxAtmLogicalPort port,
                      unsigned int vpi,
                      unsigned int vci,
                      IxAtmdAccAalType aalServiceType,
                      IxAtmdAccUserId userId,
                      IxAtmdAccTxVcBufferReturnCallback txDoneCallback,
                      IxAtmConnId * connIdPtr)
{

    IX_STATUS returnStatus = IX_FAIL;

    /* we have not been initialised, or we
    have be passed a null poiter, so no point
    in continuing
    */
    if(ixAtmdAccTxCfgInitDone)
    {
        /* protect against re-entrancy */
        IX_ATMDACC_TX_LOCK_GET ();

        if(ixAtmdAccTxVcConnectParamsValidate (port,
            vpi,
            vci,
            aalServiceType,
            txDoneCallback,
            connIdPtr) == IX_SUCCESS)
        {
            returnStatus = ixAtmdAccTxVcConnectPerform (port,
                vpi,
                vci,
                aalServiceType,
                userId,
                txDoneCallback,
                connIdPtr);
        }

        if(returnStatus == IX_SUCCESS)
        {
            /* increment statistics */
            ixAtmdAccTxStats.connectCount++;
        }
        else
        {
            /* increment statistics */
            ixAtmdAccTxStats.connectDenied++;
        } /* end of if-else(ixAtmdAccTxCfgVcConfigured) */

        IX_ATMDACC_TX_LOCK_RELEASE ();
    }
    return returnStatus;
}

/* ---------------------------------------------------
*   remove a connection
*/
PUBLIC IX_STATUS
ixAtmdAccTxVcTryDisconnect (IxAtmConnId connId)
{
    unsigned int txId;
    IX_STATUS returnStatus = IX_FAIL;

    /* we have not been initialised no point
    in continuing
    */
    if(ixAtmdAccTxCfgInitDone)
    {
        IX_ATMDACC_TX_LOCK_GET ();

        /* check the connId and retrieve the channel id */
        if(ixAtmdAccTxCfgIndexGet (connId, &txId) == IX_SUCCESS)
        {
            /* invalidate the connection Id */
            ixAtmdAccTxCfgConnIdInvalidate (txId);

            /* remove entries from the tx queue if the port is down */
            if(ixAtmdAccTxCfgPortResourcesRelease (connId) == IX_SUCCESS)
            {
                /* check if resources are still allocated */
                if(!ixAtmdAccTxCfgFreeResourcesCheck (txId))
                {
                    returnStatus = IX_ATMDACC_RESOURCES_STILL_ALLOCATED;
                }
                else
                {
                    /* clear any scheduler demand
                    * this must be done before reset because reset
                    * destroys the schedulerId
                    */
                    ixAtmdAccTxCfgVcDemandClear (connId);

                    /* reset internal data */
                    returnStatus = ixAtmdAccTxCfgChannelReset (txId);
                } /* end of if-else(qEntry) */
            }
        }

        if(returnStatus == IX_SUCCESS)
        {
            ixAtmdAccTxStats.disconnectCount++;
        }
        else
        {
            ixAtmdAccTxStats.disconnectDenied++;
        } /* end of if-else(ixAtmdAccTxCfgVcConfigured) */

        IX_ATMDACC_TX_LOCK_RELEASE ();
    }
    return returnStatus;
}

/* ------------------------------------------------
*   set the tx queue nearly-empty threshold
*/
PUBLIC IX_STATUS
ixAtmdAccPortTxCallbackRegister (IxAtmLogicalPort port,
                                 unsigned int numberOfCells,
                                 IxAtmdAccPortTxLowCallback callback)
{
    IX_STATUS returnStatus = IX_FAIL;

    if (ixAtmdAccTxCfgInitDone &&
        (port >= IX_UTOPIA_PORT_0) &&
        (port < IX_UTOPIA_MAX_PORTS) &&
        (callback != NULL))
    {
        IX_ATMDACC_TX_LOCK_GET ();

        if(ixAtmdAccPortConfigured (port) &&
            !ixAtmdAccTxCfgPortVcsExist (port))
        {
            returnStatus = ixAtmdAccTxCfgTxCallbackRegister (port, numberOfCells, callback);
        }

        IX_ATMDACC_TX_LOCK_RELEASE ();
    } /* end of if(ixAtmdAccTxCfgInitDone) */

    return returnStatus;
}

/* ------------------------------------------------
*   set the transmit dome nearly-full threshold
*/
PUBLIC IX_STATUS
ixAtmdAccTxDoneDispatcherRegister (unsigned int numberOfMbufs,
                                   IxAtmdAccTxDoneDispatcher callback)
{
    IX_STATUS returnStatus = IX_FAIL;

    /* Check that initialisation has been done,
    that we have not been passed a null pointer,
    and that the port is idle, the threshold should
    not be set whilst there is traffic present
    */
    if(ixAtmdAccTxCfgInitDone &&
        (callback != NULL))
    {
        IX_ATMDACC_TX_LOCK_GET ();

        if(!ixAtmdAccTxCfgVcsExist ())
        {
            returnStatus = ixAtmdAccTxCfgTxDoneCallbackRegister (numberOfMbufs, callback);
        }

        IX_ATMDACC_TX_LOCK_RELEASE ();
    }

    return returnStatus;
}

/* ----------------------------------------------------
*   check the input params for ixAtmdAccPortTxScheduledModeEnable
*/
PUBLIC IX_STATUS
ixAtmdAccPortTxScheduledModeEnablePararmsValidate (IxAtmLogicalPort port,
                                                   IxAtmdAccTxVcDemandUpdateCallback demandUpdate,
                                                   IxAtmdAccTxVcDemandClearCallback demandClear,
                                                   IxAtmdAccTxSchVcIdGetCallback vcIdGet)
{
    IX_STATUS returnStatus = IX_SUCCESS;

    /* check parameters */
    if ((port >= IX_UTOPIA_MAX_PORTS) ||
        (port < IX_UTOPIA_PORT_0)  ||
        (demandUpdate == NULL)     ||
        (demandClear == NULL)      ||
        (vcIdGet == NULL)          ||
        !ixAtmdAccPortConfigured (port))
    {
        returnStatus = IX_FAIL;
    } /* end of if(port) */

    return returnStatus;
}

/* ----------------------------------------------------
*        regsiter the scheduler
*/
PUBLIC IX_STATUS
ixAtmdAccPortTxScheduledModeEnable (IxAtmLogicalPort port,
                                    IxAtmdAccTxVcDemandUpdateCallback demandUpdate,
                                    IxAtmdAccTxVcDemandClearCallback demandClear,
                                    IxAtmdAccTxSchVcIdGetCallback vcIdGet)
{
    IX_STATUS returnStatus = IX_FAIL;

    /* we have not been initialised no point
    in continuing
    */
    if(ixAtmdAccTxCfgInitDone)
    {
        IX_ATMDACC_TX_LOCK_GET ();

        /* check input params */
        if(ixAtmdAccPortTxScheduledModeEnablePararmsValidate (port,
            demandUpdate,
            demandClear,
            vcIdGet) == IX_SUCCESS)
        {
            /* check no channel are already set on this port */
            if (!ixAtmdAccTxCfgPortVcsExist (port))
            {
                ixAtmdAccTxCfgSchCallbackRegister (port,
                    demandUpdate,
                    demandClear,
                    vcIdGet);
                returnStatus = IX_SUCCESS;
            } /* end of if(ixAtmdAccTxCfgPortVcsExist) */
        }
        IX_ATMDACC_TX_LOCK_RELEASE ();
    }

    return returnStatus;
}

/* ----------------------------------------------------
*       get the tx done queue size
*/
PUBLIC IX_STATUS
ixAtmdAccTxDoneQueueSizeQuery (unsigned int *numberOfPdusPtr)
{
    IX_STATUS returnStatus = IX_FAIL;

    /* check parameters */
    if (ixAtmdAccTxCfgInitDone &&
        numberOfPdusPtr != NULL)
    {
        /* query the queue size */
        returnStatus =
            ixAtmdAccUtilQueueSizeQuery(IX_NPE_A_QMQ_ATM_TX_DONE,
            numberOfPdusPtr);
    } /* end of if(ixAtmdAccTxCfgInitDone) */
    return returnStatus;
}


