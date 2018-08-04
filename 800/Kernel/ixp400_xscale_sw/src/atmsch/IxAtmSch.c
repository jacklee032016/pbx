/* 
 * @file        IxAtmSch.c
 * @author Intel Corporation
 * @date        29-JUL-2000
 *
 * @brief Implementiation file for the example Atm scheduler.
 * 
 * Design Notes:
 *    14-SEP-00  A different implementation of the UBR chaining
 *               which keeps only those UBR VCs which have pending
 *               cells on the chain is available in CVS version 1.16.
 *    18-DEC-00  Added a 'sch' prefix to all global variables.  The 
 *               coding standards specify that this prefix should be
 *               IxAtmSch, but due to the complexity of these 
 *               functions, names are being maintained as short as
 *               possible to retain readability.
 *    02-JAN-01  This file is compiled automatically into the BSP
 *               by the ixp22x BSP Makefile.  It is 
 *               not, therefore, built by a standard arm_sw build.
 *
 *    26-FEB-02  Ported this code from IXP22X. Expanded to support
 *               multiple ports. Adjusted API to accommodate new 
 *               IxAtmdAcc component.
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

#include "IxOsal.h"
#include "IxAtmSch.h"
#include "IxAtmSch_p.h"
#include "IxAtmSchUtils_p.h"

/*
 * #defines and macros used in this file.
 */
#define IX_ATMSCH_QUEUE_LIMIT 100000

typedef struct 
{
    unsigned int timePerCell;       /* Time per cell in us */
    unsigned int interVbrTime;      /* Ideal time between VBR cells in us */
    IxAtmSchedulerVcId vbrVcId; /* VcId for the single vbr VC on the port */
    
    unsigned int vbrDeadline;       /* Time at which the next VBR cell should be sent */
    unsigned int currentSchTime;    /* Current vitual time */
    unsigned int minCellsToSchedule;/* minimum number of cells to schedule when there */
                                /* is at least one cell available to transmit */
    
    IxAtmScheduleTable schTable;/* A pointer to the schedule table for this port */
    IxAtmScheduleTableEntry schTableEntries[IX_ATMSCH_MAX_TABLE_ENTRIES]; /* The Sched table */

}IxAtmSchPortSchedulingInfo;

/*
 * Variable declarations global to this file only. Externs are followed by
 * static variables.  */

/* Holds the per port scheduling state information */
static IxAtmSchPortSchedulingInfo portSchedulingInfo[IX_UTOPIA_MAX_PORTS];

/* 
 * Variable declarations used by other source files in this component 
 */
BOOL               ixAtmSchedulingEnabled[IX_UTOPIA_MAX_PORTS];
IxAtmSchedulerVcId ixAtmSchNextUbrToBeScheduled[IX_UTOPIA_MAX_PORTS];
IxAtmSchVcInfo     ixAtmSchVcTable[IX_ATM_MAX_NUM_AAL_OAM_TX_VCS]; 
IxAtmSchStats      ixAtmSchStats[IX_UTOPIA_MAX_PORTS]; 
/************************************************************************/
void
ixAtmSchedulingInit(void)
{
    IxAtmLogicalPort i;

    /* Initialise the scheduling info for all ports */
    ixOsalMemSet(portSchedulingInfo, 0, sizeof(portSchedulingInfo));

    for (i=0; i<IX_UTOPIA_MAX_PORTS; i++) 
    {
        portSchedulingInfo[i].vbrVcId   = IX_ATMSCH_NULL_INDEX;    
        ixAtmSchNextUbrToBeScheduled[i] = IX_ATMSCH_NULL_INDEX;
        ixAtmSchedulingEnabled[i]       = FALSE;
    }

    for (i=0; i<IX_ATM_MAX_NUM_AAL_OAM_TX_VCS; i++)
    {
        ixAtmSchVcTable[i].inUse = FALSE;
        ixAtmSchVcTable[i].port  = IX_UTOPIA_MAX_PORTS;
    }
}

/************************************************************************/

void
ixAtmSchCellTimeSet(IxAtmLogicalPort port, unsigned int cellTime)
{
    portSchedulingInfo[port].timePerCell = cellTime;
}

/************************************************************************/

unsigned int
ixAtmSchCellTimeGet(IxAtmLogicalPort port)
{
    return portSchedulingInfo[port].timePerCell;
}

/************************************************************************/

void
ixAtmSchMinCellsSet(IxAtmLogicalPort port, unsigned int minCellsToSchedule)
{
    portSchedulingInfo[port].minCellsToSchedule = minCellsToSchedule;
}

/************************************************************************/

unsigned int
ixAtmSchMinCellsGet(IxAtmLogicalPort port)
{
    return portSchedulingInfo[port].minCellsToSchedule;
}

/************************************************************************/

void
ixAtmSchVbrStateSet(IxAtmLogicalPort port, unsigned int vbrTime, IxAtmSchedulerVcId vbrVcId)
{
    portSchedulingInfo[port].interVbrTime = vbrTime;
    portSchedulingInfo[port].vbrVcId = vbrVcId;
}

int
ixAtmSchVbrIndexGet(IxAtmLogicalPort port)
{
    return portSchedulingInfo[port].vbrVcId;
}

/************************************************************************/
PUBLIC IX_STATUS 
ixAtmSchVcQueueUpdate( IxAtmLogicalPort port, 
                       IxAtmSchedulerVcId vcId, 
                       unsigned int numberOfCells)
{
    /* 
     * Having the tests inline saves us 25% of the time in this
     * function. Putting the most common scenario in as the successful
     * branch of the if statement saves another 10%.  
     */

    if ((vcId < IX_ATM_MAX_NUM_AAL_OAM_TX_VCS) &&
	(vcId >= 0) &&
	(ixAtmSchVcTable[vcId].inUse == TRUE)&&
	(ixAtmSchVcTable[vcId].port  == port))
    {
	/* This is hard to read but is a slightly faster than doing a
	   seperate increment and test */
	if ((ixAtmSchVcTable[vcId].count += numberOfCells) < IX_ATMSCH_QUEUE_LIMIT)
	{
	    ixAtmSchStats[port].updateCalls++;
	    ixAtmSchStats[port].cellsQueued += numberOfCells;
	    return IX_SUCCESS;
	}
	else
	{
	    /* doing this is faster than a test and increment on every
	       call, we will rarely go inside this  */
	    ixAtmSchVcTable[vcId].count -= numberOfCells;
	    ixAtmSchStats[port].queueFull ++;
	    return IX_ATMSCH_RET_QUEUE_FULL;
	}
    }
    return IX_FAIL;
}

/************************************************************************/
PUBLIC IX_STATUS
ixAtmSchVcQueueClear( IxAtmLogicalPort port, 
                      IxAtmSchedulerVcId vcId)
{
    if ((vcId < IX_ATM_MAX_NUM_AAL_OAM_TX_VCS) &&
	(vcId >= 0) &&
        (ixAtmSchVcTable[vcId].inUse == TRUE)&&
	(ixAtmSchVcTable[vcId].port  == port))
    {
	ixAtmSchVcTable[vcId].count = 0;
	return IX_SUCCESS;
    }
    return IX_FAIL;
}


/************************************************************************/
/* This function is big because we do not have the performance budget
 * to make sub functions out of it. 
 *
 * This is called when the client decides it needs a new sequence of
 * cells to send (probably because the transmit queue is near to
 * empty for this ATM port)
 *
 * This function cannot be interrupted by a call to ubrVcChainRemove 
 * since such a call could cause errors through a non-atomic modification 
 * of the ixAtmSchNextUbrToBeScheduled variable.
 *
 * SchTime in this function is maintained in microseconds and is updated
 * by the SEND_CELLS macro every time cells are sent.
 *
 * The algorithm will schedule at least the lesser of minCellsToSchedule
 * cells and maxCells. The value of minCellsToSchedule should be a trade-off
 * between maximum VBR cell delay (low value) and efficient execution of the sch 
 * table (high value). Tuning of this value is expected to be performed on a
 * per-application basis.
 */

/* This "inline" function is only used in the following ixAtmSchTableUpdate() */
#define SEND_CELLS(COUNT, CONNID, TBL_INDEX)   \
{					       \
    portSchInfo->schTableEntries[(TBL_INDEX)].connId        = (CONNID);\
    portSchInfo->schTableEntries[(TBL_INDEX)].numberOfCells = (COUNT); \
    currentSchTime += (timePerCell * (COUNT)); \
    schTable->tableSize = (TBL_INDEX)+1;       \
    cellCount += (COUNT);		       \
}

PUBLIC IX_STATUS
ixAtmSchTableUpdate(IxAtmLogicalPort port, 
                    unsigned int maxCells,
                    IxAtmScheduleTable **retTable)
{
    int i;
    unsigned int numUbrToSend;
    UINT32 cellCount = 0;
    IxAtmSchPortSchedulingInfo *portSchInfo;
    unsigned int timePerCell;
    unsigned int currentSchTime;
    unsigned int vbrDeadline;
    IxAtmSchedulerVcId prevUBR = -1;
    IxAtmSchedulerVcId vbrVcId;
    IxAtmSchedulerVcId schNextUbrToBeScheduled;
    IxAtmScheduleTable *schTable;

    /* Update the count for calls to this function */
    ixAtmSchStats[port].scheduleTableCalls ++;

    if ((port>=IX_UTOPIA_PORT_0) && 
        (port< IX_UTOPIA_MAX_PORTS)&&
        (ixAtmSchedulingEnabled[port]))
    {
        portSchInfo = &portSchedulingInfo[port];

        /* make a local copy */ 
        timePerCell    = portSchInfo->timePerCell;
        currentSchTime = portSchInfo->currentSchTime;
        vbrDeadline    = portSchInfo->vbrDeadline;
        vbrVcId        = portSchInfo->vbrVcId;

        schNextUbrToBeScheduled = ixAtmSchNextUbrToBeScheduled[port];

        /* Point to the sceduler table for this port */
        schTable            = &portSchInfo->schTable;
        schTable->tableSize = 0;                   

        if (currentSchTime > IX_ATMSCH_nS_PER_SECOND)
	{
	    currentSchTime = 0;
	    vbrDeadline    = 0;
	}

	for (i=0;(i<IX_ATMSCH_MAX_TABLE_ENTRIES)&&(cellCount < maxCells); i++) 
	{
	    if (currentSchTime >= vbrDeadline)
	    {
                /* We are at or beyond the dealine for a VBR Cells */
            	/* First schedule one VBR cell if any are available */
		if ( (vbrVcId != IX_ATMSCH_NULL_INDEX)                  /* Test for an active VBR VC  */
                   &&(ixAtmSchVcTable[vbrVcId].count > 0)) /* Test for demand on that VC */
                   
		{
                    IX_OSAL_ENSURE((ixAtmSchVcTable[vbrVcId].port == port), 
				   "IxAtmSch Major internal error" );
		    SEND_CELLS(1, ixAtmSchVcTable[vbrVcId].connId , i);

		    ixAtmSchVcTable[vbrVcId].count--;

		    /* Update the dealine based on the intercell gap calculate from
                       the VC descriptor. */
                    vbrDeadline += portSchInfo->interVbrTime;

                    /* Check that the deadline for the next VBR is in the future */
		    if (vbrDeadline > currentSchTime)
		    {
		        /* calculate the number of UBR to fill to the next VBR cell */
                	numUbrToSend = ((vbrDeadline - currentSchTime) / timePerCell) + 1;

                        /* Advance to the next table entry */
			i++;
		    }
		    else
		    {
                        /* The deadline is now or in the past so the next  cell to be 
                           scheduled should be a VBR. */
			numUbrToSend = 0;
		    }
		}
		else
		{
		    /* Under Jetstream parameters, the long-term SCR
		     * will never be attained.  Resetting the dealine
		     * to the current currentSchTime in the event of there being
		     * no VBR cells to queue avoids the problem of VBR
		     * bursting to catch up with the SCR. */
		    vbrDeadline = currentSchTime;

		    if (cellCount < portSchInfo->minCellsToSchedule)
		    {
			numUbrToSend = portSchInfo->minCellsToSchedule - cellCount;
		    }
		    else
		    {
			break; /* for (i ...) */
		    }
		}
	    }
	    else
	    {
		/* Leading UBR cells in this table, recalculate the
		 * number of UBR cells to send based on the current
		 * vbrDeadline and currentSchTime. */
		numUbrToSend = ((vbrDeadline - currentSchTime) / timePerCell) + 1;
	    }

            /* Modify the number of UBR so the maxCells is not exceeded */
            if ( (numUbrToSend+cellCount) > maxCells )
            {
                numUbrToSend = maxCells - cellCount;
            }

	    /* Now fill the gap between VBR cells with cells from the
	     * UBR VCs. */
	    while ((numUbrToSend > 0) && (i < IX_ATMSCH_MAX_TABLE_ENTRIES))
	    {
		if ( (schNextUbrToBeScheduled != IX_ATMSCH_NULL_INDEX) &&
		     (ixAtmSchVcTable[schNextUbrToBeScheduled].count >= numUbrToSend) )
		{
                    /* CASE 1 - enough UBR in this VC to fulfill numUbrToSend */
                    IX_OSAL_ENSURE((ixAtmSchVcTable[schNextUbrToBeScheduled].port == port),
                                   "IxAtmSch Major internal error") ;
		    
                    SEND_CELLS (numUbrToSend, 
                                ixAtmSchVcTable[schNextUbrToBeScheduled].connId, 
                                i);

		    ixAtmSchVcTable[schNextUbrToBeScheduled].count -= numUbrToSend;
		    numUbrToSend = 0;
		}
		else if ( (schNextUbrToBeScheduled != IX_ATMSCH_NULL_INDEX) &&
			  (ixAtmSchVcTable[schNextUbrToBeScheduled].count > 0) )
		{
		    /* CASE 2 - some, but not enough UBR in this VC to fulfill numUbrToSend */
                    IX_OSAL_ENSURE((ixAtmSchVcTable[schNextUbrToBeScheduled].port == port),
                                   "IxAtmSch Major internal error") ;
		    
                    SEND_CELLS(ixAtmSchVcTable[schNextUbrToBeScheduled].count, 
                               ixAtmSchVcTable[schNextUbrToBeScheduled].connId, 
			       i);
		    numUbrToSend -= ixAtmSchVcTable[schNextUbrToBeScheduled].count;
		    ixAtmSchVcTable[schNextUbrToBeScheduled].count = 0;
                    
		    i++;
		}
		else if ( (schNextUbrToBeScheduled == IX_ATMSCH_NULL_INDEX) ||
			  (prevUBR == schNextUbrToBeScheduled) )
		{
		    /* CASE 3 - There is no UBR enabled or we 
                     * have gone all the way around the UBR
		     * chain and there is no data in it. If we
		     * have VBR cells or we have sent UBR this
		     * time we insert required idle
		     * cells. Otherwise the table is empty.
		     */
		    if ( ( (vbrVcId != IX_ATMSCH_NULL_INDEX) && 
			   (ixAtmSchVcTable[vbrVcId].count > 0)) ||
			 (cellCount > 0) )
		    {
			SEND_CELLS(numUbrToSend, 
                                   IX_ATM_IDLE_CELLS_CONNID, 
                                   i);
			ixAtmSchStats[port].idleCellsScheduled += numUbrToSend;
			numUbrToSend = 0;
		    }
		    else
		    {
			*retTable = NULL;
                        portSchInfo->currentSchTime = 0;
                        portSchInfo->vbrDeadline    = 0;
                        ixAtmSchNextUbrToBeScheduled[port] = schNextUbrToBeScheduled;
			
                        return IX_ATMSCH_RET_QUEUE_EMPTY;
		    }
		}

		/* Find the next UBR VC with cells, stop if we
		 * have gone all the way around the UBR chain.
		 */
		if (prevUBR != schNextUbrToBeScheduled)
		{
		    prevUBR = schNextUbrToBeScheduled;
		    do
		    {
			schNextUbrToBeScheduled 
			    = ixAtmSchVcTable[schNextUbrToBeScheduled].nextUBR;
		    }
		    while ( (prevUBR != schNextUbrToBeScheduled) &&
			    (ixAtmSchVcTable[schNextUbrToBeScheduled].count == 0) );
		}
	    }
	}

        /* Save the status */
        portSchInfo->currentSchTime = currentSchTime;
        portSchInfo->vbrDeadline    = vbrDeadline;

        ixAtmSchNextUbrToBeScheduled[port] = schNextUbrToBeScheduled;

	schTable->totalCellSlots    = cellCount;
	ixAtmSchStats[port].cellsScheduled    += cellCount;
	schTable->table             = portSchInfo->schTableEntries;

	*retTable = schTable;

	return IX_SUCCESS;
    }
    return IX_FAIL;
}
/************************************************************************/

