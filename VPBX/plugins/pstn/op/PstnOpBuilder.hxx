/*
* $Id: PstnOpBuilder.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#ifndef		__PSTN_OP_BUILDER_HXX__
#define		__PSTN_OP_BUILDER_HXX__

#include "PstnOperator.hxx"

#include "EndPoint.hxx"
#include "PbxState.hxx"
#include "PbxTimerEvent.hxx"
#include "DigitEvent.hxx"
#include "CallEvent.hxx"
#include "CallMsgBuilder.hxx"

namespace Assist
{

	PSTN_OP_DECLARE(PstnOpDialError, 				"PstnOpDialError");
	PSTN_OP_DECLARE(PstnOpOnHook, 				"PstnOpOnHook");
	PSTN_OP_DECLARE(PstnOpRing, 					"PstnOpRing");
	PSTN_OP_DECLARE(PstnOpSetup, 					"PstnOpSetup");
	PSTN_OP_DECLARE(PstnOpStartCall, 				"PstnOpStartCall");
	PSTN_OP_DECLARE(PstnOpStartDialTone, 			"PstnOpStartDialTone");
	PSTN_OP_DECLARE(PstnOpStartErrorTone, 			"PstnOpStartErrorTone");
	PSTN_OP_DECLARE(PstnOpStopDialTone, 			"PstnOpStopDialTone");


	PSTN_OP_DECLARE(PstnOpStartRingbackTone, 		"PstnOpStartRingbackTone");
	PSTN_OP_DECLARE(PstnOpFarEndAnswered, 		"PstnOpFarEndAnswered");
	PSTN_OP_DECLARE(PstnOpFarEndError, 			"PstnOpFarEndError");					/* include ahndler of PstnOpFarEndBusy also*/
	PSTN_OP_DECLARE(PstnOpTimeout, 				"PstnOpTimeout");
	PSTN_OP_DECLARE(PstnOpTerminateTrying, 		"PstnOpTerminateTrying");			/* include handlers for PstnOpStopRingbackTone also */

	PSTN_OP_DECLARE(PstnOpQueryStatus, 			"PstnOpQueryStatus");
	PSTN_OP_DECLARE(PstnOpDeviceStatus, 			"PstnOpDeviceStatus");				/* include handlers for both of OpDeviceIdle and OpDeviceBusy */
	PSTN_OP_DECLARE(PstnOpRingCancel, 			"PstnOpRingCancel");
	PSTN_OP_DECLARE(PstnOpAnswerCall, 			"PstnOpAnswerCall");
	PSTN_OP_DECLARE(PstnOpStopRinging, 			"PstnOpStopRinging");	

	PSTN_OP_DECLARE(PstnOpTerminateCall, 			"PstnOpTerminateCall");
	PSTN_OP_DECLARE(PstnOpEndCall, 				"PstnOpEndCall");

	PSTN_OP_DECLARE(PstnOpPrimAnswer, 			"PstnOpPrimAnswer");					/* include handlers for both PrimaryAnswer and ImcomingSucess */

	PSTN_OP_DECLARE(PstnOpEndCar, 				"PstnOpEndCar");

	PSTN_OP_DECLARE(PstnOpFwdDigitInCall, 			"PstnOpFwdDigitInCall");
	PSTN_OP_DECLARE(PstnOpSetupInCall, 			"PstnOpSetupInCall");
	
}

#endif

