/* 
 * @file        IxAtmSchServiceIf.c
 *
 *              
 * @date        29-JUL-2000 
 *
 * @brief Implementation file for Service Interface of the Atm scheduler.
 * 
 * Design Notes:
 *    All non data path invoked functionality is contained in this
 *    file.  Other functionality is separated as it is included 
 *    in internal IXP22x RAM.
 *
 *    26-FEB-02 Ported this code from IXP22x and modified to support
 *    multiple ports.
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

#include "IxOsal.h"
#include "IxAtmSch.h"
#include "IxAtmSch_p.h"
#include "IxAtmSchUtils_p.h"

/*
 * Variable declarations global to this file only. Externs are followed by
 * static variables.  */
static unsigned int schPortRate[IX_UTOPIA_MAX_PORTS];
static BOOL schInitDone = FALSE;
static IxAtmTrafficDescriptor schTd[IX_ATM_MAX_NUM_AAL_OAM_TX_VCS];

/* External variables defined in the AtmSch component.*/
extern BOOL               ixAtmSchedulingEnabled[IX_UTOPIA_MAX_PORTS];
extern IxAtmSchVcInfo     ixAtmSchVcTable[];
extern IxAtmSchedulerVcId ixAtmSchNextUbrToBeScheduled[IX_UTOPIA_MAX_PORTS];
extern IxAtmSchStats      ixAtmSchStats[IX_UTOPIA_MAX_PORTS];

/* Function definition */
PRIVATE int
ixAtmSchCacPCRCheck(IxAtmLogicalPort port, IxAtmTrafficDescriptor *trafficDesc);

PRIVATE 
IxAtmSchedulerVcId ixAtmSchFreeVcIdGet( void );

PRIVATE void
ixAtmSchUbrChainVcInsert(IxAtmLogicalPort port, IxAtmSchedulerVcId vcId);

PRIVATE void
ixAtmSchUbrChainVcRemove(IxAtmLogicalPort port, IxAtmSchedulerVcId vcId);

PRIVATE BOOL
ixAtmSchParamIsValid(IxAtmLogicalPort port, IxAtmSchedulerVcId vcId);

/************************************************************************/
PUBLIC IX_STATUS 
ixAtmSchInit(void)
{
    if (!schInitDone ) 
    {
        ixAtmSchedulingInit();
        ixAtmSchStatsClear();
        schInitDone = TRUE;
        return IX_SUCCESS;
    }
    return IX_FAIL;
}

/************************************************************************/
PUBLIC void
ixAtmSchShow(void)
{
    IxAtmSchedulerVcId i;
    IxAtmLogicalPort port;
    unsigned int vcCnt;

    if (!schInitDone) 
    {
    printf("\n=========== IxAtmSch not initialized ==============\n");
    }
    else
    {
    printf("\n================== IxAtmSch State ==================");
    for (port=0; port<IX_UTOPIA_MAX_PORTS; port++)
    {
        if (ixAtmSchedulingEnabled[port])
        {
            printf("\n--- UTOPIA_PORT_%d Info ---",port);
            printf("\nRate = %u, Next UBR VcId = %d, VBR VcId = %d", 
                        schPortRate[port],
                        ixAtmSchNextUbrToBeScheduled[port],
                        ixAtmSchVbrIndexGet(port));
           
            /* */
            printf("\n--- Shaping Statistics:"); 
            printf("\nDemand Update Calls \t= %u", ixAtmSchStats[port].updateCalls);
            printf("\nCells Queued \t\t= %u", ixAtmSchStats[port].cellsQueued);
            printf("\nTable Update Calls \t= %u", ixAtmSchStats[port].scheduleTableCalls);
            printf("\nCells Scheduled \t= %u", ixAtmSchStats[port].cellsScheduled);
            printf("\nIdle Cells Scheduled \t= %u", ixAtmSchStats[port].idleCellsScheduled);
            printf("\nQueue Full Occurences\t= %u", ixAtmSchStats[port].queueFull);

            /* */
            printf("\n--- VC information:");
            printf("\n%-4s %-4s %-8s %-8s %-8s %-4s %-4s %-8s %-7s %-8s",     
                      "VcId",
      	              "serv",
                      "pcr",
                      "scr",
                      "cdvt",
                      "mbs",
                      "mfs",
                      "count",
                      "nextUBR",
                      "connId");

            for (i=0, vcCnt = 0; i<IX_ATM_MAX_NUM_AAL_OAM_TX_VCS; i++)
            {
	        if ((ixAtmSchVcTable[i].inUse == TRUE) &&
                    (ixAtmSchVcTable[i].port == port))
	        {
                    vcCnt++;
	            printf("\n%-4d %-4d %-8u %-8u %-8u %-4u %-4d %-8u %-7d %-8u", 
		            i,
                            schTd[i].atmService,
		            schTd[i].pcr,
		            schTd[i].scr,
		            schTd[i].cdvt,
		            schTd[i].mbs,
		            schTd[i].mfs,
		            ixAtmSchVcTable[i].count,
		            ixAtmSchVcTable[i].nextUBR,
                            ixAtmSchVcTable[i].connId);
	        }
            }
            if ( vcCnt == 0 ) 
            {
	        printf("\nNo VCs"); 
            }
        }
    }         
    printf("\n====================================================\n");
    }
}

PUBLIC void
ixAtmSchStatsClear(void)
{
    IxAtmLogicalPort port;

    /* clear all port statistics */
    for ( port = 0 ; port < IX_UTOPIA_MAX_PORTS ; port++ ) 
    {
        ixAtmSchStats[port].idleCellsScheduled = 0;
        ixAtmSchStats[port].updateCalls = 0;
        ixAtmSchStats[port].cellsQueued = 0;
        ixAtmSchStats[port].cellsScheduled = 0;
        ixAtmSchStats[port].queueFull = 0;
        ixAtmSchStats[port].scheduleTableCalls = 0;
    }

    return;
}

/************************************************************************/
PUBLIC IX_STATUS
ixAtmSchPortModelInitialize( IxAtmLogicalPort port, 
                             unsigned int portRate,
                             unsigned int minCellsToSchedule)
{
    if (!schInitDone||
        (port<IX_UTOPIA_PORT_0)||
        (port>=IX_UTOPIA_MAX_PORTS)||
        (ixAtmSchedulingEnabled[port])||
        (portRate == 0))
    {
	return IX_FAIL;
    }
    else 
    {
	schPortRate[port] = portRate;
	ixAtmSchCellTimeSet(port, (IX_ATMSCH_nS_PER_SECOND / portRate));
        ixAtmSchMinCellsSet(port, minCellsToSchedule);
        ixAtmSchedulingEnabled[port] = TRUE;
	return IX_SUCCESS;
    }
}

/************************************************************************/
PUBLIC IX_STATUS
ixAtmSchPortRateModify( IxAtmLogicalPort port, 
                        unsigned int portRate)
{
    unsigned int vbrTime;
    unsigned int cellTime;
    IxAtmSchedulerVcId vbrVcId;

    if (!schInitDone||
        (port<IX_UTOPIA_PORT_0)||
        (port>=IX_UTOPIA_MAX_PORTS)||
        !ixAtmSchedulingEnabled[port])
    {
        return IX_FAIL;
    }

    cellTime = IX_ATMSCH_nS_PER_SECOND / portRate;

    vbrVcId = ixAtmSchVbrIndexGet(port);

    if (vbrVcId != IX_ATMSCH_NULL_INDEX)
    {
	if( schTd[vbrVcId].pcr > portRate)
	{
	    return IX_FAIL;
	}
	
	/* Calculate #microseconds between each VBR cell.  Uses the 
	 * PCR value as PCR==SCR for this implementation.  CAC function
	 * should have verified that pcr > 0 also.  */
	vbrTime = IX_ATMSCH_nS_PER_SECOND / schTd[vbrVcId].pcr;
	ixAtmSchVbrStateSet( port, vbrTime, vbrVcId );
    }

    schPortRate[port] = portRate;
    ixAtmSchCellTimeSet(port, cellTime);

    return IX_SUCCESS;
}

/************************************************************************/
PRIVATE int
ixAtmSchCacPCRCheck(IxAtmLogicalPort port, IxAtmTrafficDescriptor *trafficDesc)
{
    switch(trafficDesc->atmService)
    {
    case IX_ATM_CBR:    
    case IX_ATM_VBR:
    case IX_ATM_ABR:
	return IX_FAIL;
    case IX_ATM_UBR:
	/* UBR PCR must be at least equal to the port rate */
	if (trafficDesc->pcr < schPortRate[port]) 
	{
	    return IX_ATMSCH_RET_NOT_ADMITTED;
	}
	break;
    case IX_ATM_RTVBR:
	/* Assuming only one RT-VBR VC, it can have a PCR up 
	 * to the port rate.  Also, a simplyfying assumpition 
	 * is made based on Jetstream traffic parameters that 
	 * the PCR is the same as the SCR. 
	 */
	if ( (trafficDesc->pcr > schPortRate[port]) ||
	     (trafficDesc->pcr != trafficDesc->scr) ||
	     (trafficDesc->pcr == 0))
	{
	    return IX_ATMSCH_RET_NOT_ADMITTED;
	}
	break;
    default:
	/* Unknown service type */
	IX_ATMSCH_ASSERT(FALSE);
	return IX_FAIL;
    }
    return IX_SUCCESS;
}

/************************************************************************/
PRIVATE IxAtmSchedulerVcId
ixAtmSchFreeVcIdGet( void ) 
{
    IxAtmSchedulerVcId i;
    
    for (i=0; i<IX_ATM_MAX_NUM_AAL_OAM_TX_VCS; i++) 
    {
	if (ixAtmSchVcTable[i].inUse == FALSE)
	{
	    return i;
	}
    }
    return IX_ATMSCH_NULL_INDEX;
}

/************************************************************************/

PRIVATE void
ixAtmSchUbrChainVcInsert(IxAtmLogicalPort port, IxAtmSchedulerVcId vcId)
{
    IX_ATMSCH_ASSERT(vcId != IX_ATMSCH_NULL_INDEX);

    if (ixAtmSchNextUbrToBeScheduled[port] == IX_ATMSCH_NULL_INDEX) 
    {
	ixAtmSchVcTable[vcId].nextUBR = vcId;
	ixAtmSchNextUbrToBeScheduled[port] = vcId;
    }
    else 
    {
	/* It is important to update the nextUBR fields in this order
	 * as this table may be simultaneously accessed by the
	 * updateTable interrupt. The insert into the chain is atomic.
	 */
	IX_ATMSCH_ASSERT(ixAtmSchVcTable[ixAtmSchNextUbrToBeScheduled[port]].nextUBR 
                         != IX_ATMSCH_NULL_INDEX);
	ixAtmSchVcTable[vcId].nextUBR 
             = ixAtmSchVcTable[ixAtmSchNextUbrToBeScheduled[port]].nextUBR;
	ixAtmSchVcTable[ixAtmSchNextUbrToBeScheduled[port]].nextUBR = vcId;
    }
}

PRIVATE void
ixAtmSchUbrChainVcRemove(IxAtmLogicalPort port, IxAtmSchedulerVcId vcId) 
{
    IxAtmSchedulerVcId i = 0;
    
    for (i=0; i<IX_ATM_MAX_NUM_AAL_OAM_TX_VCS; i++) 
    {
	if ( (ixAtmSchVcTable[i].inUse == TRUE ) && 
	     (ixAtmSchVcTable[i].nextUBR == vcId) &&
             (ixAtmSchVcTable[i].port == port))
	{
	    IX_ATMSCH_ASSERT(ixAtmSchVcTable[vcId].nextUBR != IX_ATMSCH_NULL_INDEX);

	    ixAtmSchVcTable[i].nextUBR = ixAtmSchVcTable[vcId].nextUBR;
	    if (ixAtmSchNextUbrToBeScheduled[port] == vcId) 
	    {
		/* This code is interruptible by the scheduler
		 * function while modifying the value of
		 * ixAtmSchNextUbrToBeScheduled, but the worst consequence is
		 * that it would cause a UBR Vc to be skipped in the
		 * chain */
		if (ixAtmSchVcTable[vcId].nextUBR == vcId)
		{
		    /* this vc is the only one in the chain */
		    ixAtmSchNextUbrToBeScheduled[port] = IX_ATMSCH_NULL_INDEX;
		}
		else
		{
		    ixAtmSchNextUbrToBeScheduled[port] = ixAtmSchVcTable[vcId].nextUBR;
		}
	    }
	    return;
	}
    }
    if (i == IX_ATM_MAX_NUM_AAL_OAM_TX_VCS) 
    {
	IX_ATMSCH_ERROR_REPORT("UBR VC not in UBR chain when removed.");
    }
}

/************************************************************************/
PUBLIC IX_STATUS
ixAtmSchVcModelSetup(IxAtmLogicalPort port, 
		     IxAtmTrafficDescriptor *trafficDesc, 
		     IxAtmSchedulerVcId *vcId)
{
    IX_STATUS retval;
    IxAtmSchedulerVcId vcId_;
    unsigned int vbrTime;

    if (!schInitDone||
        (port<IX_UTOPIA_PORT_0)||
        (port>=IX_UTOPIA_MAX_PORTS)||
        !ixAtmSchedulingEnabled[port])
    {
        return IX_FAIL;
    }
    
    retval = ixAtmSchCacPCRCheck(port ,trafficDesc);
    if (retval != IX_SUCCESS) 
    {
	return retval;
    }

    vcId_ = ixAtmSchFreeVcIdGet();
    if (vcId_ == IX_ATMSCH_NULL_INDEX) 
    {
	return IX_ATMSCH_RET_NOT_ADMITTED;
    }

    ixAtmSchVcTable[vcId_].count  = 0;
    ixAtmSchVcTable[vcId_].connId = IX_ATM_IDLE_CELLS_CONNID;
    ixAtmSchVcTable[vcId_].port   = port;

    schTd[vcId_] = *trafficDesc;
    if (schTd[vcId_].atmService == IX_ATM_UBR) 
    {
	ixAtmSchUbrChainVcInsert(port,vcId_);
    }
    else if (schTd[vcId_].atmService == IX_ATM_RTVBR)
    {
	/* Only one VBR VC */
	if ( ixAtmSchVbrIndexGet(port) != IX_ATMSCH_NULL_INDEX) 
	{
	    return IX_ATMSCH_RET_NOT_ADMITTED;
	}
	else 
	{
	    /* The length of time taken to transmit minCellsToSchedule
	     * should not exceed the CDVT value of the VBR VC. 	     */
	    if ( schTd[vcId_].cdvt <=
                 (ixAtmSchMinCellsGet(port) * ixAtmSchCellTimeGet(port)))
            { 
                IX_ATMSCH_WARNING_REPORT( 
                    "IxAtmSch - minCellsToSchedule incompatible with CDVT"); 

	        return IX_ATMSCH_RET_NOT_ADMITTED;
            }

	    /* Calculate #microseconds between each VBR cell.  Uses the 
	     * PCR value as PCR==SCR for this implementation.  CAC function
	     * should have verified that pcr > 0 also.  */
	    vbrTime = IX_ATMSCH_nS_PER_SECOND / schTd[vcId_].pcr;

	    ixAtmSchVbrStateSet( port, vbrTime, vcId_ );
	    ixAtmSchVcTable[vcId_].nextUBR = IX_ATMSCH_NULL_INDEX;
	}
    }
    else
    {
	/* Unknown service type! */
	IX_ATMSCH_ASSERT( FALSE );
    }
    ixAtmSchVcTable[vcId_].inUse = TRUE;

    *vcId = vcId_;
    return IX_SUCCESS;
}

/************************************************************************/
PRIVATE BOOL
ixAtmSchParamIsValid(IxAtmLogicalPort port, IxAtmSchedulerVcId vcId)
{
    if ((vcId >= IX_ATM_MAX_NUM_AAL_OAM_TX_VCS) || 
	(vcId < 0)||
        (ixAtmSchVcTable[vcId].inUse == FALSE)||
        (ixAtmSchVcTable[vcId].port != port)) 
    {
	return FALSE;
    }
    return TRUE;
}

/************************************************************************/
PUBLIC IX_STATUS
ixAtmSchVcConnIdSet( IxAtmLogicalPort port,
                     IxAtmSchedulerVcId vcId,
                     IxAtmConnId connId)
{
    if (!schInitDone||
        ixAtmSchParamIsValid(port, vcId) == FALSE)
    {
	return IX_FAIL;
    }
    
    ixAtmSchVcTable[vcId].connId = connId;

    return IX_SUCCESS;
}

PUBLIC IX_STATUS
ixAtmSchVcModelRemove(IxAtmLogicalPort port, IxAtmSchedulerVcId vcId)
{
    if (!schInitDone||
        ixAtmSchParamIsValid(port, vcId) == FALSE)
    {
	return IX_FAIL;
    }
    if (schTd[vcId].atmService == IX_ATM_UBR) 
    {
	ixAtmSchUbrChainVcRemove( port, vcId );
    }
    else if (schTd[vcId].atmService == IX_ATM_RTVBR)
    {
	IX_ATMSCH_ASSERT( ixAtmSchVbrIndexGet(port) == vcId);
	ixAtmSchVbrStateSet( port, 0, IX_ATMSCH_NULL_INDEX); /* vbrTime, vbrIndex */
    }
    else
    {
	IX_ATMSCH_ASSERT( FALSE );
    }

    if (ixAtmSchVcTable[vcId].count != 0)
    {
	IX_ATMSCH_WARNING_REPORT( "Removal of VC with pending cells!");
    }

    ixAtmSchVcTable[vcId].inUse = FALSE;
    ixAtmSchVcTable[vcId].port  = IX_UTOPIA_MAX_PORTS;

    return IX_SUCCESS;
}

/************************************************************************/

