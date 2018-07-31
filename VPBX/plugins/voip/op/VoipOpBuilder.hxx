#ifndef	__VOIP_OP_BUILDER_HXX__
#define	__VOIP_OP_BUILDER_HXX__
/*
* $Id: VoipOpBuilder.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "SipContext.hxx"
#ifdef HAS_RSVPLIB
	#include "SuaRsvp.hxx"
#endif
#include "SdpHandler.hxx"
#include "RsvpHandler.hxx"
#include "Md5.hxx"
#include "digest.hxx"
#include "SipDigest.hxx"
#include "SipCallId.hxx"
#include "SipSdp.hxx"
#include "SipVia.hxx"
#include "Sdp2Media.hxx"
#include "StatusMsg.hxx"
#include "InviteMsg.hxx"

#include <sys/time.h>
#include <string.h>
#include "SystemInfo.hxx"

#include "VoipOperator.hxx"

#include "EndPoint.hxx"
#include "PbxState.hxx"
#include "PbxTimerEvent.hxx"
#include "DigitEvent.hxx"
#include "SipEvent.hxx"
#include "CallEvent.hxx"
#include "CallMsgBuilder.hxx"

#ifndef NO_POLICY
	#include "PepAgent.hxx"
#endif
#if __CDR_OPEN__
	#include "SimulateDB.hxx"
	using namespace SDB;
#endif

#include "PbxConfiguration.hxx"

namespace Assist
{
/* IDLE's operators */
	VOIP_OP_DECLARE(VoipOpMsgInviteAuthenIdle, 	"VoipOpMsgInviteAuthenIdle");
	VOIP_OP_DECLARE(VoipOpMsgInviteAuthorIdle, 	"VoipOpMsgInviteAuthorIdle");
	VOIP_OP_DECLARE(VoipOpMsgInviteIdle, 		"VoipOpMsgInviteIdle");
	VOIP_OP_DECLARE(VoipOpCallSetupIdle, 		"VoipOpCallSetupIdle");

/* RINGING's operators */
	VOIP_OP_DECLARE(VoipOpCallReleaseRinging,	"VoipOpCallReleaseRinging");
	VOIP_OP_DECLARE(VoipOpCallProceedingRinging,"VoipOpCallProceedingRinging");
	VOIP_OP_DECLARE(VoipOpCallConnectRinging, 	"VoipOpCallConnectRinging");
	VOIP_OP_DECLARE(VoipOpMsgCancelByeRinging,	"VoipOpMsgCancelByeRinging");
	VOIP_OP_DECLARE(VoipOpMsgReInviteRinging, 	"VoipOpMsgReInviteRinging");
	VOIP_OP_DECLARE(VoipOpTimeoutRinging, 		"VoipOpTimeoutRinging");

/* TRYING's states  */	
	VOIP_OP_DECLARE(VoipOpCallReleaseTrying, 			"VoipOpCallReleaseTrying");
	VOIP_OP_DECLARE(VoipOpMsgStatusCerrAuthenTrying, 	"VoipOpMsgStatusCerrAuthenTrying");
	VOIP_OP_DECLARE(VoipOpMsgStatusCerrBusyTrying, 		"VoipOpMsgStatusCerrBusyTrying");
	VOIP_OP_DECLARE(VoipOpMsgStatusErrorTrying, 		"VoipOpMsgStatusErrorTrying");
	VOIP_OP_DECLARE(VoipOpMsgStatusInfoTrying, 			"VoipOpMsgStatusInfoTrying");
	VOIP_OP_DECLARE(VoipOpMsgStatusRedirectTrying, 		"VoipOpMsgStatusRedirectTrying");
	VOIP_OP_DECLARE(VoipOpMsgStatusSuccessTrying, 		"VoipOpMsgStatusSuccessTrying");
	VOIP_OP_DECLARE(VoipOpStopRingbackTrying, 			"VoipOpStopRingbackTrying");
	
/* INCALL's states  */	
	VOIP_OP_DECLARE(VoipOpMsgAckInCall, 				"VoipOpMsgAckInCall");
	VOIP_OP_DECLARE(VoipOpMsgConfTargetOkInCall, 		"VoipOpMsgConfTargetOkInCall");
	VOIP_OP_DECLARE(VoipOpCallDisconnectInCall, 			"VoipOpCallDisconnectInCall");
	VOIP_OP_DECLARE(VoipOpMsgByeInCall, 				"VoipOpMsgByeInCall");
	VOIP_OP_DECLARE(VoipOpRtpTimeoutInCall, 			"VoipOpRtpTimeoutInCall");
	VOIP_OP_DECLARE(VoipOpMsgReInviteInCall, 			"VoipOpMsgReInviteInCall");
	VOIP_OP_DECLARE(VoipOpFwdDigitInCall, 				"VoipOpFwdDigitInCall");
//	VOIP_OP_DECLARE(VoipOpStartCallWaitingInCall, 		"VoipOpStartCallWaitingInCall");
	
/* CANCEL TRYING's ops */
	VOIP_OP_DECLARE(VoipOpMsgCancelOKTrying, 			"VoipOpMsgCancelOKTrying");

#if 0
	VOIP_OP_DECLARE(VoipOpActiveCallEnded, 	"VoipOpActiveCallEnded");
	VOIP_OP_DECLARE(VoipOpBlindXferring, 	"VoipOpBlindXferring");
	VOIP_OP_DECLARE(VoipOpBlindXferred, 	"VoipOpBlindXferred");

/* TRYING's states  */	
	VOIP_OP_DECLARE(VoipOpBlindXferHoldOk, "VoipOpBlindXferHoldOk");
	VOIP_OP_DECLARE(VoipOpBeingXferred, 	"VoipOpBeingXferred");
	VOIP_OP_DECLARE(VoipOpAnswerCall, 		"VoipOpAnswerCall");
	
/* INCALL's states  */	
	VOIP_OP_DECLARE(PstnOpTerminateTrying, "PstnOpTerminateTrying");

	VOIP_OP_DECLARE(PstnOpFarEndAnswered, "PstnOpFarEndAnswered");
	VOIP_OP_DECLARE(PstnOpFarEndError, "PstnOpFarEndError");
	VOIP_OP_DECLARE(PstnOpTimeout, "PstnOpTimeout");
	VOIP_OP_DECLARE(PstnOpTerminateTrying, "PstnOpTerminateTrying");
/* misc */
	VOIP_OP_DECLARE(VoipOpConfError, 		"VoipOpConfError");
	VOIP_OP_DECLARE(VoipOpConfTargetOk, 	"VoipOpConfTargetOk");
	VOIP_OP_DECLARE(VoipOpConvertCw, 		"VoipOpConvertCw");
	VOIP_OP_DECLARE(VoipOpConsultXferring, 	"PstnOpTerminateTrying");
	VOIP_OP_DECLARE(VoipOpConsultXferred, 	"VoipOpConsultXferred");

	VOIP_OP_DECLARE(VoipOpConfComplete, 	"VoipOpConfComplete");
	VOIP_OP_DECLARE(VoipOpConferencing, 	"PstnOpFarEndError");
	VOIP_OP_DECLARE(VoipOpCwRing, 		"VoipOpCwRing");
	VOIP_OP_DECLARE(PstnOpTerminateTrying, "PstnOpTerminateTrying");
	VOIP_OP_DECLARE(PstnOpQueryStatus, "PstnOpQueryStatus");

	VOIP_OP_DECLARE(PstnOpFarEndAnswered, "PstnOpFarEndAnswered");
	VOIP_OP_DECLARE(PstnOpFarEndError, "PstnOpFarEndError");
	VOIP_OP_DECLARE(PstnOpTimeout, "PstnOpTimeout");
	VOIP_OP_DECLARE(PstnOpTerminateTrying, "PstnOpTerminateTrying");
	
/* VOIP Device Operator */	
	VOIP_OP_DECLARE(PstnOpRingCancel, "PstnOpRingCancel");
	VOIP_OP_DECLARE(PstnOpAnswerCall, "PstnOpAnswerCall");
	VOIP_OP_DECLARE(PstnOpStopRinging, "PstnOpStopRinging");	
	VOIP_OP_DECLARE(PstnOpTerminateCall, "PstnOpTerminateCall");
	
	VOIP_OP_DECLARE(PstnOpFarEndAnswered, "PstnOpFarEndAnswered");
	VOIP_OP_DECLARE(PstnOpFarEndError, "PstnOpFarEndError");
	VOIP_OP_DECLARE(PstnOpTimeout, "PstnOpTimeout");
	VOIP_OP_DECLARE(PstnOpTerminateTrying, "PstnOpTerminateTrying");

#endif

}

#endif

