/*
 * $Id: StateTrying.cxx,v 1.3 2007/06/19 18:10:44 lizhijie Exp $
 */

#include "global.h"
#include "StateTrying.hxx"

#include "OpOnHook.hxx"
#include "OpStartRingbackTone.hxx"
#include "OpReDirect.hxx"
#include "OpFarEndAnswered.hxx"
#include "OpFarEndBusy.hxx"
#include "OpFarEndError.hxx"
#include "OpStopRingbackTone.hxx"
#include "OpTerminateTrying.hxx"
#include "OpStartTimer.hxx"
#include "OpTimeout.hxx"
#include "OpCancelTimer.hxx"
#include "OpReInviteAuthenticated.hxx"

using namespace Vocal;

StateTrying::StateTrying()
{
	addEntryOperator( new OpStartTimer );

	addOperator( new OpOnHook );
	addOperator( new OpStartRingbackTone );
	addOperator( new OpReDirect );
	addOperator( new OpReInviteAuthenticated );
	addOperator( new OpFarEndAnswered );
	addOperator( new OpFarEndBusy );
	addOperator( new OpFarEndError );
	addOperator( new OpTimeout );

	addExitOperator( new OpStopRingbackTone );
	addExitOperator( new OpTerminateTrying );
	addExitOperator( new OpCancelTimer );

}

StateTrying::~StateTrying()
{}

const char* const StateTrying::name() const
{
	return "StateTrying";
}

