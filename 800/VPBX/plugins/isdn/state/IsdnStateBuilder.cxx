/*
* $Id: IsdnStateBuilder.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "IsdnStateBuilder.hxx"
#include "IsdnOpBuilder.hxx"

#include "EpOpBuilder.hxx"

#include "PbxConfiguration.hxx"

using namespace Assist;

/*
* With ALERTING call event, from STATE_SETUP_IN or STATE_PROCEEDING_IN into this state 
*/
void IsdnStateAlertingIn::initState()
{

	addOperator( new IsdnOpConnectCall );
	addOperator( new IsdnOpConnectAckMsg );
	addOperator( new IsdnOpReleaseMsg );
	addOperator( new IsdnOpReleaseCall );

	/* TA send DisConnect when in ALERTING state in the case of terminate call by TA, 2006.06.13  */
	addOperator( new IsdnOpDisconnectMsg );
}

void IsdnStateAlertingOut::initState()
{

	addOperator( new IsdnOpConnectMsg );
	addOperator( new IsdnOpReleaseMsg );
	addOperator( new IsdnOpReleaseCall );
	addOperator( new IsdnOpDisconnectCall);

#if 0
	addExitOperator( new OpStopRingbackTone );
#endif
}

/*
* with CONNECT msg and CONNECT call event, from both STATE_XX_IN and STATE_XX_OUT into this state
*/
void IsdnStateConnect::initState()
{
	addOperator( new IsdnOpDisconnectCall );
	addOperator( new IsdnOpDisconnectMsg );
	addOperator( new EpOpCallReleaseComplete(ISDN_EP_STATE_IN_DISCONNECT) );
}

void IsdnStateDisconnectIn::initState()
{
	/* for NT stack, RELEASE_COMPLETE msg is replaced with RELEASE_CR msg */
	addOperator(new IsdnOpReleaseCRMsg);
	addOperator( new IsdnOpReleaseCompleteMsg );

}

void IsdnStateDisconnectOut::initState()
{
	addOperator( new IsdnOpReleaseMsg );
//	addOperator( new IsdnOpReleaseCall );

	/* in NT user stack, when reply release_complete to SETUP, a RELEASE_CR is feedback */
	addOperator( new IsdnOpReleaseCRMsg );
#if 0
	addEntryOperator( new OpStartBusyTone );
	addExitOperator( new OpStopBusyTone );
#endif
}

void IsdnStateError::initState()
{
	addOperator( new IsdnOpSetupCall );
}

void IsdnStateIdle::initState()
{
	/* Ops for ISDN out Call */
	addOperator( new IsdnOpSetupCall );
	
	/* Ops for ISDN in Call */
	addOperator( new IsdnOpNewCC );
	addOperator( new IsdnOpSetupMsg );
}

/*
* With PROCEEDING callevent, from STATE_SETUP_IN into this state 
*/
void IsdnStateProceedingIn::initState()
{
#if 0
	addEntryOperator( new OpStartBusyTone );
	addExitOperator( new OpStopBusyTone );
#endif	

	addOperator( new IsdnOpAlertingCall );
	addOperator( new IsdnOpConnectCall );
	addOperator( new IsdnOpReleaseMsg );

	/* after ConnectCall to enter into CONNECT state */
	addOperator( new IsdnOpConnectAckMsg );
	
//	addOperator( new IsdnOpReleaseCompleteCall );
	
	addOperator( new IsdnOpReleaseCall );

	/* TA send DisConnect when in ALERTING state in the case of terminate call by TA, 2006.06.13  */
	addOperator( new IsdnOpDisconnectMsg );
}

void IsdnStateProceedingOut::initState()
{
#if 0
	addEntryOperator( new OpStartDialTone );
#endif	

	/* Operations for ISDN msg from network side */
	addOperator( new IsdnOpAlertingMsg );
	addOperator( new IsdnOpConnectMsg );
	addOperator( new IsdnOpReleaseMsg );

	/* operations for Call Eveent come from Call Scheduler */
	addOperator( new IsdnOpReleaseCall );
}

void IsdnStateSetupIn::initState()
{
//	addEntryOperator( new IsdnOpNewCC );

	addOperator( new IsdnOpProceedingCall );
	addOperator( new IsdnOpAlertingCall );
	addOperator( new IsdnOpConnectCall );
	addOperator( new IsdnOpConnectAckMsg );
	addOperator( new IsdnOpReleaseCall );
	addOperator( new IsdnOpReleaseMsg );

	/* TA send DisConnect when in ALERTING state in the case of terminate call by TA, 2006.06.13  */
	addOperator( new IsdnOpDisconnectMsg );
#if 0
	addOperator( new IsdnOpReleaseCompleteCall );
	addExitOperator( );
#endif
}


void IsdnStateSetupOut::initState()
{
/*  CC_NEW_CR_CNF and SETUP are send in IDLE state*/

	/* Operations for ISDN msg from network side */
	addOperator( new IsdnOpProceedingMsg );
	addOperator( new IsdnOpAlertingMsg );
	addOperator( new IsdnOpConnectMsg );
	addOperator( new IsdnOpReleaseMsg );

	/* no DISCONNECT can be received in this state */
//	addOperator( new IsdnOpDisconnectMsg );

	/* operations for Call Eveent come from Call Scheduler */
	addOperator( new IsdnOpReleaseCall );

}

void IsdnStateWaitClearCR::initState()
{
	addOperator( new IsdnOpReleaseCRMsg );
}

