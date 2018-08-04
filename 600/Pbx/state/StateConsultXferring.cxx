/*
 * $Id: StateConsultXferring.cxx,v 1.2 2007/03/02 04:19:28 lizhijie Exp $
 */

#include "global.h"
#include "StateConsultXferring.hxx"

#include "OpXferHold.hxx"
#include "OpXferTargetHoldOk.hxx"
#include "OpXferComplete.hxx"
#include "OpXferEnded.hxx"
#include "OpXferDenied.hxx"

using namespace Vocal;

StateConsultXferring::StateConsultXferring()
{
    addEntryOperator( new OpXferHold );

    addOperator( new OpXferTargetHoldOk );
    addOperator( new OpXferComplete );
    addOperator( new OpXferEnded );
    addOperator( new OpXferDenied );
}


StateConsultXferring::~StateConsultXferring()
{
}


const char* const
StateConsultXferring::name() const
{
    return "StateConsultXferring";
}
