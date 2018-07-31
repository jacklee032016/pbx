/*
* $Id: VoipStateBuilder.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "VoipStateBuilder.hxx"
#include "VoipOpBuilder.hxx"

#include "EpOpBuilder.hxx"

#include "PbxConfiguration.hxx"

using namespace Assist;

void VoipStateIdle::initState()
{
    //cpDebug(LOG_DEBUG, "getAuthentication = %s", PbxConfiguration::instance()->getAuthentication().c_str());
	if (PbxConfiguration::instance()->getAuthentication() )
	{
		addOperator( new VoipOpMsgInviteAuthenIdle );
		addOperator( new VoipOpMsgInviteAuthorIdle );
	}  
	
 	addOperator( new VoipOpMsgInviteIdle );
	addOperator(new VoipOpCallSetupIdle);
}

void VoipStateRinging::initState()
{
	addEntryOperator( new EpOpStartTimer(PbxConfiguration::instance()->getUaTimeout()*1000 ) );

	addOperator( new VoipOpCallReleaseRinging );
	addOperator( new VoipOpCallProceedingRinging );

	addOperator( new VoipOpMsgCancelByeRinging );
	addOperator( new VoipOpMsgReInviteRinging );
	addOperator( new VoipOpCallConnectRinging );	/* enter into IN_CALL */

	
#if WITH_CALLWAIT
	addOperator( new VoipOpCallCwRelease );
#endif
	addOperator( new VoipOpTimeoutRinging );
//	addOperator( new EpOpCallReleaseComplete(VOIP_EP_STATE_IDLE) );
	/*added by wuaiwu */
	//addOperator( new OpAck );
	
	addExitOperator( new EpOpCancelTimer );	
}

void VoipStateTrying::initState()
{
	addEntryOperator( new EpOpStartTimer(PbxConfiguration::instance()->getUaTimeout()*1000 ) );

	addOperator( new VoipOpMsgStatusInfoTrying );
	addOperator( new VoipOpMsgStatusRedirectTrying );
	addOperator( new VoipOpMsgStatusCerrAuthenTrying );
	addOperator( new VoipOpMsgStatusSuccessTrying );
	addOperator( new VoipOpMsgStatusCerrBusyTrying );
	addOperator( new VoipOpMsgStatusErrorTrying );		/* this must be last operator for ERROR process */
	addOperator( new VoipOpCallReleaseTrying );			/* both timeout and _CallRelease are process here */
//	addOperator( new EpOpCallReleaseComplete(VOIP_EP_STATE_IDLE) );

	addExitOperator( new VoipOpStopRingbackTrying );
	addExitOperator( new EpOpCancelTimer );	
}

void VoipStateCancelTrying::initState()
{
	addEntryOperator( new EpOpStartTimer(PbxConfiguration::instance()->getUaTimeout()*1000 ) );

	addOperator( new VoipOpMsgCancelOKTrying );

	/* need timeout operator */

	addExitOperator( new EpOpCancelTimer );	
}


void VoipStateInCall::initState()
{
	addOperator( new VoipOpMsgAckInCall );
	addOperator( new VoipOpMsgConfTargetOkInCall );
	addOperator( new VoipOpFwdDigitInCall );					/* transmit DTMF digit in RFC2833 */
	addOperator( new VoipOpCallDisconnectInCall );
	addOperator( new VoipOpMsgByeInCall );
	addOperator( new VoipOpMsgReInviteInCall );
//	addOperator( new VoipOpStartCallWaitingInCall );
	addOperator( new VoipOpRtpTimeoutInCall );
	addOperator( new EpOpCallReleaseComplete(VOIP_EP_STATE_IDLE) );

#if 0
	if ( PbxConfiguration::instance()->getXferMode() != XferOff )
	{
		addOperator( new OpSecondCall );
		addOperator( new OpRecvdXfer );
	}
#endif

}

#if WITH_CALLWAIT
void VoipStateCallEnded::initState()
{
	addOperator( new OpConvertCw );
}

void VoipStateOnhold::initState()
{
    addOperator( new OpRevert );
    addOperator( new OpHoldTargetOk );
    addOperator( new OpReDirect );
    addOperator( new OpEndCall );
    addOperator( new OpReRing );
    addOperator( new OpHoldTargetErr );
}
#endif

#if 0
void VoipStateBeingXferred::initState()
{
    addEntryOperator( new OpBeingXferred );
    addOperator( new OpBlindXferred );
    addOperator( new OpConsultXferred );
    addOperator( new OpXfereeComplete );
}


void VoipStateBlindXferring::initState()
{
    addEntryOperator( new OpXferHold );

    addOperator( new OpXferTargetInviteOk );
    addOperator( new OpBlindXferHoldOk );
    addOperator( new OpXferComplete );
    addOperator( new OpXferEnded );
    addOperator( new OpXferDenied );
}

void VoipStateConferencing::initState()
{
    addEntryOperator( new OpXferHold );

    addOperator( new OpConferencing );
    addOperator( new OpReDirect );
    addOperator( new OpConfTargetOk );
    addOperator( new OpConfComplete );
    addOperator( new OpConfError );
}

void VoipStateConsultXferring::initState()
{
    addEntryOperator( new OpXferHold );

    addOperator( new OpXferTargetHoldOk );
    addOperator( new OpXferComplete );
    addOperator( new OpXferEnded );
    addOperator( new OpXferDenied );
}

void VoipStateIn2Calls::initState()
{
    addEntryOperator( new OpStartAudioDuplex );

    addOperator( new OpHeldCallEndedIn2Calls );

    UaXferMode xferMode = PbxConfiguration::instance()->getXferMode();
    if ( xferMode == XferOn )
    {
        addOperator( new OpStartConsultXferring );
        addOperator( new OpSwapCalls );
    }
    else if ( xferMode == ConfOn )
    {
        addOperator( new OpStartConferencing );
    }

    addOperator( new OpSwapComplete );

    addExitOperator( new OpStopAudio );
}

void VoipStateInCallBusy::initState()
{
    addEntryOperator( new OpStartBusyTone );

    addOperator( new OpRevert );
    addOperator( new OpReRing );
    addOperator( new OpAckReInvite );

    addExitOperator( new OpStopBusyTone );
}

void VoipStateInCallDialing::initState()
{
    addEntryOperator( new OpStartDialTone );

    addOperator( new OpAddDigit );
    addOperator( new OpReRing );
    addOperator( new OpRevert );
    addOperator( new OpInCallInvite );
    addOperator( new OpHeldCallEndedDialing );
    addOperator( new OpAckReInvite );
}

void VoipStateInCallFastBusy::initState()
{
    addEntryOperator( new OpStartFastBusyTone );

    addOperator( new OpRevert );
    addOperator( new OpReRing );
    addOperator( new OpAckReInvite );

    addExitOperator( new OpStopFastBusyTone );
}

void VoipStateInCallTrying::initState()
{
    addOperator( new OpHeldCallEndedTrying );
    addOperator( new OpInCallStopTrying );
    addOperator( new OpRevert );
    addOperator( new OpStartRingbackTone );
    addOperator( new OpReDirect );
    addOperator( new OpStartBlindXferring );
    addOperator( new OpInCallFarEndAnswered );
    addOperator( new OpInCallBusy );
    addOperator( new OpInCallFastBusy );

    addExitOperator( new OpStopRingbackTone );
}

void VoipStateReRinging::initState()
{
    //    addOperator( new OpInCallDialing );
    addEntryOperator( new OpStartRinging );

    addOperator( new OpRevert );
    addOperator( new OpActiveCallEnded );
    addOperator( new OpAckReInvite );

    addExitOperator( new OpStopRinging );
}
#endif

