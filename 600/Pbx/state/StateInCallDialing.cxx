/*
 * $Id: StateInCallDialing.cxx,v 1.2 2007/03/02 04:19:29 lizhijie Exp $
 */

#include "global.h"
#include "StateInCallDialing.hxx"

#include "OpSuspendAudio.hxx"
#include "OpStartDialTone.hxx"
#include "OpStopDialTone.hxx"
#include "OpReRing.hxx"
#include "OpRevert.hxx"
#include "OpInCallInvite.hxx"
#include "OpHeldCallEndedDialing.hxx"
#include "OpAckReInvite.hxx"
#include "OpAddDigit.hxx"

using namespace Vocal;

StateInCallDialing::StateInCallDialing()
{
    addEntryOperator( new OpStartDialTone );

    addOperator( new OpAddDigit );
    addOperator( new OpReRing );
    addOperator( new OpRevert );
    addOperator( new OpInCallInvite );
    addOperator( new OpHeldCallEndedDialing );
    addOperator( new OpAckReInvite );
}


StateInCallDialing::~StateInCallDialing()
{
}


const char* const
StateInCallDialing::name() const
{
    return "StateInCallDialing";
}
