/*
 * $Id: StateOnhold.cxx,v 1.2 2007/03/02 04:19:29 lizhijie Exp $
 */

#include "global.h"
#include "StateOnhold.hxx"

#include "OpRevert.hxx"
#include "OpHoldTargetOk.hxx"
#include "OpReDirect.hxx"
#include "OpHoldTargetErr.hxx"
#include "OpEndCall.hxx"
#include "OpReRing.hxx"

using namespace Vocal;

StateOnhold::StateOnhold()
{
    addOperator( new OpRevert );
    addOperator( new OpHoldTargetOk );
    addOperator( new OpReDirect );
    addOperator( new OpEndCall );
    addOperator( new OpReRing );
    addOperator( new OpHoldTargetErr );
}


StateOnhold::~StateOnhold()
{
}


const char* const
StateOnhold::name() const
{
    return "StateOnhold";
}
