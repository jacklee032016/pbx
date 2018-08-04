/*
* $Id: CallOpBuilder.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#ifndef	__CALL_OP_BUILDER_HXX__
#define	__CALL_OP_BUILDER_HXX__

#include "CallOperator.hxx"

#include "EndPoint.hxx"
#include "PbxState.hxx"
#include "CallEvent.hxx"
#include "CallMsgBuilder.hxx"


namespace Assist
{

#define CALL_OP_DECLARE(type, name) \
class type : public CallOperator\
{ \
public: \
	type() : CallOperator(name) {} \
	~type(){} \
	virtual const Sptr <PbxState>  process(const Sptr <PbxEvent> event); \
}


	CALL_OP_DECLARE(CallOpAlerting, 					"CallOpAlerting");
	CALL_OP_DECLARE(CallOpClean, 						"CallOpClean");
	CALL_OP_DECLARE(CallOpConnect, 					"CallOpConnect");
	CALL_OP_DECLARE(CallOpConnectAckBeforeConnect, 	"CallOpConnectAckBeforeConnect");
	CALL_OP_DECLARE(CallOpConnectAckInConnect, 		"CallOpConnectAckInConnect");
	CALL_OP_DECLARE(CallOpDisconnect, 					"CallOpDisconnect");
	CALL_OP_DECLARE(CallOpProceeding, 					"CallOpProceeding");
	CALL_OP_DECLARE(CallOpRelease, 					"CallOpRelease");
	CALL_OP_DECLARE(CallOpForwardSetupInCall, 			"CallOpForwardSetupInCall");


	CALL_OP_DECLARE(CallOpRouting, 					"CallOpRouting");
}

#endif

