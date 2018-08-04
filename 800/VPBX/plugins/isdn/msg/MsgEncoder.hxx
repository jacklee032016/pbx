/*
* $Id: MsgEncoder.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/
#ifndef __MSG_ENCODER_HXX__
#define __MSG_ENCODER_HXX__

#include "global.h"
#include "vpbx_globals.h"

#include "Data.hxx"
#include "Sptr.hxx"
#include "VException.hxx"

#include "IsdnIE.hxx"
#include "IsdnMsg.hxx"

#include "as_msg.h"
#include "ask_isdn.h"

#include "MsgCodec.hxx"

namespace Assist
{

class MsgEncoder : public MsgCodec 
{
	public:
		MsgEncoder( );
		
		virtual ~MsgEncoder();

		/* SETUP msg */
		virtual int encCallingPN4SetupMsg(unsigned char *header,  unsigned char *ie) = 0;
		virtual int encCalledPN4SetupMsg(unsigned char *header,  unsigned char *ie) = 0;
#ifdef CENTREX
		virtual int encCentrex4SetupMsg(unsigned char *header,  unsigned char *ie) = 0;
#endif
		virtual int encUserUser4SetupMsg(unsigned char *header,  unsigned char *ie) = 0;
#if 0
		virtual int encComplete4SetupMsg(unsigned char *header,  unsigned char *ie) = 0;
#endif
		virtual int encRedirNR4SetupMsg(unsigned char *header,  unsigned char *ie) = 0;
		virtual int encBearer4SetupMsg(unsigned char *header,  unsigned char *ie) = 0;
		virtual int encChanId4SetupMsg(unsigned char *header,  unsigned char *ie) = 0;

		virtual int encCallingSub4SetupMsg(unsigned char *header,  unsigned char *ie) = 0;
		virtual int encCalledSub4SetupMsg(unsigned char *header,  unsigned char *ie) = 0;
		virtual int encProgress4SetupMsg(unsigned char *header,  unsigned char *ie) = 0;
		
		virtual int encDisplay4SetupMsg(unsigned char *header,  unsigned char *ie) = 0;
		virtual int encSignal4SetupMsg(unsigned char *header,  unsigned char *ie) = 0;
		virtual int encKeypad4SetupMsg(unsigned char *header,  unsigned char *ie) = 0;

		virtual int encFacility4SetupMsg(unsigned char *header,  unsigned char *ie) = 0;

		/* INFO msg */
		virtual int encKeypad4InfoMsg(unsigned char *header,  unsigned char *ie) = 0;
		virtual int encCalledPN4InfoMsg(unsigned char *header,  unsigned char *ie) = 0;
		virtual int encSignal4InfoMsg(unsigned char *header,  unsigned char *ie) = 0;
		virtual int encDisplay4InfoMsg(unsigned char *header,  unsigned char *ie) = 0;
		
#if 0
		virtual int encComplete4InfoMsg(unsigned char *header,  unsigned char *ie) = 0;
#endif

		/* SETUP_ACK msg */
		virtual int encChanId4SetupAckMsg(unsigned char *header,  unsigned char *ie) = 0;
		virtual int encProgress4SetupAckMsg(unsigned char *header,  unsigned char *ie) = 0;

		/* PROCEEDING msg */
		virtual int encChanId4ProceedingMsg(unsigned char *header,  unsigned char *ie) = 0;
		virtual int encProgress4ProceedingMsg(unsigned char *header,  unsigned char *ie) = 0;
		virtual int encDisplay4ProceedingMsg(unsigned char *header,  unsigned char *ie) = 0;
		/* 2006.06.14 */
		virtual int encFacility4ProceedingMsg(unsigned char *header,  unsigned char *ie) = 0;

		/* ALERTING msg */
		virtual int encChanId4AlertingMsg(unsigned char *header,  unsigned char *ie) = 0;
		virtual int encProgress4AlertingMsg(unsigned char *header,  unsigned char *ie) = 0;
		virtual int encDisplay4AlertingMsg(unsigned char *header,  unsigned char *ie) = 0;
		virtual int encSignal4AlertingMsg(unsigned char *header,  unsigned char *ie) = 0;

		/* CONNECTION msg */
		virtual int encChanId4ConnectMsg(unsigned char *header,  unsigned char *ie) = 0;
#if 0		
		virtual int encConnectPN4ConnectMsg(unsigned char *header,  unsigned char *ie) = 0;
#endif

#ifdef CENTREX
		virtual int encCentrex4ConnectMsg(unsigned char *header,  unsigned char *ie) = 0;
#endif
		virtual int encProgress4ConnectMsg(unsigned char *header,  unsigned char *ie) = 0;
		virtual int encDisplay4ConnectMsg(unsigned char *header,  unsigned char *ie) = 0;
		virtual int encSignal4ConnectMsg(unsigned char *header,  unsigned char *ie) = 0;


		/**********  CONNECT_ACK msg ******************/
		virtual int encDisplay4ConnectAckMsg(unsigned char *header,  unsigned char *ie) = 0;
		virtual int encSignal4ConnectAckMsg(unsigned char *header,  unsigned char *ie) = 0;


		/* DIS_CONNECTION msg */
		virtual int encProgress4DisConnMsg(unsigned char *header,  unsigned char *ie) = 0;
		virtual int encCause4DisConnMsg(unsigned char *header,  unsigned char *ie) = 0;
		virtual int encDisplay4DisConnMsg(unsigned char *header,  unsigned char *ie) = 0;
		virtual int encSignal4DisConnMsg(unsigned char *header,  unsigned char *ie) = 0;

		virtual int encCharge4DisConnMsg(unsigned char *header,  unsigned char *ie) = 0;

		/* NOTIFY msg */
		virtual int encNotify4NotifyMsg(unsigned char *header,  unsigned char *ie) = 0;
		virtual int encRedirDN4NotifyMsg(unsigned char *header,  unsigned char *ie) = 0;
		virtual int encDisplay4NotifyMsg(unsigned char *header,  unsigned char *ie) = 0;

		/**** RELEASE msg */
		virtual int encCause4ReleaseMsg(unsigned char *header,  unsigned char *ie) = 0;
		virtual int encDisplay4ReleaseMsg(unsigned char *header,  unsigned char *ie) = 0;
		virtual int encSignal4ReleaseMsg(unsigned char *header,  unsigned char *ie) = 0;

		/* RELEASE_COMPLETE msg */
		virtual int encCause4ReleaseCompleteMsg(unsigned char *header,  unsigned char *ie) = 0;
		virtual int encDisplay4ReleaseCompleteMsg(unsigned char *header,  unsigned char *ie) = 0;
		virtual int encSignal4ReleaseCompleteMsg(unsigned char *header,  unsigned char *ie) = 0;

		/* SUSPEND msg */
		virtual int encCallId4SuspendMsg(unsigned char *header,  unsigned char *ie) = 0;
		
		/***************** RESUME msg ****************/
		virtual int encCallId4ResumeMsg(unsigned char *header,  unsigned char *ie) = 0;

		/* RETRIEVE msg */
		virtual int encChanId4RetrieveMsg(unsigned char *header,  unsigned char *ie) = 0;
		
#if 0
		/* FACILITY msg */
		virtual int encFacilityPoint4FacilityMsg(unsigned char *header,  unsigned char *ie) = 0;
#endif
		virtual int encDisplay4FacilityMsg(unsigned char *header,  unsigned char *ie) = 0;
		
	protected:

	private:

};

}
#endif

