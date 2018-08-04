/*
 * $Id: StateAutoTrying.cxx,v 1.2 2007/03/07 18:24:09 lizhijie Exp $
 */

#include "global.h"
#include "StateAutoTrying.hxx"

#include "OpAutoFarEndAnswered.hxx"
#include "OpAutoTryingTO.hxx"

using namespace Vocal;

StateAutoTrying::StateAutoTrying()
{
    addOperator( new OpAutoFarEndAnswered );
    addOperator( new OpAutoTryingTO );
}


StateAutoTrying::~StateAutoTrying()
{
}

const char* const StateAutoTrying::name() const
{
    return "StateAutoTrying";
}

