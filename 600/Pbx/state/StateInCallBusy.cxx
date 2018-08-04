/*
 * $Id: StateInCallBusy.cxx,v 1.2 2007/03/02 04:19:29 lizhijie Exp $
 */

#include "global.h"
#include "StateInCallBusy.hxx"

#include "OpStartBusyTone.hxx"
#include "OpStopBusyTone.hxx"
#include "OpRevert.hxx"
#include "OpReRing.hxx"
#include "OpAckReInvite.hxx"

using namespace Vocal;

StateInCallBusy::StateInCallBusy()
{
    addEntryOperator( new OpStartBusyTone );

    addOperator( new OpRevert );
    addOperator( new OpReRing );
    addOperator( new OpAckReInvite );

    addExitOperator( new OpStopBusyTone );
}


StateInCallBusy::~StateInCallBusy()
{
}


const char* const
StateInCallBusy::name() const
{
    return "StateInCallBusy";
}
