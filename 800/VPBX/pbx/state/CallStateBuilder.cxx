/*
* $Id: CallStateBuilder.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "CallStateBuilder.hxx"
#include "CallOpBuilder.hxx"

using namespace Assist;

void CallStateSetup::initState()
{
	cpLog(LOG_DEBUG, "CallStateSetup begining...");
	addOperator( new CallOpProceeding );
	addOperator( new CallOpAlerting);
	addOperator( new CallOpConnect );
	addOperator( new CallOpRelease );
	addOperator( new CallOpConnectAckBeforeConnect);		/* really enter into state of CONNECT in this operator */
	cpLog(LOG_DEBUG, "CallStateSetup ended...");
}

void CallStateProceeding::initState()
{
	cpLog(LOG_DEBUG, "CallStateProceeding begining...");
	addOperator( new CallOpAlerting);
	addOperator( new CallOpConnect );
	addOperator( new CallOpRelease );
	addOperator( new CallOpConnectAckBeforeConnect);		/* really enter into state of CONNECT in this operator */
	cpLog(LOG_DEBUG, "CallStateProceeding ended...");
}


void CallStateAlerting::initState()
{
	cpLog(LOG_DEBUG, "CallStateAlerting begining...");
	addOperator( new CallOpConnect );
	addOperator( new CallOpRelease );
	addOperator( new CallOpDisconnect);
	addOperator( new CallOpConnectAckBeforeConnect);		/* really enter into state of CONNECT in this operator */
	cpLog(LOG_DEBUG, "CallStateAlerting finished");
}


void CallStateConnect::initState()
{
	addOperator( new CallOpForwardSetupInCall );
	addOperator( new CallOpDisconnect );
	addOperator( new CallOpConnectAckInConnect);		/* only used for the scond call of console */
}

void CallStateDisconnect::initState()
{
}

/*
void CallStateError::initState()
{
	addEntryOperator( new CallOpClean );
}
*/

void CallStateIdle::initState()
{
	addOperator( new CallOpRouting );
	//addOperator( new CallOpRelease );
}

