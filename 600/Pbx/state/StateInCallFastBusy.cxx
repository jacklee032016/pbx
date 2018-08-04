/*
 * $Id: StateInCallFastBusy.cxx,v 1.2 2007/03/02 04:19:29 lizhijie Exp $
 */

#include "global.h"
#include "StateInCallFastBusy.hxx"

#include "OpStartFastBusyTone.hxx"
#include "OpStopFastBusyTone.hxx"
#include "OpRevert.hxx"
#include "OpReRing.hxx"
#include "OpAckReInvite.hxx"

using namespace Vocal;

StateInCallFastBusy::StateInCallFastBusy()
{
    addEntryOperator( new OpStartFastBusyTone );

    addOperator( new OpRevert );
    addOperator( new OpReRing );
    addOperator( new OpAckReInvite );

    addExitOperator( new OpStopFastBusyTone );
}


StateInCallFastBusy::~StateInCallFastBusy()
{
}


const char* const
StateInCallFastBusy::name() const
{
    return "StateInCallFastBusy";
}
