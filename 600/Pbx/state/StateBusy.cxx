/*
 * $Id: StateBusy.cxx,v 1.3 2007/03/30 09:13:18 lizhijie Exp $
 */

#include "global.h"
#include "StateBusy.hxx"

#include "OpStartBusyTone.hxx"
#include "OpOnHook.hxx"
#include "OpHangupError.hxx"
#include "OpStopBusyTone.hxx"

using namespace Vocal;

/* only BUSY(486) */
StateBusy::StateBusy()
{
	addEntryOperator( new OpStartBusyTone );

	addOperator( new OpOnHook );
	addOperator( new OpHangupError );

	addExitOperator( new OpStopBusyTone );
}


StateBusy::~StateBusy()
{
}

const char* const StateBusy::name() const
{
	return "StateBusy";
}

