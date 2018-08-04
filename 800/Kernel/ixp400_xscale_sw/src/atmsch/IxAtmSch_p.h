/*
 * @file        IxAtmSch_p.h
 * @author Intel Corporation
 * @date        13-12-2000  Ported:      1-Mar-2002
 *
 * @brief Private header file for AtmSch component.
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

#ifndef IXATMSCH_P_H
#define IXATMSCH_P_H

#include "IxOsalTypes.h"
#include "IxAtmTypes.h"

/*
 * Defines
 */
#define IX_ATMSCH_nS_PER_SECOND 1000000000
#define IX_ATMSCH_MAX_TABLE_ENTRIES  10
#define IX_ATMSCH_NULL_INDEX         -1

/*
 * Typedefs
 */
typedef struct
{
    BOOL inUse;			/* indicates whether the table
				 * element is currently in use */
    IxAtmLogicalPort port;      /* The port on which the VC is enabled */
    unsigned int count;		/* a count of the queued cells for the VC */
    int nextUBR;		/* the next UBR VC in the UBR chain */
    IxAtmConnId connId;         /* connId which the scheduling client knows
                                   the VC. */
} IxAtmSchVcInfo;

typedef struct _IxAtmSchStats
{
    unsigned int idleCellsScheduled;
    unsigned int cellsQueued;
    unsigned int updateCalls;
    unsigned int queueFull;
    unsigned int cellsScheduled;
    unsigned int scheduleTableCalls;
} IxAtmSchStats;

/*
 * Function Prototypes
 */
void
ixAtmSchedulingInit(void);
void
ixAtmSchCellTimeSet(IxAtmLogicalPort port, unsigned int cellTime );
unsigned int
ixAtmSchCellTimeGet(IxAtmLogicalPort port);
void
ixAtmSchMinCellsSet(IxAtmLogicalPort port, unsigned int minCellsToSchedule);
unsigned int
ixAtmSchMinCellsGet(IxAtmLogicalPort port);
void
ixAtmSchVbrStateSet(IxAtmLogicalPort port, unsigned int vbrTime, int vbrIndex);
int
ixAtmSchVbrIndexGet(IxAtmLogicalPort port);

#endif /* IXATMSCH_P_H */
