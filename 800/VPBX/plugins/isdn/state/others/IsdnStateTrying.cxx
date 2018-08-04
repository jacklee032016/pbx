/*
 * $Id: IsdnStateTrying.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */

#include "global.h"
#include "IsdnStateTrying.hxx"

#include "IsdnOpOnHook.hxx"
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

using namespace Assist;

IsdnStateTrying::IsdnStateTrying()
{
	addEntryOperator( new OpStartTimer );

	addOperator( new IsdnOpOnHook );
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

IsdnStateTrying::~IsdnStateTrying()
{}

const char* const IsdnStateTrying::name() const
{
	return "IsdnStateTrying";
}

