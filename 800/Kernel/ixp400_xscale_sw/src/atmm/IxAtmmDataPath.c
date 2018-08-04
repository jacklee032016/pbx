/** 
 * @file    IxAtmmDataPath.c
 * @author Intel Corporation
 * @date    1-MAR-2002
 *
 * @brief   IxAtmm data path sub component (AtmmDataPath) APIs.
 *          No rollback in initialization if any part of initialization fails.
 *          Assumptions:
 *           - DemandUpdate callback from IxAtmd is serialized per VC.
 *           - TxLow callback from IxAtmd will occur in an IRQ context
 *           - VcDemandUpdate callback from IxAtmd can occur in IRQ or task context
 *               => TxLow callback can interrupt VcDemandUpdate callback
 *               => TxLow callback cannot interrupt VcDemandUpdate callback
 *           - No OS calls made in IxAtmmPortTxProcess, i.e. safe to intLock
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
 * Put the system defined include files required
 */

/*
 * Put the user defined include files required
 */
#include "IxOsal.h"
#include "IxAtmTypes.h"
#include "IxAtmm.h"
#include "IxAtmdAccCtrl.h"
#include "IxAtmSch.h"
#include "IxAtmmDataPath_p.h"

/*
 * #defines and macros used in this file.
 */

/*
 * Process Tx Done queue every 1/2 second to ensure queue gets cleared out
 * when idle/low traffic.
 */
#define IX_ATMM_TX_DONE_PERIOD_MSECS        500

/*
 * Process Rx low priority queue every 20 msecs
 */
#define IX_ATMM_RX_LO_PRIORITY_PERIOD_MSECS 20

/*
 * Tx Vc Q fast mutexes, one Q per port.
 */
#define IX_ATMM_TX_MUTEX_TRY_LOCK(port) ixOsalFastMutexTryLock (&ixAtmmTxBusyFastMutex[(port)])
#define IX_ATMM_TX_MUTEX_UNLOCK(port)   (void)ixOsalFastMutexUnlock (&ixAtmmTxBusyFastMutex[(port)])

/*
 * Port State fast mutexes, one per port.
 */
#define IX_ATMM_PORT_MUTEX_TRY_LOCK(port) ixOsalFastMutexTryLock (&ixAtmmPortFastMutex[(port)])
#define IX_ATMM_PORT_MUTEX_UNLOCK(port)   (void)ixOsalFastMutexUnlock (&ixAtmmPortFastMutex[(port)])

/*
 *  Tx timer based
 */

/*
 * Typedefs whose scope is limited to this file.
 */
typedef struct {
    IxOsalThread rxLoPriorityId;
    IxOsalThread txDoneId;
} IxAtmmTaskIds;

/*
 * Variable declarations global to this file only. Externs are followed by
 * static variables.
 */
static IxAtmmTaskIds ixAtmmTaskIds;

/*
 * VC query callback registered by IxAtmm.c
 */
static IxAtmmVcQueryCallback vcQueryCallback = NULL;

/*
 * Fast muxtexs, initialized later
 */
static IxOsalFastMutex ixAtmmTxBusyFastMutex[IX_UTOPIA_MAX_PORTS];
static IxOsalFastMutex ixAtmmPortFastMutex[IX_UTOPIA_MAX_PORTS];

/*
 * TX Vc Q size, needed when system goes IDLE -> BUSY
 */
static unsigned ixAtmmTxQSize[IX_UTOPIA_MAX_PORTS];

/*
 * Static function prototypes
 */
PRIVATE IX_STATUS
ixAtmmTxDoneInit (void);

PRIVATE IX_STATUS
ixAtmmTxSchedulingInit (IxAtmLogicalPort port);

PRIVATE IX_STATUS
ixAtmmRxHiPriorityInit (void);

PRIVATE IX_STATUS
ixAtmmRxLoPriorityInit (void);

PRIVATE void
ixAtmmTxLowHandle (IxAtmLogicalPort port, unsigned numRemainingCells);

PRIVATE IX_STATUS
ixAtmmTxDoneHandle (unsigned numOfPdusToProcess,
		    unsigned *reservedPtr);

PRIVATE IX_STATUS
ixAtmmVcDemandUpdate (IxAtmLogicalPort port, IxAtmSchedulerVcId vcId, unsigned numberOfCells);

PRIVATE void
ixAtmmVcQueueClear (IxAtmLogicalPort port, IxAtmSchedulerVcId vcId);

PRIVATE IX_STATUS 
ixAtmmVcIdGet (IxAtmLogicalPort port,
	       unsigned vpi,
	       unsigned vci,
	       IxAtmConnId connId,
	       IxAtmSchedulerVcId *vcId);

PRIVATE IX_STATUS
ixAtmmTxDoneLoop (void* arg, void** retArgObj);

PRIVATE IX_STATUS
ixAtmmRxLoPriorityLoop (void* arg,
                        void** ptrRetObj);

/*
 * Function definitons
 */


void
ixAtmmVcQueryCallbackRegister ( IxAtmmVcQueryCallback callback)
{
    /* NULL callback not supported */
    IX_OSAL_ASSERT(callback != NULL);

    vcQueryCallback = callback;
}

PRIVATE IX_STATUS
ixAtmmTxDoneLoop (void* arg, void** retArgObj)
{
    unsigned dummyPtr;
    IX_STATUS retval;

    while (TRUE)
    {
	/* 
	 * Sleep for the timer duration 
	 */
	ixOsalSleep (IX_ATMM_TX_DONE_PERIOD_MSECS);

	/* 
	 * Service Tx Done, process all the queue
         * but release the fast mutex to allow interrupts
         * running and service tx done when it is really needed. 
	 */
	retval = ixAtmmTxDoneHandle (IX_ATMDACC_ALLPDUS, &dummyPtr);
	
	IX_OSAL_ENSURE(retval == IX_SUCCESS, "Call to ixAtmmTxDoneHandle() failed");
    }
}

PRIVATE IX_STATUS
ixAtmmRxLoPriorityLoop (void* arg,
                        void** ptrRetObj)
{
    unsigned dummyPtr;
    IX_STATUS retval;

    while (TRUE)
    {
	/* 
	 * Sleep for the timer duration 
	 */
	ixOsalSleep (IX_ATMM_RX_LO_PRIORITY_PERIOD_MSECS);

	/*
	 * Service Rx low priority queue, process all in Q
	 */
	retval = ixAtmdAccRxDispatch ( IX_ATM_RX_B,
					     IX_ATMDACC_ALLPDUS,
					     &dummyPtr);

	IX_OSAL_ENSURE(retval == IX_SUCCESS, "Call to ixAtmdAccRxDispatch() failed");
    }
}

PUBLIC IX_STATUS
ixAtmmPortEnable (IxAtmLogicalPort port)
{
   unsigned int maxTxCells;
   UINT32 lockKey;
   IX_STATUS retval;

   retval = ixAtmdAccPortEnable(port);
   if (retval == IX_SUCCESS)
   {
       /* ensure no interrupt is running */
       lockKey = ixOsalIrqLock();
   
       retval = ixAtmdAccPortTxFreeEntriesQuery(port, &maxTxCells);
   
       if(retval == IX_SUCCESS)
       {
	   IX_ATMM_PORT_MUTEX_UNLOCK(port);
	   if (maxTxCells == ixAtmmTxQSize[port])
	   {
	       /* there are no entries in the TX queue, it is necessary
		  to restart the transmission of cells 
	       */
	       ixAtmmTxLowHandle (port, maxTxCells);
	   }
	   else
	   {
	       /* there are cells in the tx queue : the NPE will consume 
		  them and this will trigger a tx Low interrupt. 
		  The TX traffic will start again
	       */
	   }
       }
       ixOsalIrqUnlock(lockKey);
   }
   else if (retval == IX_ATMDACC_WARNING)
   {
       /* the port is already up */
       retval = IX_SUCCESS;
   }
   else
   {
       retval = IX_FAIL;
   }

   return retval;
}

PUBLIC IX_STATUS
ixAtmmPortDisable (IxAtmLogicalPort port)
{
    IX_STATUS retval;
    UINT32 lockKey;
    
    /* ensure no interrupt is running 
       (scheduling and transmission is done under interrupts)
     */
    lockKey = ixOsalIrqLock();
    
    IX_ATMM_PORT_MUTEX_TRY_LOCK(port);
    
    /* now, cell transmission is stopped */
    ixOsalIrqUnlock(lockKey);

    /* tell the NPE to stop draining the TX queue */

    retval = ixAtmdAccPortDisable(port);
    if (retval == IX_SUCCESS)
    {
	/* wait until a Disable is complete */
	while (ixAtmdAccPortDisableComplete(port) == FALSE)
	{
	    /* since TX done is processing the response, the time 
	       to wait is linked to the TxDone polling time */
	    ixOsalSleep(IX_ATMM_TX_DONE_PERIOD_MSECS/2 );
	}
    }
    else if (retval == IX_ATMDACC_WARNING)
    {
	/* the port is already down */
        retval = IX_SUCCESS;
    }
    else
    {
        retval = IX_FAIL;
    }

    return retval;
}

IX_STATUS
ixAtmmDataPathSetup (IxAtmLogicalPort port)
{
    IX_STATUS retval = IX_SUCCESS;

    /*
     * Initialize Tx Scheduling control
     */
    retval = ixAtmmTxSchedulingInit (port);

    if (retval == IX_SUCCESS)
    {
	/*
	 * Initialize Tx Done control
	 */
	retval = ixAtmmTxDoneInit ();
    }
    
    if (retval == IX_SUCCESS)
    {
	/*
	 * Initialize Rx High Priority control
	 */
	retval = ixAtmmRxHiPriorityInit ();
    }

    if (retval == IX_SUCCESS)
    {
	/*
	 * Initialize Rx Low Priority control
	 */
	retval = ixAtmmRxLoPriorityInit ();
    }

    return retval;
}

PRIVATE IX_STATUS
ixAtmmTxSchedulingInit (IxAtmLogicalPort port)
{
    static BOOL firstInitDone = FALSE;
    static BOOL txSchedulingInitDone[IX_UTOPIA_MAX_PORTS];
    IX_STATUS retval = IX_SUCCESS;
    UINT32 i;

    /* Clear InitDone flags on first initialization */
    if (!firstInitDone)
    {
	for (i=0; i < (sizeof(txSchedulingInitDone) / sizeof(BOOL)); i++)
	{
	    txSchedulingInitDone[i] = FALSE;
	}
	firstInitDone = TRUE;
    }

    /* Only initialize each port once */
    if (txSchedulingInitDone[port])
    {
	retval =  IX_ATMM_RET_ALREADY_INITIALIZED;
    }

    if (retval == IX_SUCCESS)
    {
	/*
	 * Put Atmm in scheduling mode, this registers Atmm callbacks.     
	 */
	retval = ixAtmdAccPortTxScheduledModeEnable (port,
						     ixAtmmVcDemandUpdate,
						     ixAtmmVcQueueClear,
						     ixAtmmVcIdGet);
    }

    if (retval == IX_SUCCESS)
    {
	/*
	 * Get depth of tx Q for case when system goes IDLE -> BUSY
	 */
	retval = ixAtmdAccPortTxFreeEntriesQuery (port, &ixAtmmTxQSize[port]);
    }

    
    if (retval == IX_SUCCESS)
    {
	/*
	 * Setup notification for Tx Q low.
	 * Notify when 0 enties in Q, i.e. Q is EMPTY.
	 */
	retval = ixAtmdAccPortTxCallbackRegister (port,
						  0,
						  ixAtmmTxLowHandle);
    }
    
    if (retval == IX_SUCCESS)
    {
	/*
	 * Initialize BUSY fast mutex for this port, 
	 * the mutex will be free, i.e. the port is not BUSY
	 */
	retval = ixOsalFastMutexInit (&ixAtmmTxBusyFastMutex[port]);
    }

    if (retval == IX_SUCCESS)
    {
	/*
	 * Initialize PORT fast mutex for this port, 
	 * the mutex will be UP 
	 */
	retval = ixOsalFastMutexInit (&ixAtmmPortFastMutex[port]);
	if (retval == IX_SUCCESS)
        {
          retval = IX_ATMM_PORT_MUTEX_TRY_LOCK(port);
        }
    }

    if (retval == IX_SUCCESS)
    {
	txSchedulingInitDone[port] = TRUE;
    }
    else
    {
        retval = IX_FAIL;
    }
    return retval;    
}

PRIVATE void
ixAtmmVcQueueClear (IxAtmLogicalPort port, IxAtmSchedulerVcId vcId)
{   
    IX_STATUS retval;
    UINT32 lockKey;

    lockKey = ixOsalIrqLock();
    retval = ixAtmSchVcQueueClear (port, vcId);
    ixOsalIrqUnlock (lockKey);

    IX_OSAL_ENSURE(retval == IX_SUCCESS, "Call to ixAtmSchVcQueueClear() failed");
}

PRIVATE IX_STATUS
ixAtmmVcDemandUpdate (IxAtmLogicalPort port, IxAtmSchedulerVcId vcId, unsigned numberOfCells)
{
    IX_STATUS retval = IX_FAIL;
    IxAtmScheduleTable *retTable;
    UINT32 lockKey;
    
    /*
     * N.B. Running from a task or interrupt level.
     * From the task level we could be interrupted
     * so need to protect VcQueueUpdate, SchTableUpdate
     * and PortTxProcess
     */
    lockKey = ixOsalIrqLock();

    /*
     * If we can get the PORT fast mutex it means the port was
     * UP so can initiate transmission.
     */
    if (IX_ATMM_PORT_MUTEX_TRY_LOCK(port) == IX_SUCCESS)
    {

        /*
         * Inform Scheduler of the demand
         * N.B. This is data path so IxAtmSch will validate parameters
         * rather than do this twice.
         */
        retval = ixAtmSchVcQueueUpdate (port, vcId, numberOfCells);
        if( retval == IX_SUCCESS )
        {
            /*
             * If we can get the BUSY fast mutex it means the port was
             * IDLE so we need to initiate transmission.
             */
            if (IX_ATMM_TX_MUTEX_TRY_LOCK(port) == IX_SUCCESS)
            {
                /* 
                 * Build a new schedule table, N.B. system was idle so can potentially
                 * generate a table with ixAtmmTxQSize cells
                 */
                retval = ixAtmSchTableUpdate (port,
            				  ixAtmmTxQSize[port],
            				  &retTable);
                
                /*
                 * VcQueueUpdate above placed cells in the scheduler
                 * so the tableUpdate should succeed
                 */
                IX_OSAL_ASSERT(retval == IX_SUCCESS);
                
                /*
                 * Tell Atmd to send the cells in the schedule table.
                 */
                retval = ixAtmdAccPortTxProcess (port, retTable);
                
                if (retval != IX_SUCCESS)
                {
            	    retval = IX_FAIL;
    	        }
            }
        } 
	IX_ATMM_PORT_MUTEX_UNLOCK(port);
    }

    ixOsalIrqUnlock (lockKey);
    return retval;
}

PRIVATE void
ixAtmmTxLowHandle (IxAtmLogicalPort port, unsigned maxTxCells)
{
    IxAtmScheduleTable *retTable;
    IX_STATUS retval;
    UINT32 lockKey;

    /*
     * If we can get the PORT fast mutex it means the port was
     * UP so can initiate transmission.
     */
    if (IX_ATMM_PORT_MUTEX_TRY_LOCK(port) == IX_SUCCESS)
    {
        lockKey = ixOsalIrqLock();

        /*
         * Build a new schedule table
         * N.B. This is data path so IxAtmSch will validate parameters
         *      rather than do this twice.
         */
        retval = ixAtmSchTableUpdate (port,
    				  maxTxCells,
    				  &retTable);
    
        if (retval == IX_SUCCESS)
        {
    	    /*
    	     * Tell Atmd to send the cells in the schedule table.
    	     */
    	    retval = ixAtmdAccPortTxProcess (port, retTable);	
        }
        else
        {
    	    /*
    	     * Only other valid retval is QUEUE_EMPTY
    	     */
    	    IX_OSAL_ASSERT(retval == IX_ATMSCH_RET_QUEUE_EMPTY);
    	
    	    /*
    	     * The schedule table is empty so the port has gone
    	     * IDLE. Free the BUSY mutex.
    	     */
    	    IX_ATMM_TX_MUTEX_UNLOCK(port);
        }
        ixOsalIrqUnlock(lockKey); 
    	IX_ATMM_PORT_MUTEX_UNLOCK(port);
    }
}


PRIVATE IX_STATUS
ixAtmmRxHiPriorityInit (void)
{
    static BOOL rxHiPriorityInitDone = FALSE;
    IX_STATUS retval = IX_SUCCESS;

    /* Initialization done in first call to Init,
     *  subsequent calls do nothing
     */       
    if (!rxHiPriorityInitDone)
    {
	/* 
	 * Hook Rx notification to Atmd rx process perform
	 * this will notify us whenever there is data available.
	 */
	retval = ixAtmdAccRxDispatcherRegister (IX_ATM_RX_A,
						ixAtmdAccRxDispatch);
	if (retval == IX_SUCCESS)
	{
	    rxHiPriorityInitDone = TRUE;
	}
    }

    return retval;
}


PRIVATE IX_STATUS
ixAtmmRxLoPriorityInit (void)
{
    static BOOL rxLoPriorityInitDone = FALSE;
    IxOsalThreadAttr ixAtmmRxLoPriorityThreadAttr;
    IX_STATUS retval = IX_SUCCESS;
    IX_STATUS retStatus;
    char *threadName = "Atmm Rx Low Priority Thread";

    /* Initialization done in first call to Init,
     *  subsequent calls do nothing
     */       
    if (!rxLoPriorityInitDone)
    {
	/*
	 * Rx Low priority queue will be processed from
	 * a timer.
	 */
	ixAtmmRxLoPriorityThreadAttr.name = threadName;
	ixAtmmRxLoPriorityThreadAttr.stackSize = 0;
	ixAtmmRxLoPriorityThreadAttr.priority = IX_ATMM_THREAD_PRI_HIGH;
        if ((retStatus = ixOsalThreadCreate( 
                    	     &ixAtmmTaskIds.rxLoPriorityId,
			     &ixAtmmRxLoPriorityThreadAttr,
			     (IxOsalVoidFnVoidPtr) ixAtmmRxLoPriorityLoop,
			     NULL)) ==
              IX_SUCCESS )
        {
	    /* start the thread */
	    retStatus |= ixOsalThreadStart(&ixAtmmTaskIds.rxLoPriorityId);
	}
	
	if (IX_SUCCESS != retStatus)
	{
	    retval = IX_FAIL;
	}
	else
	{

	    rxLoPriorityInitDone = TRUE;
        } 
    }

    return retval;
}


PRIVATE IX_STATUS
ixAtmmTxDoneInit (void)
{
    static BOOL txDoneInitDone = FALSE;
    IxOsalThreadAttr ixAtmmTxDoneThreadAttr;
    IX_STATUS retval = IX_SUCCESS;
    IX_STATUS retStatus;
    unsigned qSize;
    char *threadName = "Atmm Tx Done Thread";

    /* Initialization done in first call to Init,
     *  subsequent calls do nothing
     */       
    if (!txDoneInitDone)
    {
	/*
	 * Get depth of TxDone Q
	 */
	retval = ixAtmdAccTxDoneQueueSizeQuery (&qSize);
	if (retval == IX_SUCCESS)
	{
	    /*
	     * Hook Tx Done notification to done handler,
	     * threshold is set to half of q size
	     */      
	    retval = ixAtmdAccTxDoneDispatcherRegister ( qSize/2,
							 ixAtmmTxDoneHandle);
	}    

	if (retval == IX_SUCCESS)
	{
	    /*
	     * Setup timer to handle disconnect scenario
	     * during idle traffic
	     */
	    ixAtmmTxDoneThreadAttr.name = threadName;
	    ixAtmmTxDoneThreadAttr.stackSize = 0;
	    ixAtmmTxDoneThreadAttr.priority = IX_ATMM_THREAD_PRI_HIGH;
	    if ((retStatus = ixOsalThreadCreate( 
		                 &ixAtmmTaskIds.txDoneId,
				 &ixAtmmTxDoneThreadAttr,
				 (IxOsalVoidFnVoidPtr) ixAtmmTxDoneLoop,
				 NULL)) ==
		IX_SUCCESS )
	    {
		/* start the thread */
		retStatus |= ixOsalThreadStart(&ixAtmmTaskIds.txDoneId);
	    } 

	    if (IX_SUCCESS != retStatus)
	    {
		retval = IX_FAIL;
	    }
	    else
	    {
		txDoneInitDone = TRUE;
	    }
	}
    }
    
    return retval;
}


PRIVATE IX_STATUS
ixAtmmTxDoneHandle (unsigned numOfPdusToProcess,
		    unsigned *reservedPtr)
{
    IX_STATUS retval = IX_SUCCESS;
    UINT32 lockKey;

    lockKey = ixOsalIrqLock();

    /* Call the Atmd Tx Done API */
    retval = ixAtmdAccTxDoneDispatch (numOfPdusToProcess,
				      reservedPtr);

    ixOsalIrqUnlock(lockKey); 

    if (retval != IX_SUCCESS)
    {
	retval = IX_FAIL;
    }

    return retval;
}

PRIVATE IX_STATUS 
ixAtmmVcIdGet (IxAtmLogicalPort port,
	       unsigned vpi,
	       unsigned vci,
	       IxAtmConnId connId,
	       IxAtmSchedulerVcId *vcId)
{
    IX_STATUS retval = IX_SUCCESS;
    IxAtmmVc vcDesc;

    /* Callback should be registered before calling this function */
    if (vcQueryCallback == NULL)
    {
	retval = IX_FAIL;
    }

    if (retval == IX_SUCCESS)
    {
	/*
	 * Call the registered VC query callback
	 */
	retval = vcQueryCallback (port, vpi, vci, IX_ATMM_VC_DIRECTION_TX, vcId, &vcDesc);
    }

    if (retval == IX_SUCCESS)
    {
	/*
	 * Now inform AtmSch of the user connId
	 */
	retval = ixAtmSchVcConnIdSet ( port, *vcId, connId);

	if (retval != IX_SUCCESS)
	{
	    retval = IX_FAIL;
	}
    }

    return retval;
}

