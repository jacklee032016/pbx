/*
$Id: StatePbxRinging.cxx,v 1.1.1.1 2006/11/29 09:50:50 lizhijie Exp $
 */

#include "global.h"
#include "StatePbxRinging.hxx"
#include "OpStartAudioDuplex.hxx"
#include "OpAck.hxx"
#include "OpTerminateCall.hxx"
#include "OpEndCall.hxx"
#include "OpReInvite.hxx"
#include "OpConfTargetOk.hxx"
#include "OpQueryPbxStatus.hxx"
#include "OpStartTimer.hxx"
#include "OpTimePbxEnd.hxx"
#include "OpPbxHookUp.hxx"
#include "OpBranchIdle.hxx"
#include "OpBranchBusy.hxx"
#include "OpCwPbxRing.hxx"
#include "OpStopPbxRinging.hxx"
#include "OpCancelTimer.hxx"
#include "UaConfiguration.hxx"

using namespace Vocal;

StatePbxRinging::StatePbxRinging()
{
#if 0 //modified by lijie
//	addOperator( new OpFwdDigit );

	addOperator( new OpFwdDigit );
	addOperator( new OpTerminateCall );
	addOperator( new OpEndCall );
//	addOperator( new OpReInvite );
//	addOperator( new OpStartCallWaiting );


//	addOperator( new OpRingCancel );
//	addOperator( new OpRingingInvite );

	addOperator( new OpPbxHookUp );
	addOperator( new OpPbxEnd );
	
//	addOperator( new OpAnswerCall );
//	addOperator( new OpCwRing );

//	addExitOperator( new OpStopRinging );
#else
    addEntryOperator( new OpStartTimer );
    addEntryOperator( new OpQueryPbxStatus );

    addOperator( new OpAck );
    addOperator( new OpConfTargetOk );
    addOperator( new OpBranchIdle );
    addOperator( new OpBranchBusy );
    addOperator( new OpReInvite );
    addOperator( new OpPbxHookUp );
    addOperator( new OpTerminateCall );
    addOperator( new OpCwPbxRing );
    addOperator( new OpTimePbxEnd );	
    addOperator( new OpEndCall );	
	
    addExitOperator( new OpCancelTimer );	
    addExitOperator( new OpStopPbxRinging );
#endif
}

StatePbxRinging::~StatePbxRinging()
{
}

const char* const  StatePbxRinging::name() const
{
	return "StatePbxRinging";
}
