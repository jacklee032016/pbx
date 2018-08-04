/*
* $Id: ConsoleStateBuilder.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#ifndef	__CONSOLE_STATE_BUILDER_HXX__
#define	__CONSOLE_STATE_BUILDER_HXX__

#include "EpState.hxx"
#include "ConsoleEndPoint.hxx"

namespace Assist
{

ENDPOINT_STATE_DECLARE(ConsoleStateIdle, "ConsoleStateIdle", CONSOLE_EP_STATE_IDLE);
ENDPOINT_STATE_DECLARE(ConsoleStateConnect, "ConsoleStateConnect", CONSOLE_EP_STATE_CONNECT);
ENDPOINT_STATE_DECLARE(ConsoleStateInAlerting, "ConsoleStateInAlerting", CONSOLE_EP_STATE_IN_ALERTING);
ENDPOINT_STATE_DECLARE(ConsoleStateInGreeting, "ConsoleStateInGreeting", CONSOLE_EP_STATE_IN_GREETING);
ENDPOINT_STATE_DECLARE(ConsoleStateOutAlerting, "ConsoleStateOutAlerting", CONSOLE_EP_STATE_OUT_ALERTING);
ENDPOINT_STATE_DECLARE(ConsoleStateOutCalling, "ConsoleStateOutCalling", CONSOLE_EP_STATE_OUT_CALLING);

}

#endif

