/*
* $Id: MsgEncoderNT.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/
#include "cpLog.h"

#include "MsgEncoderNT.hxx"
#include "asnt_nins.h"

using namespace Assist;

MsgEncoderNT::MsgEncoderNT( )
	:MsgEncoder( )
{
	ntMode = 1;
	headerLength = ASNT_ISDNUSER_HEAD_SIZE;
}
		
MsgEncoderNT::~MsgEncoderNT()
{

}

/* SETUP msg */
int MsgEncoderNT::encCallingPN4SetupMsg(unsigned char *header,  unsigned char *ie)
{
	SETUP_t *setup = (SETUP_t *)(header);
	setup->CALLING_PN = ie;

	return 0;
}

int MsgEncoderNT::encCalledPN4SetupMsg(unsigned char *header,  unsigned char *ie)
{
	SETUP_t *setup = (SETUP_t *)(header);
	setup->CALLED_PN = ie;

	return 0;
}

#ifdef CENTREX
int MsgEncoderNT::encCentrex4SetupMsg(unsigned char *header,  unsigned char *ie)
{
	SETUP_t *setup = (SETUP_t *)(header );
	setup->FACILITY = ie;

	return 0;
}
#endif

int MsgEncoderNT::encUserUser4SetupMsg(unsigned char *header,  unsigned char *ie)
{
	SETUP_t *setup = (SETUP_t *)(header );
	setup->USER_USER = ie;

	return 0;
}

#if 0
int MsgEncoderNT::encComplete4SetupMsg(unsigned char *header,  unsigned char *ie)
{
	SETUP_t *setup = (SETUP_t *)(header);
	setup->COMPLETE = ie;

	return 0;
}
#endif

int MsgEncoderNT::encRedirNR4SetupMsg(unsigned char *header,  unsigned char *ie)
{
	SETUP_t *setup = (SETUP_t *)(header);
	setup->REDIR_NR = ie;

	return 0;
}

int MsgEncoderNT::encBearer4SetupMsg(unsigned char *header,  unsigned char *ie)
{
	SETUP_t *setup = (SETUP_t *)(header);
	setup->BEARER = ie;

	return 0;
}

int MsgEncoderNT::encChanId4SetupMsg(unsigned char *header,  unsigned char *ie)
{
	SETUP_t *setup = (SETUP_t *)(header);
	setup->CHANNEL_ID = ie;

	return 0;
}


int MsgEncoderNT::encCallingSub4SetupMsg(unsigned char *header,  unsigned char *ie)
{
	SETUP_t *setup = (SETUP_t *)(header);
	setup->CALLING_SUB= ie;

	return 0;
}

int MsgEncoderNT::encCalledSub4SetupMsg(unsigned char *header,  unsigned char *ie)
{
	SETUP_t *setup = (SETUP_t *)(header);
	setup->CALLED_SUB = ie;

	return 0;
}

int MsgEncoderNT::encProgress4SetupMsg(unsigned char *header,  unsigned char *ie)
{
	SETUP_t *setup = (SETUP_t *)(header);
	setup->PROGRESS = ie;

	return 0;
}

int MsgEncoderNT::encDisplay4SetupMsg(unsigned char *header,  unsigned char *ie)
{
	SETUP_t *setup = (SETUP_t *)(header);
	setup->DISPLAY = ie;

	return 0;
}

int MsgEncoderNT::encSignal4SetupMsg(unsigned char *header,  unsigned char *ie)
{
	SETUP_t *setup = (SETUP_t *)(header);
	setup->SIGNAL = ie;

	return 0;
}

int MsgEncoderNT::encKeypad4SetupMsg(unsigned char *header,  unsigned char *ie)
{
	SETUP_t *setup = (SETUP_t *)(header);
	setup->KEYPAD = ie;

	return 0;
}

int MsgEncoderNT::encFacility4SetupMsg(unsigned char *header,  unsigned char *ie)
{
	SETUP_t *setup = (SETUP_t *)(header);
	setup->FACILITY = ie;

	return 0;
}



/********************* for INFO msg **********************/
int MsgEncoderNT::encKeypad4InfoMsg(unsigned char *header,  unsigned char *ie)
{
	INFORMATION_t *info = (INFORMATION_t *)(header);
	info->KEYPAD = ie;
	return 0;
}
int MsgEncoderNT::encCalledPN4InfoMsg(unsigned char *header,  unsigned char *ie)
{
	INFORMATION_t *info = (INFORMATION_t *)(header);
	info->CALLED_PN = ie;
	return 0;
}
int MsgEncoderNT::encSignal4InfoMsg(unsigned char *header,  unsigned char *ie)
{
	INFORMATION_t *info = (INFORMATION_t *)(header);
	info->SIGNAL = ie;
	return 0;
}
int MsgEncoderNT::encDisplay4InfoMsg(unsigned char *header,  unsigned char *ie)
{
	INFORMATION_t *info = (INFORMATION_t *)(header);
	info->DISPLAY = ie;
	return 0;
}

#if 0
int MsgEncoderNT::encComplete4InfoMsg(unsigned char *header,  unsigned char *ie)
{
	INFORMATION_t *info = (INFORMATION_t *)(header);
	info->COMPLETE = ie;

	return 0;
}
#endif	

/* for SETUP_ACK msg */
int MsgEncoderNT::encChanId4SetupAckMsg(unsigned char *header,  unsigned char *ie)
{
	SETUP_ACKNOWLEDGE_t *setupAck = (SETUP_ACKNOWLEDGE_t *)(header);
	setupAck->CHANNEL_ID = ie;

	return 0;
}

int MsgEncoderNT::encProgress4SetupAckMsg(unsigned char *header,  unsigned char *ie)
{
	SETUP_ACKNOWLEDGE_t *setupAck = (SETUP_ACKNOWLEDGE_t *)(header);
	setupAck->PROGRESS = ie;

	return 0;
}

/* PROCEEDING msg */
int MsgEncoderNT::encChanId4ProceedingMsg(unsigned char *header,  unsigned char *ie)
{
	CALL_PROCEEDING_t *proceeding = (CALL_PROCEEDING_t *)(header);
	proceeding->CHANNEL_ID = ie;
	return 0;
}
int MsgEncoderNT::encProgress4ProceedingMsg(unsigned char *header,  unsigned char *ie)
{
	CALL_PROCEEDING_t *proceeding = (CALL_PROCEEDING_t *)(header);
	proceeding->PROGRESS = ie;
	return 0;
}
int MsgEncoderNT::encDisplay4ProceedingMsg(unsigned char *header,  unsigned char *ie)
{
	CALL_PROCEEDING_t *proceeding = (CALL_PROCEEDING_t *)(header);
	proceeding->DISPLAY = ie;
	return 0;
}
int MsgEncoderNT::encFacility4ProceedingMsg(unsigned char *header,  unsigned char *ie)
{
	CALL_PROCEEDING_t *proceeding = (CALL_PROCEEDING_t *)(header);
	proceeding->FACILITY = ie;
	return 0;
}


/******** ALERTING msg ********************/
int MsgEncoderNT::encChanId4AlertingMsg(unsigned char *header,  unsigned char *ie)
{
	ALERTING_t *alerting = (ALERTING_t *)(header);
	alerting->CHANNEL_ID = ie;

	return 0;
}
int MsgEncoderNT::encProgress4AlertingMsg(unsigned char *header,  unsigned char *ie)
{
	ALERTING_t *alerting = (ALERTING_t *)(header);
	alerting->PROGRESS = ie;

	return 0;
}
int MsgEncoderNT::encDisplay4AlertingMsg(unsigned char *header,  unsigned char *ie)
{
	ALERTING_t *alerting = (ALERTING_t *)(header);
	alerting->DISPLAY = ie;

	return 0;
}
int MsgEncoderNT::encSignal4AlertingMsg(unsigned char *header,  unsigned char *ie)
{
	ALERTING_t *alerting = (ALERTING_t *)(header);
	alerting->SIGNAL= ie;

	return 0;
}


/************************* CONNECTION msg ***********************/
int MsgEncoderNT::encChanId4ConnectMsg(unsigned char *header,  unsigned char *ie)
{
	CONNECT_t *conn = (CONNECT_t *)(header);
	conn->CHANNEL_ID = ie;
	return 0;
}
int MsgEncoderNT::encProgress4ConnectMsg(unsigned char *header,  unsigned char *ie)
{
	CONNECT_t *conn = (CONNECT_t *)(header);
	conn->PROGRESS = ie;
	return 0;
}
int MsgEncoderNT::encDisplay4ConnectMsg(unsigned char *header,  unsigned char *ie)
{
	CONNECT_t *conn = (CONNECT_t *)(header);
	conn->DISPLAY = ie;
	return 0;
}
int MsgEncoderNT::encSignal4ConnectMsg(unsigned char *header,  unsigned char *ie)
{
	CONNECT_t *conn = (CONNECT_t *)(header);
	conn->SIGNAL = ie;
	return 0;
}

/*********************   CONNECT_ACK msg ******************/
int MsgEncoderNT::encDisplay4ConnectAckMsg(unsigned char *header,  unsigned char *ie)
{
	CONNECT_ACKNOWLEDGE_t  *connAck = (CONNECT_ACKNOWLEDGE_t *)(header);
	connAck->DISPLAY = ie;
	return 0;
}
int MsgEncoderNT::encSignal4ConnectAckMsg(unsigned char *header,  unsigned char *ie)
{
	CONNECT_ACKNOWLEDGE_t *connAck = (CONNECT_ACKNOWLEDGE_t *)(header);
	connAck->SIGNAL = ie;
	return 0;
}

#if 0
int MsgEncoderNT::encConnectPN4ConnectMsg(unsigned char *header,  unsigned char *ie)
{
	CONNECT_t *conn = (CONNECT_t *)(header);
	conn->CONNECT_PN = ie;

	return 0;
}
#endif

#ifdef CENTREX
int MsgEncoderNT::encCentrex4ConnectMsg(unsigned char *header,  unsigned char *ie)
{
	CONNECT_t *conn = (CONNECT_t *)(header);
	conn->FACILITY = ie;

	return 0;
}
#endif

/* DIS_CONNECTION msg */
int MsgEncoderNT::encProgress4DisConnMsg(unsigned char *header,  unsigned char *ie)
{
	DISCONNECT_t *disConn = (DISCONNECT_t *)(header);
	disConn->PROGRESS = ie;

	return 0;
}
int MsgEncoderNT::encCause4DisConnMsg(unsigned char *header,  unsigned char *ie)
{
	DISCONNECT_t *disConn = (DISCONNECT_t *)(header);
	disConn->CAUSE = ie;

	return 0;
}
int MsgEncoderNT::encDisplay4DisConnMsg(unsigned char *header,  unsigned char *ie)
{
	DISCONNECT_t  *disConn = (DISCONNECT_t *)(header);
	disConn->DISPLAY = ie;
	return 0;
}
int MsgEncoderNT::encSignal4DisConnMsg(unsigned char *header,  unsigned char *ie)
{
	DISCONNECT_t *disConn = (DISCONNECT_t *)(header);
	disConn->SIGNAL = ie;
	return 0;
}

int MsgEncoderNT::encCharge4DisConnMsg(unsigned char *header,  unsigned char *ie)
{
	DISCONNECT_t *disConn = (DISCONNECT_t *)(header);
	disConn->CHARGE = ie;
	return 0;
}

/* NOTIFY msg */
int MsgEncoderNT::encRedirDN4NotifyMsg(unsigned char *header,  unsigned char *ie)
{
	NOTIFY_t *notifying = (NOTIFY_t *)(header);
	notifying->REDIR_DN = ie;
	return 0;
}
int MsgEncoderNT::encNotify4NotifyMsg(unsigned char *header,  unsigned char *ie)
{
	NOTIFY_t *notifying = (NOTIFY_t *)(header);
	notifying->NOTIFY = ie;
	return 0;
}
int MsgEncoderNT::encDisplay4NotifyMsg(unsigned char *header,  unsigned char *ie)
{
	NOTIFY_t *notifying = (NOTIFY_t *)(header);
	notifying->DISPLAY = ie;
	return 0;
}

/************************ RELEASE msg *******************/
int MsgEncoderNT::encCause4ReleaseMsg(unsigned char *header,  unsigned char *ie)
{
	RELEASE_t *rel = (RELEASE_t *)(header);
	rel->CAUSE = ie;
	return 0;
}
int MsgEncoderNT::encDisplay4ReleaseMsg(unsigned char *header,  unsigned char *ie)
{
	RELEASE_t *rel = (RELEASE_t *)(header);
	rel->DISPLAY = ie;
	return 0;
}
int MsgEncoderNT::encSignal4ReleaseMsg(unsigned char *header,  unsigned char *ie)
{
	RELEASE_t *rel = (RELEASE_t *)(header);
	rel->SIGNAL = ie;
	return 0;
}



/* RELEASE_COMPLETE msg */
int MsgEncoderNT::encCause4ReleaseCompleteMsg(unsigned char *header,  unsigned char *ie)
{
	RELEASE_COMPLETE_t *relComp = (RELEASE_COMPLETE_t *)(header);
	relComp->CAUSE = ie;
	return 0;
}
int MsgEncoderNT::encDisplay4ReleaseCompleteMsg(unsigned char *header,  unsigned char *ie)
{
	RELEASE_COMPLETE_t *relComp = (RELEASE_COMPLETE_t *)(header);
	relComp->DISPLAY = ie;
	return 0;
}
int MsgEncoderNT::encSignal4ReleaseCompleteMsg(unsigned char *header,  unsigned char *ie)
{
	RELEASE_COMPLETE_t *relComp = (RELEASE_COMPLETE_t *)(header);
	relComp->SIGNAL = ie;
	return 0;
}

/* SUSPEND msg */
int MsgEncoderNT::encCallId4SuspendMsg(unsigned char *header,  unsigned char *ie)
{
	SUSPEND_t *suspend = (SUSPEND_t *)(header);
	suspend->CALL_ID = ie;
	return 0;
}

/************** RESUME msg ********************/
int MsgEncoderNT::encCallId4ResumeMsg(unsigned char *header,  unsigned char *ie)
{
	RESUME_t *resume = (RESUME_t *)(header);
	resume->CALL_ID = ie;
	return 0;
}

/* RETRIEVE msg */
int MsgEncoderNT::encChanId4RetrieveMsg(unsigned char *header,  unsigned char *ie)
{
	RETRIEVE_t *retrieve = (RETRIEVE_t *)(header);
	retrieve->CHANNEL_ID = ie;

	return 0;
}

#if 0
/* FACILITY msg */
int MsgEncoderNT::encFacilityPoint4FacilityMsg(unsigned char *header,  unsigned char *ie)
{
	FACILITY_t *facility = (FACILITY_t *)(header);
	facility->FACILITY = ie;

	return 0;
}
#endif
int MsgEncoderNT::encDisplay4FacilityMsg(unsigned char *header,  unsigned char *ie)
{
	FACILITY_t *facility = (FACILITY_t *)(header);
	facility->DISPLAY = ie;
	return 0;
}

