/*
 * $Id: StateIn2Calls.cxx,v 1.2 2007/03/02 04:19:29 lizhijie Exp $
 */

#include "global.h"
#include "StateIn2Calls.hxx"

#include "OpStartAudioDuplex.hxx"
#include "OpHeldCallEndedIn2Calls.hxx"
#include "OpStartConsultXferring.hxx"
#include "OpSwapCalls.hxx"
#include "OpStartConferencing.hxx"
#include "OpStopAudio.hxx"
#include "OpSwapComplete.hxx"
#include "OpStartCallWaiting.hxx"

#include "UaConfiguration.hxx"

using namespace Vocal;

StateIn2Calls::StateIn2Calls()
{
    addEntryOperator( new OpStartAudioDuplex );

    addOperator( new OpHeldCallEndedIn2Calls );

    UaXferMode xferMode = UaConfiguration::instance()->getXferMode();
    if ( xferMode == XferOn )
    {
        addOperator( new OpStartConsultXferring );
        addOperator( new OpSwapCalls );
    }
    else if ( xferMode == ConfOn )
    {
        addOperator( new OpStartConferencing );
    }

    addOperator( new OpSwapComplete );

    addExitOperator( new OpStopAudio );
}


StateIn2Calls::~StateIn2Calls()
{
}


const char* const
StateIn2Calls::name() const
{
    return "StateIn2Calls";
}
