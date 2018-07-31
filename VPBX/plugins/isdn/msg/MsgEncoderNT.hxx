/*
* $Id: MsgEncoderNT.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/
#ifndef __MSG_ENCODER_NT_HXX__
#define __MSG_ENCODER_NT_HXX__

#include "MsgEncoder.hxx"

namespace Assist
{

/* handle all Indication from ISDN, eg. from driver */
class MsgEncoderNT : public MsgEncoder
{
	public:
		MsgEncoderNT( );
		
		~MsgEncoderNT();

		/* SETUP msg */
		virtual int encCallingPN4SetupMsg(unsigned char *header,  unsigned char *ie);
		virtual int encCalledPN4SetupMsg(unsigned char *header,  unsigned char *ie);
#ifdef CENTREX
		virtual int encCentrex4SetupMsg(unsigned char *header,  unsigned char *ie);
#endif
		virtual int encUserUser4SetupMsg(unsigned char *header,  unsigned char *ie);
#if 0
		virtual int encComplete4SetupMsg(unsigned char *header,  unsigned char *ie);
#endif
		virtual int encRedirNR4SetupMsg(unsigned char *header,  unsigned char *ie) ;
		virtual int encBearer4SetupMsg(unsigned char *header,  unsigned char *ie);
		virtual int encChanId4SetupMsg(unsigned char *header,  unsigned char *ie);
		
		virtual int encCallingSub4SetupMsg(unsigned char *header,  unsigned char *ie);
		virtual int encCalledSub4SetupMsg(unsigned char *header,  unsigned char *ie);
		virtual int encProgress4SetupMsg(unsigned char *header,  unsigned char *ie);
		
		virtual int encDisplay4SetupMsg(unsigned char *header,  unsigned char *ie);
		virtual int encSignal4SetupMsg(unsigned char *header,  unsigned char *ie);
		virtual int encKeypad4SetupMsg(unsigned char *header,  unsigned char *ie);

		virtual int encFacility4SetupMsg(unsigned char *header,  unsigned char *ie);

		/* INFO msg */
		virtual int encKeypad4InfoMsg(unsigned char *header,  unsigned char *ie);
		virtual int encCalledPN4InfoMsg(unsigned char *header,  unsigned char *ie);
		virtual int encSignal4InfoMsg(unsigned char *header,  unsigned char *ie);
		virtual int encDisplay4InfoMsg(unsigned char *header,  unsigned char *ie);
		
#if 0
		virtual int encComplete4InfoMsg(unsigned char *header,  unsigned char *ie);
#endif

		/* SETUP_ACK msg */
		virtual int encChanId4SetupAckMsg(unsigned char *header,  unsigned char *ie);
		virtual int encProgress4SetupAckMsg(unsigned char *header,  unsigned char *ie);

		/* PROCEEDING msg */
		virtual int encChanId4ProceedingMsg(unsigned char *header,  unsigned char *ie);
		virtual int encProgress4ProceedingMsg(unsigned char *header,  unsigned char *ie);
		virtual int encDisplay4ProceedingMsg(unsigned char *header,  unsigned char *ie);
		/* 2006.06.14 */
		virtual int encFacility4ProceedingMsg(unsigned char *header,  unsigned char *ie);

		/* ALERTING msg */
		virtual int encChanId4AlertingMsg(unsigned char *header,  unsigned char *ie);
		virtual int encProgress4AlertingMsg(unsigned char *header,  unsigned char *ie);
		virtual int encDisplay4AlertingMsg(unsigned char *header,  unsigned char *ie);
		virtual int encSignal4AlertingMsg(unsigned char *header,  unsigned char *ie);

		/* CONNECTION msg */
		virtual int encChanId4ConnectMsg(unsigned char *header,  unsigned char *ie);
#if 0		
		virtual int encConnectPN4ConnectMsg(unsigned char *header,  unsigned char *ie);
#endif

#ifdef CENTREX
		virtual int encCentrex4ConnectMsg(unsigned char *header,  unsigned char *ie);
#endif
		virtual int encProgress4ConnectMsg(unsigned char *header,  unsigned char *ie);
		virtual int encDisplay4ConnectMsg(unsigned char *header,  unsigned char *ie);
		virtual int encSignal4ConnectMsg(unsigned char *header,  unsigned char *ie);

		/**********  CONNECT_ACK msg ******************/
		virtual int encDisplay4ConnectAckMsg(unsigned char *header,  unsigned char *ie);
		virtual int encSignal4ConnectAckMsg(unsigned char *header,  unsigned char *ie);

		/* DIS_CONNECTION msg */
		virtual int encProgress4DisConnMsg(unsigned char *header,  unsigned char *ie);
		virtual int encCause4DisConnMsg(unsigned char *header,  unsigned char *ie);
		virtual int encDisplay4DisConnMsg(unsigned char *header,  unsigned char *ie);
		virtual int encSignal4DisConnMsg(unsigned char *header,  unsigned char *ie);


		virtual int encCharge4DisConnMsg(unsigned char *header,  unsigned char *ie);

		/* NOTIFY msg */
		virtual int encNotify4NotifyMsg(unsigned char *header,  unsigned char *ie);
		virtual int encRedirDN4NotifyMsg(unsigned char *header,  unsigned char *ie);
		virtual int encDisplay4NotifyMsg(unsigned char *header,  unsigned char *ie);

		/* RELEASE msg */
		virtual int encCause4ReleaseMsg(unsigned char *header,  unsigned char *ie);
		virtual int encDisplay4ReleaseMsg(unsigned char *header,  unsigned char *ie);
		virtual int encSignal4ReleaseMsg(unsigned char *header,  unsigned char *ie);

		/* RELEASE_COMPLETE msg */
		virtual int encCause4ReleaseCompleteMsg(unsigned char *header,  unsigned char *ie);
		virtual int encDisplay4ReleaseCompleteMsg(unsigned char *header,  unsigned char *ie);
		virtual int encSignal4ReleaseCompleteMsg(unsigned char *header,  unsigned char *ie);

		/* SUSPEND msg */
		virtual int encCallId4SuspendMsg(unsigned char *header,  unsigned char *ie);

		/***************** RESUME msg ****************/
		virtual int encCallId4ResumeMsg(unsigned char *header,  unsigned char *ie);
		
		/* RETRIEVE msg */
		virtual int encChanId4RetrieveMsg(unsigned char *header,  unsigned char *ie);
		
#if 0
		/* FACILITY msg */
		virtual int encFacilityPoint4FacilityMsg(unsigned char *header,  unsigned char *ie);
#endif
		virtual int encDisplay4FacilityMsg(unsigned char *header,  unsigned char *ie);
	protected:

	private:
		
};

}
#endif

