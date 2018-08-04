/*
* $Id: PstnStateBuilder.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "PstnStateBuilder.hxx"
#include "PstnOpBuilder.hxx"

#include "EpOpBuilder.hxx"

#include "PbxConfiguration.hxx"

using namespace Assist;

void PstnStateCallId::initState()
{
	addEntryOperator(new EpOpStartTimer(PbxConfiguration::instance()->getCarTimes()*1000) );
	addEntryOperator( new PstnOpQueryStatus );

	addOperator(new PstnOpDeviceStatus);
	/* Timeout event is process in OpPrimaryAnswer */
	addOperator(new PstnOpPrimAnswer);
	
	addExitOperator( new EpOpCancelTimer );	
}

/*
* no timeout event is need for in this state, because when FXO thread demoduating, it will timeout if no FSK signal is detected 
* Timeout must beed added if no CAR or no RING
*/
void PstnStateCar::initState()
{
//	addEntryOperator (new EpOpStartTimer(PbxConfiguration::instance()->getCarCount() ) );
	addOperator( new PstnOpEndCar ); 
	addOperator( new PstnOpStartCall ); 
	addOperator (new PstnOpTimeout);
}

void PstnStateDialing::initState()
{
	addEntryOperator (new EpOpStartTimer(PbxConfiguration::instance()->getInitialDigitTimeout() ) );
	addEntryOperator( new PstnOpStartDialTone );
	
	addOperator( new PstnOpStopDialTone );
	/* this operation must be added before PstnOpSetup and after OpStopDialTone */
	addOperator( new EpOpAddDigit );
	addOperator( new PstnOpSetup);
//	addOperator( new PstnOpDialError );
	addOperator (new PstnOpTimeout);
	addOperator( new PstnOpOnHook ); 
//	addOperator( new EpOpCallReleaseComplete(PSTN_EP_STATE_IDLE) );
	
	addExitOperator( new EpOpCancelTimer );	
}

/* when rx DisConnect signal from far-end, play BUSY TONE and wait on-hook */
void PstnStateError::initState()
{
	addEntryOperator( new PstnOpStartErrorTone );
	addOperator( new PstnOpOnHook );
 }

void PstnStateIdle::initState()
{
	addOperator( new PstnOpStartCall );
	addOperator( new PstnOpRing );
}

void PstnStateInCall::initState()
{
	addOperator( new PstnOpFwdDigitInCall );		/* forward DTMF digit rx from driver to peer */
	addOperator( new PstnOpSetupInCall );		/* rx DTMF digit forward by peer and play it , only on FXO device */
	addOperator( new PstnOpTerminateCall );		/* get Hook-down event from device */
	addOperator( new EpOpCallReleaseComplete(PSTN_EP_STATE_IDLE) );
	addOperator( new PstnOpEndCall );			/* rx CALL_DISCONNECT from peer */
}

void PstnStateIncoming::initState()
{
	addEntryOperator(new EpOpStartTimer(PbxConfiguration::instance()->getCarTimes()*1000) );
	addOperator(new PstnOpPrimAnswer);
	addOperator(new PstnOpTimeout);
	
	addExitOperator( new EpOpCancelTimer );	
}

void PstnStateRinging::initState()
{
	addEntryOperator( new EpOpStartTimer(PbxConfiguration::instance()->getConsoleAlertingTimeout() ) );
	addEntryOperator( new PstnOpQueryStatus );

	addOperator( new PstnOpDeviceStatus );

	addOperator( new PstnOpRingCancel );
	addOperator( new PstnOpAnswerCall );
	addOperator( new PstnOpTimeout);	

	addExitOperator( new EpOpCancelTimer );	
	addExitOperator( new PstnOpStopRinging );	/*it should be implemented as enter operation */
}

void PstnStateTrying::initState()
{
	addEntryOperator( new EpOpStartTimer(PbxConfiguration::instance()->getConsoleCallingTimeout() ) );

	addOperator( new PstnOpOnHook );
//	addOperator( new EpOpCallReleaseComplete(PSTN_EP_STATE_IDLE) );
	addOperator( new PstnOpStartRingbackTone );
	addOperator( new PstnOpFarEndAnswered );
	addOperator( new PstnOpFarEndError );
	addOperator( new PstnOpTimeout );

	addExitOperator( new PstnOpTerminateTrying );
	addExitOperator( new EpOpCancelTimer );
}

