/*
* $Id: VoipStateBuilder.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#ifndef	__VOIP_STATE_BUILDER_HXX__
#define	__VOIP_STATE_BUILDER_HXX__

#include "EpState.hxx"
#include "VoipEndPoint.hxx"

namespace Assist
{

ENDPOINT_STATE_DECLARE(VoipStateIdle, 			"VoipStateIdle", 				VOIP_EP_STATE_IDLE );
ENDPOINT_STATE_DECLARE(VoipStateRinging, 		"VoipStateRinging", 			VOIP_EP_STATE_RINGING);
ENDPOINT_STATE_DECLARE(VoipStateTrying, 		"VoipStateTrying", 				VOIP_EP_STATE_TRYING);
ENDPOINT_STATE_DECLARE(VoipStateCancelTrying, 	"VoipStateCancelTrying", 		VOIP_EP_STATE_CANCEL_TRYING);
ENDPOINT_STATE_DECLARE(VoipStateInCall, 		"VoipStateInCall", 				VOIP_EP_STATE_INCALL);

#if WITH_CALLWAIT
ENDPOINT_STATE_DECLARE(VoipStateCallEnded, 	"VoipStateCallEnded", 	VOIP_EP_STATE_CALL_ENDED);
ENDPOINT_STATE_DECLARE(VoipStateOnhold, "VoipStateOnhold", PSTN_EP_STATE_DIALING);

#endif


#if 0
ENDPOINT_STATE_DECLARE(VoipStateBeingXferred, "VoipStateBeingXferred", VOIP_EP_STATE_CALLID);
ENDPOINT_STATE_DECLARE(VoipStateBlindXferring, "VoipStateBlindXferring", VOIP_EP_STATE_CAR);

ENDPOINT_STATE_DECLARE(VoipStateConferencing, "VoipStateConferencing", VOIP_EP_STATE_INCALL);
ENDPOINT_STATE_DECLARE(VoipStateConsultXferring, "VoipStateConsultXferring", VOIP_EP_STATE_CAR);


ENDPOINT_STATE_DECLARE(VoipStateIn2Calls, "VoipStateIn2Calls", PSTN_EP_STATE_CAR);
ENDPOINT_STATE_DECLARE(VoipStateInCallBusy, "VoipStateInCallBusy", PSTN_EP_STATE_IDLE);
ENDPOINT_STATE_DECLARE(VoipStateInCallDialing, "VoipStateInCallDialing", PSTN_EP_STATE_ERROR);

ENDPOINT_STATE_DECLARE(VoipStateInCallFastBusy, "VoipStateInCallFastBusy", PSTN_EP_STATE_INCALL);
ENDPOINT_STATE_DECLARE(VoipStateInCallTrying, "VoipStateInCallTrying", PSTN_EP_STATE_CAR);
ENDPOINT_STATE_DECLARE(VoipStateReRinging, "VoipStateReRinging", PSTN_EP_STATE_IDLE);

ENDPOINT_STATE_DECLARE(VoipStatePbxDialing, "VoipStatePbxDialing", PSTN_EP_STATE_INCALL);
ENDPOINT_STATE_DECLARE(VoipStateInCallTrying, "VoipStateInCallTrying", PSTN_EP_STATE_CAR);

#endif


}

#endif

