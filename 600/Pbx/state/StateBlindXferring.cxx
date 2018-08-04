/*
 * $Id: StateBlindXferring.cxx,v 1.2 2007/03/02 04:19:28 lizhijie Exp $
 */

#include "global.h"
#include "StateBlindXferring.hxx"

#include "OpXferHold.hxx"
#include "OpXferTargetInviteOk.hxx"
#include "OpBlindXferHoldOk.hxx"
#include "OpXferEnded.hxx"
#include "OpXferComplete.hxx"
#include "OpXferDenied.hxx"

using namespace Vocal;


StateBlindXferring::StateBlindXferring()
{
    addEntryOperator( new OpXferHold );

    addOperator( new OpXferTargetInviteOk );
    addOperator( new OpBlindXferHoldOk );
    addOperator( new OpXferComplete );
    addOperator( new OpXferEnded );
    addOperator( new OpXferDenied );
}


StateBlindXferring::~StateBlindXferring()
{
}


const char* const
StateBlindXferring::name() const
{
    return "StateBlindXferring";
}
