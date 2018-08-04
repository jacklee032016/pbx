/*
* $Id: ConsoleStateBuilder.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "ConsoleStateBuilder.hxx"
#include "ConsoleOpBuilder.hxx"

#include "EpOpBuilder.hxx"

#include "PbxConfiguration.hxx"

using namespace Assist;

void ConsoleStateConnect::initState()
{
	addOperator( new ConsoleOpOutCallConnectAck );
	addOperator( new ConsoleOpDisconnect );
}

void ConsoleStateIdle::initState()
{
	addOperator( new ConsoleOpInCallSetup );
}

/* both DISCONNECT call and AnswerTimer event are process in this state */
void ConsoleStateInAlerting::initState()
{
	addOperator( new ConsoleOpDisconnect );
	addOperator( new ConsoleOpInCallAnswerTimer );
}

/*
* with CONNECT msg and CONNECT call event, from both STATE_XX_IN and STATE_XX_OUT into this state
*/
void ConsoleStateInGreeting::initState()
{
	/* added Timeout for alert voice */
	addEntryOperator(new EpOpStartTimer(PbxConfiguration::instance()->getConsoleAlertingTimeout() ) );
	
	addOperator( new ConsoleOpSetupInCall );			/*rx DTMF digit (out-band signal) */
	addOperator(new ConsoleOpInCallTimeout);
	addOperator( new ConsoleOpDisconnect );
	addOperator(new ConsoleOpStopDialTone);
	/* addDigit must be ahead of ReDial */
	addOperator( new EpOpAddDigit);
	/* when a DigitEvent pass to this operator, a new second call begin, Li Zhijie,2005.03.22  */		
	addOperator( new ConsoleOpInCallReDial );
	
	addExitOperator(new EpOpCancelTimer);
}

/* both DISCONNECT call and AnswerTimer event are process in this state */
void ConsoleStateOutAlerting::initState()
{
	addOperator( new ConsoleOpDisconnect );

	addOperator( new ConsoleOpOutCallRelease );
	addOperator( new ConsoleOpOutCallConnect );
	addOperator(new ConsoleOpOutCallTimeout );

}

/* both DISCONNECT call and AnswerTimer event are process in this state */
void ConsoleStateOutCalling::initState()
{
	/* CallEvent from CallSchduler for Slave EP */
	addOperator( new ConsoleOpOutCallRelease );
	addOperator( new ConsoleOpOutCallProceeding );
	addOperator( new ConsoleOpOutCallAlerting );
	addOperator( new ConsoleOpOutCallConnect );
	addOperator( new ConsoleOpOutCallTimeout );

	/* cancel timer started in ConsoleOpOutCallAlerting */
	addExitOperator(new EpOpCancelTimer);
}

