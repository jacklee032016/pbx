/*
 * $Id: EpStateOverLapOut.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */

#include "global.h"
#include "EpStateOverlapOut.hxx"

#include "OpStartAudioDuplex.hxx"
#include "OpHeldCallEndedIn2Calls.hxx"
#include "OpStartConsultXferring.hxx"
#include "OpSwapCalls.hxx"
#include "OpStartConferencing.hxx"
#include "OpStopAudio.hxx"
#include "OpSwapComplete.hxx"
#include "OpStartCallWaiting.hxx"

#include "PbxConfiguration.hxx"

using namespace Assist;

EpStateOverlapOut::EpStateOverlapOut()
{
#if 0
	addEntryOperator( new OpStartAudioDuplex );

	addOperator( new OpHeldCallEndedIn2Calls );
	addOperator( new OpSwapComplete );

	addExitOperator( new OpStopAudio );
#endif
}

EpStateOverlapOut::~EpStateOverlapOut()
{
}

const char* const EpStateOverlapOut::name() const
{
	return "EpStateOverlapOut";
}

