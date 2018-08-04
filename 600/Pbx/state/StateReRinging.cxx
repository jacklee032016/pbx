/*
 * $Id: StateReRinging.cxx,v 1.2 2007/03/02 04:19:29 lizhijie Exp $
 */

#include "global.h"
#include "StateReRinging.hxx"

#include "OpStartRinging.hxx"
#include "OpStopRinging.hxx"
#include "OpRevert.hxx"
#include "OpActiveCallEnded.hxx"
#include "OpAckReInvite.hxx"

using namespace Vocal;

StateReRinging::StateReRinging()
{
    //    addOperator( new OpInCallDialing );
    addEntryOperator( new OpStartRinging );

    addOperator( new OpRevert );
    addOperator( new OpActiveCallEnded );
    addOperator( new OpAckReInvite );

    addExitOperator( new OpStopRinging );
}


StateReRinging::~StateReRinging()
{}


const char* const
StateReRinging::name() const
{
    return "StateReRinging";
}
