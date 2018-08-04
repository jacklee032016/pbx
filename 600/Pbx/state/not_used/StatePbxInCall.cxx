/*
$Id: StatePbxInCall.cxx,v 1.1.1.1 2006/11/29 09:50:50 lizhijie Exp $
 */

#include "global.h"
#include "StatePbxInCall.hxx"
#include "OpPbxEnd.hxx"

#include "OpTerminateCall.hxx"
#include "OpEndCall.hxx"
#include "OpStopAudio.hxx"
#include "OpSecondCall.hxx"
#include "OpReInvite.hxx"
#include "OpRecvdXfer.hxx"
#include "OpStartCallWaiting.hxx"
#include "OpConfTargetOk.hxx"
#include "OpFwdDigit.hxx"

#include "UaConfiguration.hxx"

using namespace Vocal;

/* now it is not used, lizhijie, 2005.02.22 */
StatePbxInCall::StatePbxInCall()
{
	addEntryOperator( new OpPbxEnd );

//	addOperator( new OpAck );
	addOperator( new OpConfTargetOk );
	addOperator( new OpFwdDigit );
	addOperator( new OpTerminateCall );
	addOperator( new OpEndCall );
	addOperator( new OpReInvite );
	addOperator( new OpStartCallWaiting );

	addExitOperator( new OpStopAudio );
}

StatePbxInCall::~StatePbxInCall()
{
}

const char* const StatePbxInCall::name() const
{
	return "StatePbxInCall";
}
