/*
 * $Id: StateError.cxx,v 1.2 2007/03/02 04:19:28 lizhijie Exp $
 */

#include "global.h"
#include "StateError.hxx"

#include "OpStartErrorTone.hxx"
#include "OpOnHook.hxx"
#include "OpStopErrorTone.hxx"

using namespace Vocal;

StateError::StateError()
{
    addEntryOperator( new OpStartErrorTone );

    addOperator( new OpOnHook );

    addExitOperator( new OpStopErrorTone );
}


StateError::~StateError()
{
}


const char* const
StateError::name() const
{
    return "StateError";
}
