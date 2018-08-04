/*
 * $Id: StateCallEnded.cxx,v 1.2 2007/03/02 04:19:28 lizhijie Exp $
 */

#include "global.h"
#include "StateCallEnded.hxx"

#include "OpOnHook.hxx"
#include "OpConvertCw.hxx"
#include "OpHangupError.hxx"

using namespace Vocal;

StateCallEnded::StateCallEnded()
{
    addOperator( new OpOnHook );
    addOperator( new OpConvertCw );
    addOperator( new OpHangupError );
}


StateCallEnded::~StateCallEnded()
{
}


const char* const
StateCallEnded::name() const
{
    return "StateCallEnded";
}
