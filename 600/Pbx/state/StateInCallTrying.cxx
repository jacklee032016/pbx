/*
 * $Id: StateInCallTrying.cxx,v 1.2 2007/03/02 04:19:29 lizhijie Exp $
 */

#include "global.h"
#include "StateInCallTrying.hxx"

#include "OpHeldCallEndedTrying.hxx"
#include "OpRevert.hxx"
#include "OpInCallStopTrying.hxx"
#include "OpStartRingbackTone.hxx"
#include "OpReDirect.hxx"
#include "OpStartBlindXferring.hxx"
#include "OpInCallFarEndAnswered.hxx"
#include "OpInCallBusy.hxx"
#include "OpInCallFastBusy.hxx"
#include "OpStopRingbackTone.hxx"

using namespace Vocal;

StateInCallTrying::StateInCallTrying()
{
    addOperator( new OpHeldCallEndedTrying );
    addOperator( new OpInCallStopTrying );
    addOperator( new OpRevert );
    addOperator( new OpStartRingbackTone );
    addOperator( new OpReDirect );
    addOperator( new OpStartBlindXferring );
    addOperator( new OpInCallFarEndAnswered );
    addOperator( new OpInCallBusy );
    addOperator( new OpInCallFastBusy );

    addExitOperator( new OpStopRingbackTone );
}


StateInCallTrying::~StateInCallTrying()
{
}


const char* const
StateInCallTrying::name() const
{
    return "StateInCallTrying";
}
