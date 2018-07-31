/*
* $Id: CallStateBuilder.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#ifndef	__CALL_STATE_BUILDER_HXX__
#define	__CALL_STATE_BUILDER_HXX__

#include "CallState.hxx"

namespace Assist
{

CALL_STATE_DECLARE(CallStateSetup, 		"CallStateSetup", 			CALL_STATE_SETUP);
CALL_STATE_DECLARE(CallStateProceeding, 	"CallStateProceeding", 		CALL_STATE_PROCEEDING);
CALL_STATE_DECLARE(CallStateAlerting, 		"CallStateAlerting", 		CALL_STATE_ALERTING);
CALL_STATE_DECLARE(CallStateConnect, 		"CallStateConnect", 		CALL_STATE_CONNECT);
CALL_STATE_DECLARE(CallStateDisconnect, 	"CallStateDisconnect", 		CALL_STATE_DISCONNECT);
CALL_STATE_DECLARE(CallStateIdle, 		"CallStateIdle", 			CALL_STATE_IDLE);
//CALL_STATE_DECLARE(CallStateError, 		"CallStateError", 			CALL_STATE_ERROR);

}

#endif

