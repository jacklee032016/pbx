/*
* $Id: MsgDecoderNT.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/
#include "cpLog.h"

#include "MsgDecoderNT.hxx"
#include "asnt_nins.h"

using namespace Assist;

MsgDecoderNT::MsgDecoderNT( )
	:MsgDecoder( )
{
	ntMode = 1;
	headerLength = ASNT_ISDNUSER_HEAD_SIZE;
}
		
MsgDecoderNT::~MsgDecoderNT()
{

}

/* SETUP msg */
const  unsigned char *MsgDecoderNT::getCallingPN4SetupMsg(const unsigned char *data)
{
	SETUP_t *setup = (SETUP_t *)(data);
	unsigned char *p = setup->CALLING_PN;

	return p;
}

const  unsigned char *MsgDecoderNT::getCalledPN4SetupMsg(const unsigned char *data)
{
	SETUP_t *setup = (SETUP_t *)(data);
	unsigned char *p = setup->CALLED_PN;

	return p;
}

#ifdef CENTREX
const  unsigned char *MsgDecoderNT::getCentrex4SetupMsg(const unsigned char *data)
{
	SETUP_t *setup = (SETUP_t *)(data );
	unsigned char *p = setup->FACILITY;

	return p;
}
#endif

const  unsigned char *MsgDecoderNT::getUserUser4SetupMsg(const unsigned char *data)
{
	SETUP_t *setup = (SETUP_t *)(data );
	unsigned char *p = setup->USER_USER;

	return p;
}

#if 0
const  unsigned char *MsgDecoderNT::getComplete4SetupMsg(const unsigned char *data)
{
	SETUP_t *setup = (SETUP_t *)(data);
	unsigned char *p = setup->COMPLETE;

	return p;
}
#endif

const  unsigned char *MsgDecoderNT::getRedirNR4SetupMsg(const unsigned char *data)
{
	SETUP_t *setup = (SETUP_t *)( data);
	unsigned char *p = setup->REDIR_NR;

	return p;
}

const  unsigned char *MsgDecoderNT::getBearer4SetupMsg(const unsigned char *data)
{
	SETUP_t *setup = (SETUP_t *)(data);
	unsigned char *p = setup->BEARER;

	return p;
}

const  unsigned char *MsgDecoderNT::getChanId4SetupMsg(const unsigned char *data)
{
	SETUP_t *setup = (SETUP_t *)(data);
	unsigned char *p = setup->CHANNEL_ID;

	return p;
}

/* for INFO msg */
const  unsigned char *MsgDecoderNT::getKeypad4InfoMsg(const unsigned char *data)
{
	INFORMATION_t *info = (INFORMATION_t *)(data);
	unsigned char *p = info->KEYPAD;

	return p;
}

const  unsigned char *MsgDecoderNT::getCalledPN4InfoMsg(const unsigned char *data)
{
	INFORMATION_t *info = (INFORMATION_t *)(data);
	unsigned char *p = info->CALLED_PN;

	return p;
}

#if 0
const  unsigned char *MsgDecoderNT::getComplete4InfoMsg(const unsigned char *data)
{
	INFORMATION_t *info = (INFORMATION_t *)( data);
	unsigned char *p = info->COMPLETE;

	return p;
}
#endif	

/* for SETUP_ACK msg */
const  unsigned char *MsgDecoderNT::getChanId4SetupAckMsg(const unsigned char *data)
{
	SETUP_ACKNOWLEDGE_t *setupAck = (SETUP_ACKNOWLEDGE_t *)(data);
	unsigned char *p = setupAck->CHANNEL_ID;

	return p;
}

const  unsigned char *MsgDecoderNT::getProgress4SetupAckMsg(const unsigned char *data)
{
	SETUP_ACKNOWLEDGE_t *setupAck = (SETUP_ACKNOWLEDGE_t *)(data);
	unsigned char *p = setupAck->PROGRESS;

	return p;
}

/* PROCEEDING msg */
const  unsigned char *MsgDecoderNT::getChanId4ProceedingMsg(const unsigned char *data)
{
	CALL_PROCEEDING_t *proceeding = (CALL_PROCEEDING_t *)(data);
	unsigned char *p = proceeding->CHANNEL_ID;

	return p;
}

const  unsigned char *MsgDecoderNT::getProgress4ProceedingMsg(const unsigned char *data)
{
	CALL_PROCEEDING_t *proceeding = (CALL_PROCEEDING_t *)(data);
	unsigned char *p = proceeding->PROGRESS;

	return p;
}

/* ALERTING msg */
const  unsigned char *MsgDecoderNT::getChanId4AlertingMsg(const unsigned char *data)
{
	ALERTING_t *alerting = (ALERTING_t *)(data);
	unsigned char *p = alerting->CHANNEL_ID;

	return p;
}

const  unsigned char *MsgDecoderNT::getProgress4AlertingMsg(const unsigned char *data)
{
	ALERTING_t *alerting = (ALERTING_t *)(data);
	unsigned char *p = alerting->PROGRESS;

	return p;
}


/* CONNECTION msg */
const  unsigned char *MsgDecoderNT::getChanId4ConnectMsg(const unsigned char *data)
{
	CONNECT_t *conn = (CONNECT_t *)(data);
	unsigned char *p = conn->CHANNEL_ID;

	return p;
}

#if 0
const  unsigned char *MsgDecoderNT::getConnectPN4ConnectMsg(const unsigned char *data)
{
	CONNECT_t *conn = (CONNECT_t *)(data);
	unsigned char *p = conn->CONNECT_PN;

	return p;
}
#endif

#ifdef CENTREX
const  unsigned char *MsgDecoderNT::getCentrex4ConnectMsg(const unsigned char *data)
{
	CONNECT_t *conn = (CONNECT_t *)(data);
	unsigned char *p = conn->FACILITY;

	return p;
}
#endif

/* DIS_CONNECTION msg */
const  unsigned char *MsgDecoderNT::getProgress4DisConnMsg(const unsigned char *data)
{
	DISCONNECT_t *disConn = (DISCONNECT_t *)(data);
	unsigned char *p = disConn->PROGRESS;

	return p;
}

const  unsigned char *MsgDecoderNT::getCause4DisConnMsg(const unsigned char *data)
{
	DISCONNECT_t *disConn = (DISCONNECT_t *)(data);
	unsigned char *p = disConn->CAUSE;

	return p;
}

const  unsigned char *MsgDecoderNT::getCharge4DisConnMsg(const unsigned char *data)
{
	return NULL;
}

/* NOTIFY msg */
const  unsigned char *MsgDecoderNT::getRedirDN4NotifyMsg(const unsigned char *data)
{
	NOTIFY_t *notifying = (NOTIFY_t *)(data);
	unsigned char *p = notifying->REDIR_DN;

	return p;
}
const  unsigned char *MsgDecoderNT::getNotify4NotifyMsg(const unsigned char *data)
{
	NOTIFY_t *notifying = (NOTIFY_t *)(data);
	unsigned char *p = notifying->NOTIFY;

	return p;
}

/* RELEASE msg */
const  unsigned char *MsgDecoderNT::getCause4ReleaseMsg(const unsigned char *data)
{
	RELEASE_t *rel = (RELEASE_t *)(data);
	unsigned char *p = rel->CAUSE;

	return p;
}

/* RELEASE_COMPLETE msg */
const  unsigned char *MsgDecoderNT::getCause4ReleaseCompleteMsg(const unsigned char *data)
{
	RELEASE_COMPLETE_t *relComp = (RELEASE_COMPLETE_t *)(data);
	unsigned char *p = relComp->CAUSE;

	return p;
}

/* SUSPEND msg */
const  unsigned char *MsgDecoderNT::getCallId4SuspendMsg(const unsigned char *data)
{
	SUSPEND_t *suspend = (SUSPEND_t *)(data );
	unsigned char *p = suspend->CALL_ID;

	return p;
}

/* RETRIEVE msg */
const  unsigned char *MsgDecoderNT::getChanId4RetrieveMsg(const unsigned char *data)
{
	RETRIEVE_t *retrieve = (RETRIEVE_t *)(data);
	unsigned char *p = retrieve->CHANNEL_ID;

	return p;
}

#if 0
/* FACILITY msg */
const  unsigned char *MsgDecoderNT::getFacilityPoint4FacilityMsg(const unsigned char *data)
{
	FACILITY_t *facility = (FACILITY_t *)(data);
	unsigned char	*p = facility->FACILITY;

	return p;
}
#endif

int MsgDecoderNT::getPrimType(const msg_t *msg)
{
	ASNT_ISDNUSER_HEADER	*hh;
	hh = (ASNT_ISDNUSER_HEADER *)msg->data;
	return hh->prim;
}

