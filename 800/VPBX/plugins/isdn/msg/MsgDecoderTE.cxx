/*
* $Id: MsgDecoderTE.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/
#include "cpLog.h"
#include "ask_isdn.h"
#include "MsgDecoderTE.hxx"

using namespace Assist;

MsgDecoderTE::MsgDecoderTE( )
	:MsgDecoder( )
{
	ntMode = 0 ;
	headerLength = AS_ISDN_HEADER_LEN;
}
		
MsgDecoderTE::~MsgDecoderTE()
{
}


Q931_info_t *MsgDecoderTE::getQ931Info( const iframe_t  *frm)
{
//	Q931_info_t *qi = (Q931_info_t *)frm->data.p;
	unsigned char *p = (unsigned char *)((unsigned char *) frm + 16 );
	
	Q931_info_t *qi = (Q931_info_t *)p ; //((unsigned char *)_frm->data+2);

	return qi;
}

const unsigned char *MsgDecoderTE::getChanId(const unsigned char *data)
{
	Q931_info_t *qi = (Q931_info_t *)data;
	const unsigned char *p = NULL;

	if (qi->channel_id)
	{	
		p = data + sizeof(Q931_info_t) + qi->channel_id ;
		p = checkTeIE( p,IE_CHANNEL_ID);
	}	
	return p;
	
}

const unsigned char *MsgDecoderTE::getCallId(const unsigned char *data)
{
	Q931_info_t *qi = (Q931_info_t *) data;
	const unsigned char *p = NULL;

	if (qi->call_id)
	{	
		p = data + sizeof(Q931_info_t) + qi->call_id;
		p = checkTeIE( p, IE_CALL_ID);
	}
	return p;
}

const unsigned char *MsgDecoderTE::getProgress(const unsigned char *data)
{
	Q931_info_t *qi = (Q931_info_t *)data;
	const unsigned char *p = NULL;

	if (qi->progress)
	{	
		p = data + sizeof(Q931_info_t) + qi->progress ;
		p = checkTeIE( p, IE_PROGRESS);
	}
	return p;
}

const unsigned char *MsgDecoderTE::getCause(const unsigned char *data)
{
	Q931_info_t *qi = (Q931_info_t *)data;
	const unsigned char *p = NULL;

	if (qi->cause)
	{	
		p = data + sizeof(Q931_info_t) + qi->cause ;
		p = checkTeIE( p, IE_CAUSE );
	}
	return p;
}

const unsigned char *MsgDecoderTE::getFacility(const unsigned char *data)
{
	Q931_info_t *qi = (Q931_info_t *)data;
	const unsigned char *p = NULL;

	if (qi->facility)
	{
		p = data + sizeof(Q931_info_t) + qi->facility ;
		p= checkTeIE( p, IE_FACILITY);
	}
	return p;
}

const unsigned char *MsgDecoderTE::getCalledPN(const unsigned char *data)
{
	Q931_info_t *qi = (Q931_info_t *)data;
	const unsigned char *p = NULL;

	if (qi->called_nr)
	{	
		p = data + sizeof(Q931_info_t) + qi->called_nr ;
		p = checkTeIE(p, IE_CALLED_PN);
	}
	return p;
}

#if 0
const unsigned char *MsgDecoderTE::getComplete(const unsigned char *data)
{/* IE COMPLETE has no content, only IE_TYPE */
	Q931_info_t *qi = (Q931_Info_t *)data;
	const unsigned char *p = NULL;

	if (qi->sending_complete)
	{	
		p = data+ sizeof(Q931_info_t) + qi->sending_complete ;
		return NULL;
	}
	return p;
}
#endif


/* SETUP msg */
const unsigned char *MsgDecoderTE::getCallingPN4SetupMsg(const unsigned char *data)
{
	Q931_info_t *qi = (Q931_info_t *)data;
	const unsigned char *p = NULL;

	if (qi->calling_nr)
	{	
		p = data + sizeof(Q931_info_t) + qi->calling_nr ;
		p = checkTeIE( p, IE_CALLING_PN);
	}
	return p;
}
const  unsigned char *MsgDecoderTE::getCalledPN4SetupMsg(const unsigned char *data)
{
	return getCalledPN(data);
}


#ifdef CENTREX
const  unsigned char *MsgDecoderTE::getCentrex4SetupMsg(const unsigned char *data)
{
	return getFacility( data);
}
#endif

const  unsigned char *MsgDecoderTE::getUserUser4SetupMsg(const unsigned char *data)
{
	return getFacility( data);
}

#if 0
const  unsigned char *MsgDecoderTE::getComplete4SetupMsg(const unsigned char *data)
{
	return getComplete( data);;
}
#endif

const  unsigned char *MsgDecoderTE::getRedirNR4SetupMsg(const unsigned char *data)
{
	Q931_info_t *qi = (Q931_info_t *)data;
	const unsigned char *p = NULL;

	if (qi->redirect_nr)
	{	
		p = data + sizeof(Q931_info_t) + qi->redirect_nr;
		p = checkTeIE(p,  IE_REDIR_NR);
	}	
	
	return p;
}
const  unsigned char *MsgDecoderTE::getBearer4SetupMsg(const unsigned char *data)
{
	Q931_info_t *qi = (Q931_info_t *)data;
	const unsigned char *p = NULL;

	if (qi->bearer_capability)
	{	
		p = data + sizeof(Q931_info_t) + qi->bearer_capability;
		p = checkTeIE( p, IE_BEARER);
	}
	
	
	return p;
}
const  unsigned char *MsgDecoderTE::getChanId4SetupMsg(const unsigned char *data)
{
	return getChanId(data);;
}


/* for INFO msg */
const  unsigned char *MsgDecoderTE::getKeypad4InfoMsg(const unsigned char *data)
{
	Q931_info_t *qi = (Q931_info_t *)data;
	const unsigned char *p = NULL;

	if (qi->keypad)
	{	
		p = data + sizeof(Q931_info_t) + qi->keypad ;
		p = checkTeIE(p, IE_KEYPAD);
	}
	
	return p;
}
const  unsigned char *MsgDecoderTE::getCalledPN4InfoMsg(const unsigned char *data)
{
	return getCalledPN(data);;
}

#if 0
const  unsigned char *MsgDecoderTE::getComplete4InfoMsg(const unsigned char *data)
{
	return getComplete( data);
}
#endif	

/* for SETUP_ACK msg */
const  unsigned char *MsgDecoderTE::getChanId4SetupAckMsg(const unsigned char *data)
{
	return getChanId(data);
}
const  unsigned char *MsgDecoderTE::getProgress4SetupAckMsg(const unsigned char *data)
{
	return getProgress( data);
}

/* PROCEEDING msg */
const  unsigned char *MsgDecoderTE::getChanId4ProceedingMsg(const unsigned char *data)
{
	return getChanId( data);
}
const  unsigned char *MsgDecoderTE::getProgress4ProceedingMsg(const unsigned char *data)
{
	return getProgress( data);
}


/* ALERTING msg */
const  unsigned char *MsgDecoderTE::getChanId4AlertingMsg(const unsigned char *data)
{
	return getChanId(data);
}
const  unsigned char *MsgDecoderTE::getProgress4AlertingMsg(const unsigned char *data)
{
	return getProgress( data);
}


/* CONNECTION msg */
const  unsigned char *MsgDecoderTE::getChanId4ConnectMsg(const unsigned char *data)
{
	return getChanId(data);
}

#if 0
const  unsigned char *MsgDecoderTE::getConnectPN4ConnectMsg(const unsigned char *data)
{
	Q931_info_t *qi = (Q931_Info_t *)data;
	const char *p = NULL;

	if (qi->connected_nr)
	{	
		p = data + sizeof(Q931_info_t) + qi->connected_nr ;
		p = checkTeIE(p, IE_CONNECT_PN);
	}
	return p;
}
#endif

#ifdef CENTREX
const  unsigned char *MsgDecoderTE::getCentrex4ConnectMsg(const unsigned char *data)
{
	return getFacility( data);
}
#endif

/* DIS_CONNECTION msg */
const  unsigned char *MsgDecoderTE::getProgress4DisConnMsg(const unsigned char *data)
{
	return getProgress( data);
}
const  unsigned char *MsgDecoderTE::getCause4DisConnMsg(const unsigned char *data)
{
	return getCause( data);
}

const  unsigned char *MsgDecoderTE::getCharge4DisConnMsg(const unsigned char *data)
{
	Q931_info_t *qi = (Q931_info_t *)data;
	const unsigned char *p = NULL;

	if (qi->fill1)
	{	
		p = data + sizeof(Q931_info_t) + qi->fill1 ;
		p = checkTeIE( p, IE_CODESET_6);
	}
	if( !p)
		return NULL;

	if(p[0] == IE_ADVISE_CHARGE)
	{
		p++;
		return p;
	}	
	return NULL;
}

/* NOTIFY msg */
const  unsigned char *MsgDecoderTE::getRedirDN4NotifyMsg(const unsigned char *data)
{
	cpLog(LOG_WARNING, "not used in TE mode");	
//#warning REINSERT redir_dn, when included in te-mode
//		if (qi->redir_dn)
//			p = (const  char *)qi + sizeof(Q931_info_t) + qi->redir_dn + 1;
	return NULL;
}
const  unsigned char *MsgDecoderTE::getNotify4NotifyMsg(const unsigned char *data)
{
	Q931_info_t *qi = (Q931_info_t *)data;
	const unsigned char *p = NULL;

	if (qi->notify)
	{	
		p = data + sizeof(Q931_info_t) + qi->notify;
		p = checkTeIE(p, IE_NOTIFY);
	}
	
	return p;
}

/* RELEASE msg */
const  unsigned char *MsgDecoderTE::getCause4ReleaseMsg(const unsigned char *data)
{
	return getCause( data);
}

/* RELEASE_COMPLETE msg */
const  unsigned char *MsgDecoderTE::getCause4ReleaseCompleteMsg(const unsigned char *data)
{
	return getCause( data);
}

/* SUSPEND msg */
const  unsigned char *MsgDecoderTE::getCallId4SuspendMsg(const unsigned char *data)
{
	return getCallId( data);
}

/* RETRIEVE msg */
const  unsigned char *MsgDecoderTE::getChanId4RetrieveMsg(const unsigned char *data)
{
	return getChanId( data);
}

#if 0
const  unsigned char *MsgDecoderTE::getCallId4SuspendMsg(const unsigned char *data)
{
	return getCallId( data);
}
#endif

#if 0
const  unsigned char *MsgDecoderTE::getFacilityPoint4FacilityMsg(const unsigned char *data)
{
	return getFacility( data);
}
#endif

int MsgDecoderTE::getPrimType(const msg_t *msg)
{
	iframe_t	*frm;
	frm = (iframe_t *)msg->data;
	return frm->prim;
}

const unsigned char *MsgDecoderTE::checkTeIE(const unsigned char *ieData, int ieType)
{
	if(ieData[0]!=ieType)
	{
		return NULL;
	}

	ieData++;
	return ieData;
}

