/*
* $Id: ConsoleOpBuilder.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#ifndef	__CONSOLE_OP_BUILDER_HXX__
#define	__CONSOLE_OP_BUILDER_HXX__

#include "ConsoleOperator.hxx"
#include "EndPoint.hxx"
#include "PbxState.hxx"
#include "PbxTimerEvent.hxx"
#include "DigitEvent.hxx"
#include "CallEvent.hxx"
#include "CallMsgBuilder.hxx"

namespace Assist
{

#define CONSOLE_OP_DECLARE(type, name) \
class type : public ConsoleOperator\
{ \
public: \
	type() : ConsoleOperator(name) {} \
	~type(){} \
	virtual const Sptr <PbxState>  process(const Sptr <PbxEvent> event); \
}


CONSOLE_OP_DECLARE(ConsoleOpSetupInCall, 				"ConsoleOpSetupInCall");
CONSOLE_OP_DECLARE(ConsoleOpDisconnect, 				"ConsoleOpDisconnect");
CONSOLE_OP_DECLARE(ConsoleOpStopDialTone, 				"ConsoleOpStopDialTone");

/*IN operators */
CONSOLE_OP_DECLARE(ConsoleOpInCallAnswerTimer, 			"ConsoleOpInCallAnswerTimer");
CONSOLE_OP_DECLARE(ConsoleOpInCallReDial, 				"ConsoleOpInCallReDial");
CONSOLE_OP_DECLARE(ConsoleOpInCallSetup, 				"ConsoleOpInCallSetup");
CONSOLE_OP_DECLARE(ConsoleOpInCallTimeout, 				"ConsoleOpInCallTimeout");

/* OUT Operators */
CONSOLE_OP_DECLARE(ConsoleOpOutCallAlerting, 			"ConsoleOpOutCallAlerting");
CONSOLE_OP_DECLARE(ConsoleOpOutCallConnect, 			"ConsoleOpOutCallConnect");
CONSOLE_OP_DECLARE(ConsoleOpOutCallConnectAck, 			"ConsoleOpOutCallConnectAck");
CONSOLE_OP_DECLARE(ConsoleOpOutCallProceeding, 			"ConsoleOpOutCallProceeding");
CONSOLE_OP_DECLARE(ConsoleOpOutCallRelease, 			"ConsoleOpOutCallRelease");	
CONSOLE_OP_DECLARE(ConsoleOpOutCallTimeout, 			"ConsoleOpTerminateCall");

}

#endif

