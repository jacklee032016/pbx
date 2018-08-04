/*
 * $Id: StateInCall.cxx,v 1.3 2007/03/16 19:33:56 lizhijie Exp $
 */

#include "global.h"
#include "StateInCall.hxx"

#include "OpStartAudioDuplex.hxx"
#include "OpAck.hxx"
#include "OpTerminateCall.hxx"
#include "OpEndCall.hxx"
#include "OpStopAudio.hxx"
#include "OpSecondCall.hxx"
#include "OpReInvite.hxx"
#include "OpRecvdXfer.hxx"
#include "OpStartCallWaiting.hxx"
#include "OpConfTargetOk.hxx"
#include "OpFwdDigit.hxx"
#include "OpDropTalk.hxx"

#include "UaConfiguration.hxx"

using namespace Vocal;

StateInCall::StateInCall()
{
	addEntryOperator( new OpStartAudioDuplex );

	addOperator( new OpAck );
	addOperator( new OpConfTargetOk );
	addOperator( new OpFwdDigit );
	addOperator( new OpTerminateCall );
	addOperator( new OpEndCall );
	addOperator( new OpReInvite );
	addOperator( new OpStartCallWaiting );
	addOperator( new OpDropTalk );

	if ( UaConfiguration::instance()->getXferMode() != XferOff )
	{
		addOperator( new OpSecondCall );
		addOperator( new OpRecvdXfer );
	}

	addExitOperator( new OpStopAudio );
}


StateInCall::~StateInCall()
{
}


const char* const
StateInCall::name() const
{
    return "StateInCall";
}
