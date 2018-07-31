/*
* $Id: IsdnStateRinging.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "global.h"
#include "IsdnStateRinging.hxx"

#include "OpStartRinging.hxx"
#include "OpRingCancel.hxx"
#include "OpRingingInvite.hxx"
#include "OpAnswerCall.hxx"
#include "OpStopRinging.hxx"
#include "OpCwRing.hxx"
#include "OpQueryStatus.hxx"
#include "OpDeviceIdle.hxx"
#include "OpDeviceBusy.hxx"
#include "OpStartTimer.hxx"
#include "OpTimeOver.hxx"
#include "OpCancelTimer.hxx"

using namespace Assist;

IsdnStateRinging::IsdnStateRinging()
{
	addEntryOperator( new OpStartTimer );
	addEntryOperator( new OpQueryStatus );

	addOperator( new OpDeviceIdle );
	addOperator( new OpDeviceBusy );

	addOperator( new OpRingCancel );
	addOperator( new OpRingingInvite );
	addOperator( new OpAnswerCall );
	addOperator( new OpCwRing );
	addOperator( new OpTimeOver );

	addExitOperator( new OpCancelTimer );
	addExitOperator( new OpStopRinging );
}

IsdnStateRinging::~IsdnStateRinging()
{
}

const char* const IsdnStateRinging::name() const
{
	return "IsdnStateRinging";
}

