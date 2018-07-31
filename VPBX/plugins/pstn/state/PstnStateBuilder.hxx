/*
* $Id: PstnStateBuilder.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#ifndef	__PSTN_STATE_BUILDER_HXX__
#define	__PSTN_STATE_BUILDER_HXX__

#include "EpState.hxx"
#include "PstnEndPoint.hxx"

namespace Assist
{

ENDPOINT_STATE_DECLARE(PstnStateCallId, "PstnStateCallId", PSTN_EP_STATE_CALLID);
ENDPOINT_STATE_DECLARE(PstnStateCar, "PstnStateCar", PSTN_EP_STATE_CAR);
ENDPOINT_STATE_DECLARE(PstnStateDialing, "PstnStateDialing", PSTN_EP_STATE_DIALING);
ENDPOINT_STATE_DECLARE(PstnStateError, "PstnStateError", PSTN_EP_STATE_ERROR);
ENDPOINT_STATE_DECLARE(PstnStateIdle, "PstnStateIdle", PSTN_EP_STATE_IDLE);
ENDPOINT_STATE_DECLARE(PstnStateInCall, "PstnStateInCall", PSTN_EP_STATE_INCALL);

ENDPOINT_STATE_DECLARE(PstnStateIncoming, "PstnStateIncoming", PSTN_EP_STATE_INCOMING);
ENDPOINT_STATE_DECLARE(PstnStateRinging, "PstnStateRinging", PSTN_EP_STATE_RINGING);
ENDPOINT_STATE_DECLARE(PstnStateTrying, "PstnStateTrying", PSTN_EP_STATE_TRYING);

}

#endif

