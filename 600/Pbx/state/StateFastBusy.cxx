/*
 * $Id: StateFastBusy.cxx,v 1.3 2007/03/30 09:13:18 lizhijie Exp $
 */

#include "global.h"
#include "StateFastBusy.hxx"

#include "OpStartFastBusyTone.hxx"
#include "OpOnHook.hxx"
#include "OpHangupError.hxx"
#include "OpStopFastBusyTone.hxx"

using namespace Vocal;

/* Congestion Tone when error except BUSY(486) */
StateFastBusy::StateFastBusy()
{
	addEntryOperator( new OpStartFastBusyTone );

	addOperator( new OpOnHook );
	addOperator( new OpHangupError );

	addExitOperator( new OpStopFastBusyTone );
}

StateFastBusy::~StateFastBusy()
{
}

const char* const StateFastBusy::name() const
{
	return "StateFastBusy";
}

