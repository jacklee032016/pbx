/*
* $Id: IsdnOpBuilder.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#ifndef	__ISDN_OP_BUILDER_HXX__
#define	__ISDN_OP_BUILDER_HXX__

#include "IsdnOperator.hxx"
#include "EndPoint.hxx"
#include "PbxState.hxx"
#include "PbxTimerEvent.hxx"
#include "DigitEvent.hxx"
#include "CallEvent.hxx"
#include "CallMsgBuilder.hxx"


namespace Assist
{

#define ISDN_OP_DECLARE(type, name) \
class type : public IsdnOperator\
{ \
public: \
	type() : IsdnOperator(name) {} \
	~type(){} \
	virtual const Sptr <PbxState>  process(const Sptr <PbxEvent> event); \
}


/*
* process PROCEEDING CALL event come from CallScheduler
* This is a operation executed in SRC EP, eg. SRC_STATE 
*/
ISDN_OP_DECLARE(IsdnOpAlertingCall, 				"IsdnOpAlertingCall");
/*
* process ALERTING msg come from Device Monitor thread(network side) 
* Both in SETUP and Proceeding OUT state
*/
ISDN_OP_DECLARE(IsdnOpAlertingMsg, 				"IsdnOpAlertingMsg");

/*
* process CONNECT_ACK msg come from Device Monitor thread(network side) 
* Both in IN state 
*/
ISDN_OP_DECLARE(IsdnOpConnectAckMsg, 			"IsdnOpConnectAckMsg");
/*
* process CONNECT CALL event come from CallScheduler
* This is a operation executed in SRC EP, eg. SRC_STATE 
*/
ISDN_OP_DECLARE(IsdnOpConnectCall, 			"IsdnOpConnectCall");
/*
* process CONNECT msg come from Device Monitor thread(network side) 
* Both in SETUP,Proceeding and ALERTING OUT state
*/
ISDN_OP_DECLARE(IsdnOpConnectMsg, 			"IsdnOpConnectMsg");
/*
* process DISCONNECT CALL event come from CallScheduler
* send out DISCONNECT msg to ISDN device
*/
ISDN_OP_DECLARE(IsdnOpDisconnectCall, 			"IsdnOpDisconnectCall");

/*
* process DISCONNECT msg come from Device Monitor thread(network side) 
* Both in SETUP,Proceeding and ALERTING OUT state
* With PBX, when we send out SETUP, ISDN-PBX send out a DISCONNECT msg for cause of unalocated number 
*/
ISDN_OP_DECLARE(IsdnOpDisconnectMsg, 			"IsdnOpDisconnectMsg");
ISDN_OP_DECLARE(IsdnOpNewCC, 				"IsdnOpNewCC");

/* process PROCEEDING CALL event come from CallScheduler */
ISDN_OP_DECLARE(IsdnOpProceedingCall, 			"IsdnOpProceedingCall");
/* process PROCEEDING msg come from Device Monitor thread(network side) */
ISDN_OP_DECLARE(IsdnOpProceedingMsg, 			"IsdnOpProceedingMsg");

/*
* process RELEASE CALL event come from CallScheduler
* This is a operation executed both in SRC EP and DEST EP
*/
ISDN_OP_DECLARE(IsdnOpReleaseCall, 			"IsdnOpReleaseCall");	
/* process PROCEEDING CALL event come from CallScheduler */
ISDN_OP_DECLARE(IsdnOpReleaseCompleteCall, 		"IsdnOpReleaseCompleteCall");
/*
* process RELEASE_COMPLETE msg come from Device Monitor thread(network side) after send out RELEASE msg 
* Only used in IN state ,esp. DISCONNECT_IN state 
*/
ISDN_OP_DECLARE(IsdnOpReleaseCompleteMsg, 		"IsdnOpReleaseCompleteMsg");
/*
* process RELEASE_CR msg come from Device Monitor thread(network side) after ISDN connect has been broken
* This is a common state
*/
ISDN_OP_DECLARE(IsdnOpReleaseCRMsg, 			"IsdnOpReleaseCRMsg");
/*
* process ALERTING msg come from Device Monitor thread(network side) 
* Both in SETUP and Proceeding OUT state
*/
ISDN_OP_DECLARE(IsdnOpReleaseMsg, 			"IsdnOpReleaseMsg");

/* process SETUP CALL event come from CallScheduler when EP is state of idle */
ISDN_OP_DECLARE(IsdnOpSetupCall, 				"IsdnOpSetupCall");

/* process SETUP msg come from Device Monitor thread */
ISDN_OP_DECLARE(IsdnOpSetupMsg, 				"IsdnOpSetupMsg");	

}


#include "IeBearer.hxx"
#include "IeCallingPN.hxx"
#include "IeCalledPN.hxx"
#include "IeChanId.hxx"
#include "IeCause.hxx"
#include "IeCharge.hxx"

#include "MsgCCRelease.hxx"
#include "MsgCCNew.hxx"
#include "MsgSetup.hxx"
#include "MsgDisConnect.hxx"
#include "MsgAlerting.hxx"
#include "MsgProceeding.hxx"
#include "MsgConnect.hxx"
#include "MsgConnectAck.hxx"
#include "MsgRelease.hxx"
#include "MsgReleaseComplete.hxx"


#include "IsdnMsgFactory.hxx"

#include "CallEvent.hxx"
#include "CallMsgBuilder.hxx"

#include "IsdnDeviceMgr.hxx"

#endif

