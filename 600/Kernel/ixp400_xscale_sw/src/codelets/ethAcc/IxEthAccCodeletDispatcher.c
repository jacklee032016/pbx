/**
 * @file IxEthAccCodeletDispatcher.c
 *
 * @date 22 April 2002
 *
 * @brief This file contains the implementation of the Ethernet Access 
 * Codelet that implements the QMgr entry point. All the codelet 
 * traffic is configured to run thru the QMgr disaptcher callbacks.
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
 * Put the system defined include files required.
 */
#include "IxOsal.h"

/*
 * Put the user defined include files required.
 */
#include "IxQMgr.h"
#include "IxEthAccCodelet.h"
#include "IxEthAccCodelet_p.h"

/* Qmgr interrupt */
#define IX_ETH_CODELET_QMGR_IRQ IX_OSAL_IXP400_QM1_IRQ_LVL

/*
 * Variable declarations global to this file only.
 */
PRIVATE IxQMgrDispatcherFuncPtr ixEthAccCodeletDispatcherFunc;
PRIVATE IxOsalMutex ixEthAccCodeletDispatcherPollRunning;
PRIVATE volatile BOOL ixEthAccCodeletDispatcherPollStopTrigger = FALSE;
PRIVATE BOOL ixEthAccCodeletDispatcherInitialized = FALSE;

/**
 * Function definition: ixEthAccCodeletDispatcherPoll
 *
 * This function polls the Queue manager loop.
 *
 */

PRIVATE void 
ixEthAccCodeletDispatcherPoll (void* arg, void** ptrRetObj)
{
    if (ixOsalMutexLock (&ixEthAccCodeletDispatcherPollRunning,
			 IX_OSAL_WAIT_FOREVER) != IX_SUCCESS)
    {
        printf("Dispatcher: Error starting QMgr Dispatcher thread! Failed to lock mutex.\n");
        return;
    }

    ixEthAccCodeletDispatcherPollStopTrigger = FALSE;

    while (1)
    {
        if (ixEthAccCodeletDispatcherPollStopTrigger)
        {
            break;  /* Exit the loop */
        }

	ixOsalYield();

	(*ixEthAccCodeletDispatcherFunc) (IX_QMGR_QUELOW_GROUP);
    }

    ixOsalMutexUnlock (&ixEthAccCodeletDispatcherPollRunning); 

    /* Exit the thread */
}


/**
 * Function definition: ixEthAccCodeletDispatcherStart()
 *
 * This function starts the Queue manager dispatch loop.
 * 
 */

IX_STATUS 
ixEthAccCodeletDispatcherStart(BOOL useInterrupt)
{ 
    IxOsalThread qDispatchThread;
    IxOsalThreadAttr threadAttr;

    threadAttr.name      = "Codelet Q Dispatcher";
    threadAttr.stackSize = 32 * 1024; /* 32kbytes */
    threadAttr.priority  = IX_ETHACC_CODELET_QMR_PRIORITY;

    if (!ixEthAccCodeletDispatcherInitialized)
    {
	/* this should be initialized once */
	ixQMgrDispatcherLoopGet(&ixEthAccCodeletDispatcherFunc);

	ixOsalMutexInit (&ixEthAccCodeletDispatcherPollRunning);
	ixEthAccCodeletDispatcherPollStopTrigger = TRUE;
	ixEthAccCodeletDispatcherInitialized = TRUE;
    }

    if(useInterrupt)	/* Interrupt mode */
    {
	/* 
	 * Hook the QM QLOW dispatcher to the interrupt controller. 
	 */
	if (ixOsalIrqBind(IX_ETH_CODELET_QMGR_IRQ,
			  (IxOsalVoidFnVoidPtr)(ixEthAccCodeletDispatcherFunc),
			  (void *)IX_QMGR_QUELOW_GROUP) != IX_SUCCESS)
	{
	    printf("Dispatcher: Failed to bind to QM1 interrupt\n");
	    return (IX_FAIL);
	}
    }
    else
    {  
	if (ixEthAccCodeletDispatcherPollStopTrigger)
	{
	    /* Polled mode based on a task running a loop */
	    if (ixOsalThreadCreate(&qDispatchThread,
				   &threadAttr,
				   (IxOsalVoidFnVoidPtr) ixEthAccCodeletDispatcherPoll,
				   NULL)	
		!= IX_SUCCESS)
	    {
		printf("Dispatcher: Error spawning Q Dispatcher task\n");
		return (IX_FAIL);
	    }
	    
	    /* Start the thread */
	    if (ixOsalThreadStart(&qDispatchThread) != IX_SUCCESS)
	    {
		printf("Dispatcher: Error failed to start the Q Dispatcher thread\n");
		return IX_FAIL;
	    }
	}
    }

    return (IX_SUCCESS);
}

/**
 * Function definition: ixEthAccCodeletDispatcherStop()
 *
 * This function stops the Queue manager dispatch loop.
 *
 */

IX_STATUS 
ixEthAccCodeletDispatcherStop(BOOL useInterrupt)
{
    if(useInterrupt) /* Interrupt mode */
    {
	/* 
	 * Unhook the QM QLOW dispatcher to the interrupt controller. 
	 */
	if (ixOsalIrqUnbind(IX_ETH_CODELET_QMGR_IRQ) != IX_SUCCESS)
	{
	    printf("Dispatcher: Failed to unbind to QM1 interrupt\n");
	    return (IX_FAIL);
	}
    }
    else
    {
	if (!ixEthAccCodeletDispatcherPollStopTrigger)
	{
	    ixEthAccCodeletDispatcherPollStopTrigger = TRUE;
	    if (ixOsalMutexLock (&ixEthAccCodeletDispatcherPollRunning,
				 IX_OSAL_WAIT_FOREVER) != IX_SUCCESS)
	    {
		printf("Dispatcher: Error stopping QMgr Dispatcher thread!\n");
		return (IX_FAIL);
	    }
	    ixOsalMutexUnlock (&ixEthAccCodeletDispatcherPollRunning);
	}
    }

    return (IX_SUCCESS);
}
