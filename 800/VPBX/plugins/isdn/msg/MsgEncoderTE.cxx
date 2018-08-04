/*
* $Id: MsgEncoderTE.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/
#include "cpLog.h"
#include "ask_isdn.h"
#include "MsgEncoderTE.hxx"

using namespace Assist;

MsgEncoderTE::MsgEncoderTE( )
	:MsgEncoder( )
{
	ntMode = 0;
	headerLength = AS_ISDN_HEADER_LEN;
}
		
MsgEncoderTE::~MsgEncoderTE()
{
}

int MsgEncoderTE::encChanId(unsigned char *header,  unsigned char *ie)
{
	Q931_info_t *qi = (Q931_info_t *)header;
	qi->channel_id = (unsigned char *)ie - header - sizeof(Q931_info_t) ;

	return 0;
}

int MsgEncoderTE::encCallId(unsigned char *header,  unsigned char *ie)
{
	Q931_info_t *qi = (Q931_info_t *)header;
	qi->call_id = ie - header - sizeof(Q931_info_t) ;

	return 0;
}

int MsgEncoderTE::encProgress(unsigned char *header,  unsigned char *ie)
{
	Q931_info_t *qi = (Q931_info_t *)header;
	qi->progress = ie - header - sizeof(Q931_info_t) ;

	return 0;
}

int MsgEncoderTE::encCause(unsigned char *header,  unsigned char *ie)
{
	Q931_info_t *qi = (Q931_info_t *)header;
	qi->cause = ie - header - sizeof(Q931_info_t) ;

	return 0;
}

int MsgEncoderTE::encFacility(unsigned char *header,  unsigned char *ie)
{
	Q931_info_t *qi = (Q931_info_t *)header;
	qi->facility = ie - header - sizeof(Q931_info_t) ;

	return 0;
}

int MsgEncoderTE::encCalledPN(unsigned char *header,  unsigned char *ie)
{
	Q931_info_t *qi = (Q931_info_t *)header;
	qi->called_nr = ie - header - sizeof(Q931_info_t) ;

	return 0;
}

int MsgEncoderTE::encCallingPN(unsigned char *header,  unsigned char *ie)
{
	Q931_info_t *qi = (Q931_info_t *)header;
	qi->calling_nr = ie - header - sizeof(Q931_info_t) ;

	return 0;
}

int MsgEncoderTE::encCalledSub(unsigned char *header,  unsigned char *ie)
{
	Q931_info_t *qi = (Q931_info_t *)header;
	qi->called_sub = ie - header - sizeof(Q931_info_t) ;

	return 0;
}

int MsgEncoderTE::encCallingSub(unsigned char *header,  unsigned char *ie)
{
	Q931_info_t *qi = (Q931_info_t *)header;
	qi->calling_sub = ie - header - sizeof(Q931_info_t) ;

	return 0;
}

int MsgEncoderTE::encUserUser(unsigned char *header,  unsigned char *ie)
{
	Q931_info_t *qi = (Q931_info_t *)header;
	qi->useruser = ie - header - sizeof(Q931_info_t) ;

	return 0;
}

int MsgEncoderTE::encRedirNR(unsigned char *header,  unsigned char *ie)
{
	Q931_info_t *qi = (Q931_info_t *)header;
	qi->redirect_nr = ie - header - sizeof(Q931_info_t) ;

	return 0;
}

int MsgEncoderTE::encBearer(unsigned char *header,  unsigned char *ie)
{
	Q931_info_t *qi = (Q931_info_t *)header;
	qi->bearer_capability = ie - header - sizeof(Q931_info_t) ;

	return 0;
}

int MsgEncoderTE::encKeypad(unsigned char *header,  unsigned char *ie)
{
	Q931_info_t *qi = (Q931_info_t *)header;
	qi->keypad = ie - header - sizeof(Q931_info_t) ;

	return 0;
}

int MsgEncoderTE::encNotify(unsigned char *header,  unsigned char *ie)
{
	Q931_info_t *qi = (Q931_info_t *)header;
	qi->notify = ie - header - sizeof(Q931_info_t) ;

	return 0;
}

int MsgEncoderTE::encSignal(unsigned char *header,  unsigned char *ie)
{
	Q931_info_t *qi = (Q931_info_t *)header;
	qi->signal = ie - header - sizeof(Q931_info_t) ;

	return 0;
}

int MsgEncoderTE::encDisplay(unsigned char *header,  unsigned char *ie)
{
	Q931_info_t *qi = (Q931_info_t *)header;
	qi->display = ie - header - sizeof(Q931_info_t) ;

	return 0;
}

#if 0
int MsgEncoderTE::encConnectPN(unsigned char *header,  unsigned char *ie)
{
	Q931_info_t *qi = (Q931_info_t *)header;
	qi->connected_nr = ie - header - sizeof(Q931_info_t) ;

	return 0;
}

int MsgEncoderTE::encComplete(unsigned char *header,  unsigned char *ie)
{/* IE COMPLETE has no content, only IE_TYPE */
	Q931_info_t *qi = (Q931_info_t *)header;
	qi->sending_complete = ie - header - sizeof(Q931_info_t) ;

	return 0;
}
#endif


/* SETUP msg */
int MsgEncoderTE::encCallingPN4SetupMsg(unsigned char *header,  unsigned char *ie)
{
	return encCallingPN( header, ie);
}

int MsgEncoderTE::encCalledPN4SetupMsg(unsigned char *header,  unsigned char *ie)
{
	return encCalledPN(header, ie);
}


#ifdef CENTREX
int MsgEncoderTE::encCentrex4SetupMsg(unsigned char *header,  unsigned char *ie)
{
	return encFacility(header, ie);
}
#endif

int MsgEncoderTE::encUserUser4SetupMsg(unsigned char *header,  unsigned char *ie)
{
	return encUserUser(header, ie);
}

#if 0
int MsgEncoderTE::encComplete4SetupMsg(unsigned char *header,  unsigned char *ie)
{
	return getComplete( data);;
}
#endif

int MsgEncoderTE::encRedirNR4SetupMsg(unsigned char *header,  unsigned char *ie)
{
	return encRedirNR( header, ie);
}

int MsgEncoderTE::encBearer4SetupMsg(unsigned char *header,  unsigned char *ie)
{
	return encBearer(header, ie);
}

int MsgEncoderTE::encChanId4SetupMsg(unsigned char *header,  unsigned char *ie)
{
	return encChanId(header, ie);;
}

int MsgEncoderTE::encCallingSub4SetupMsg(unsigned char *header,  unsigned char *ie)
{
	return encCallingSub(header, ie);;
}

int MsgEncoderTE::encCalledSub4SetupMsg(unsigned char *header,  unsigned char *ie)
{
	return encCalledSub(header, ie);;
}

int MsgEncoderTE::encProgress4SetupMsg(unsigned char *header,  unsigned char *ie)
{
	return encProgress(header, ie);;
}

int MsgEncoderTE::encDisplay4SetupMsg(unsigned char *header,  unsigned char *ie)
{
	return encDisplay(header, ie);;
}

int MsgEncoderTE::encSignal4SetupMsg(unsigned char *header,  unsigned char *ie)
{
	return encSignal(header, ie);;
}

int MsgEncoderTE::encKeypad4SetupMsg(unsigned char *header,  unsigned char *ie)
{
	return encKeypad(header, ie);;
}

int MsgEncoderTE::encFacility4SetupMsg(unsigned char *header,  unsigned char *ie)
{
	return encFacility(header, ie);;
}




/* for INFO msg */
int MsgEncoderTE::encKeypad4InfoMsg(unsigned char *header,  unsigned char *ie)
{
	return encKeypad( header, ie);
}
int MsgEncoderTE::encCalledPN4InfoMsg(unsigned char *header,  unsigned char *ie)
{
	return encCalledPN(header, ie);;
}
int MsgEncoderTE::encDisplay4InfoMsg(unsigned char *header,  unsigned char *ie)
{
	return encDisplay( header, ie);
}
int MsgEncoderTE::encSignal4InfoMsg(unsigned char *header,  unsigned char *ie)
{
	return encSignal(header, ie);;
}

#if 0
const  unsigned char *MsgEncoderTE::getComplete4InfoMsg( unsigned char *data)
{
	return encComplete(header, ie);
}
#endif	

/* for SETUP_ACK msg */
int MsgEncoderTE::encChanId4SetupAckMsg(unsigned char *header,  unsigned char *ie)
{
	return encChanId(header, ie);
}

int MsgEncoderTE::encProgress4SetupAckMsg(unsigned char *header,  unsigned char *ie)
{
	return encProgress( header, ie);
}

/*************** PROCEEDING msg *************************/
int MsgEncoderTE::encChanId4ProceedingMsg(unsigned char *header,  unsigned char *ie)
{
	return encChanId(header, ie);
}
int MsgEncoderTE::encProgress4ProceedingMsg(unsigned char *header,  unsigned char *ie)
{
	return encProgress(header, ie);
}
int MsgEncoderTE::encDisplay4ProceedingMsg(unsigned char *header,  unsigned char *ie)
{
	return encProgress(header, ie);
}

/* 2006.06.14 */
int MsgEncoderTE::encFacility4ProceedingMsg(unsigned char *header,  unsigned char *ie)
{
	return 0;
}

/************************** ALERTING msg **********************/
int MsgEncoderTE::encChanId4AlertingMsg(unsigned char *header,  unsigned char *ie)
{
	return encChanId(header, ie);
}
int MsgEncoderTE::encProgress4AlertingMsg(unsigned char *header,  unsigned char *ie)
{
	return encProgress(header, ie);
}
int MsgEncoderTE::encDisplay4AlertingMsg(unsigned char *header,  unsigned char *ie)
{
	return encDisplay(header, ie);
}
int MsgEncoderTE::encSignal4AlertingMsg(unsigned char *header,  unsigned char *ie)
{
	return encSignal(header, ie);
}



/* CONNECTION msg */
int MsgEncoderTE::encChanId4ConnectMsg(unsigned char *header,  unsigned char *ie)
{
	return encChanId(header, ie);
}
int MsgEncoderTE::encProgress4ConnectMsg(unsigned char *header,  unsigned char *ie)
{
	return encProgress(header, ie);
}
int MsgEncoderTE::encSignal4ConnectMsg(unsigned char *header,  unsigned char *ie)
{
	return encSignal(header, ie);
}
int MsgEncoderTE::encDisplay4ConnectMsg(unsigned char *header,  unsigned char *ie)
{
	return encDisplay(header, ie);
}

#if 0
int MsgEncoderTE::encConnectPN4ConnectMsg(unsigned char *header,  unsigned char *ie)
{
	return encConnectPN;
}
#endif

#ifdef CENTREX
int MsgEncoderTE::encCentrex4ConnectMsg(unsigned char *header,  unsigned char *ie)
{
	return encFacility(header, ie);
}
#endif

/******************  CONNECT_ACK msg ******************/
int MsgEncoderTE::encSignal4ConnectAckMsg(unsigned char *header,  unsigned char *ie)
{
	return encSignal(header, ie);
}
int MsgEncoderTE::encDisplay4ConnectAckMsg(unsigned char *header,  unsigned char *ie)
{
	return encDisplay(header, ie);
}


/* DIS_CONNECTION msg */
int MsgEncoderTE::encProgress4DisConnMsg(unsigned char *header,  unsigned char *ie)
{
	return encProgress(header, ie);
}
int MsgEncoderTE::encCause4DisConnMsg(unsigned char *header,  unsigned char *ie)
{
	return encCause(header, ie);
}
int MsgEncoderTE::encSignal4DisConnMsg(unsigned char *header,  unsigned char *ie)
{
	return encSignal(header, ie);
}
int MsgEncoderTE::encDisplay4DisConnMsg(unsigned char *header,  unsigned char *ie)
{
	return encDisplay(header, ie);
}

int MsgEncoderTE::encCharge4DisConnMsg(unsigned char *header,  unsigned char *ie)
{
	return 0;//encDisplay(header, ie);
}

/* NOTIFY msg */
int MsgEncoderTE::encRedirDN4NotifyMsg(unsigned char *header,  unsigned char *ie)
{
	cpLog(LOG_WARNING, "not used in TE mode");	
//#warning REINSERT redir_dn, when included in te-mode
//		if (qi->redir_dn)
//			p = (const  char *)qi + sizeof(Q931_info_t) + qi->redir_dn + 1;
	return -1;
}

int MsgEncoderTE::encNotify4NotifyMsg(unsigned char *header,  unsigned char *ie)
{
	return encNotify( header, ie);
}
int MsgEncoderTE::encDisplay4NotifyMsg(unsigned char *header,  unsigned char *ie)
{
	return encDisplay( header, ie);
}


/*********************** RELEASE msg ***********************/
int MsgEncoderTE::encCause4ReleaseMsg(unsigned char *header,  unsigned char *ie)
{
	return encCause(header, ie);
}
int MsgEncoderTE::encDisplay4ReleaseMsg(unsigned char *header,  unsigned char *ie)
{
	return encDisplay(header, ie);
}
int MsgEncoderTE::encSignal4ReleaseMsg(unsigned char *header,  unsigned char *ie)
{
	return encSignal(header, ie);
}


/* RELEASE_COMPLETE msg */
int MsgEncoderTE::encCause4ReleaseCompleteMsg(unsigned char *header,  unsigned char *ie)
{
	return encCause(header, ie);
}
int MsgEncoderTE::encDisplay4ReleaseCompleteMsg(unsigned char *header,  unsigned char *ie)
{
	return encDisplay(header, ie);
}
int MsgEncoderTE::encSignal4ReleaseCompleteMsg(unsigned char *header,  unsigned char *ie)
{
	return encSignal(header, ie);
}

/******************** SUSPEND msg **********************/
int MsgEncoderTE::encCallId4SuspendMsg(unsigned char *header,  unsigned char *ie)
{
	return encCallId(header, ie);
}

/************************** RESUME msg *****************/
int MsgEncoderTE::encCallId4ResumeMsg(unsigned char *header,  unsigned char *ie)
{
	return encCallId(header, ie);
}

/* RETRIEVE msg */
int MsgEncoderTE::encChanId4RetrieveMsg(unsigned char *header,  unsigned char *ie)
{
	return encChanId(header, ie);
}

#if 0
int MsgEncoderTE::encCallId4SuspendMsg(unsigned char *header,  unsigned char *ie)
{
	return encCallId(header, ie);
}
#endif

#if 0
int MsgEncoderTE::encFacilityPoint4FacilityMsg(unsigned char *header,  unsigned char *ie)
{
	return encFacility(header, ie);
}
#endif
int MsgEncoderTE::encDisplay4FacilityMsg(unsigned char *header,  unsigned char *ie)
{
	return encDisplay(header, ie);
}

int MsgEncoderTE::checkTeIE(const unsigned char *ieData, int ieType)
{
	if(ieData[0]!=ieType)
	{
		return AS_FAIL;
	}

	return AS_OK;
}

