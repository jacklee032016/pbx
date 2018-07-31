/*
* $Id: MsgDecoder.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/
#ifndef __MSG_DECODER_HXX__
#define __MSG_DECODER_HXX__

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

class MsgDecoder : public MsgCodec 
{
	public:
		MsgDecoder( );
		
		virtual ~MsgDecoder();

		/* SETUP msg */
		virtual const unsigned char *getCallingPN4SetupMsg(const unsigned char *data) = 0;
		virtual const unsigned char *getCalledPN4SetupMsg(const unsigned char *data) = 0;
#ifdef CENTREX
		virtual const unsigned char *getCentrex4SetupMsg(const unsigned char *data) = 0;
#endif
		virtual const unsigned char *getUserUser4SetupMsg(const unsigned char *data) = 0;
#if 0
		virtual const unsigned char *getComplete4SetupMsg(const unsigned char *data) = 0;
#endif
		virtual const unsigned char *getRedirNR4SetupMsg(const unsigned char *data) = 0;
		virtual const unsigned char *getBearer4SetupMsg(const unsigned char *data) = 0;
		virtual const unsigned char *getChanId4SetupMsg(const unsigned char *data) = 0;
		

		/* INFO msg */
		virtual const unsigned char *getKeypad4InfoMsg(const unsigned char *data) = 0;
		virtual const unsigned char *getCalledPN4InfoMsg(const unsigned char *data) = 0;
		
#if 0
		virtual const unsigned char *getComplete4InfoMsg(const unsigned char *data) = 0;
#endif

		/* SETUP_ACK msg */
		virtual const unsigned char *getChanId4SetupAckMsg(const unsigned char *data) = 0;
		virtual const unsigned char *getProgress4SetupAckMsg(const unsigned char *data) = 0;

		/* PROCEEDING msg */
		virtual const unsigned char *getChanId4ProceedingMsg(const unsigned char *data) = 0;
		virtual const unsigned char *getProgress4ProceedingMsg(const unsigned char *data) = 0;

		/* ALERTING msg */
		virtual const unsigned char *getChanId4AlertingMsg(const unsigned char *data) = 0;
		virtual const unsigned char *getProgress4AlertingMsg(const unsigned char *data) = 0;

		/* CONNECTION msg */
		virtual const unsigned char *getChanId4ConnectMsg(const unsigned char *data) = 0;
#if 0		
		virtual const unsigned char *getConnectPN4ConnectMsg(const unsigned char *data) = 0;
#endif

#ifdef CENTREX
		virtual const unsigned char *getCentrex4ConnectMsg(const unsigned char *data) = 0;
#endif

		/* DIS_CONNECTION msg */
		virtual const unsigned char *getProgress4DisConnMsg(const unsigned char *data) = 0;
		virtual const unsigned char *getCause4DisConnMsg(const unsigned char *data) = 0;
		virtual const unsigned char *getCharge4DisConnMsg(const unsigned char *data) = 0;

		/* NOTIFY msg */
		virtual const unsigned char *getNotify4NotifyMsg(const unsigned char *data) = 0;
		virtual const unsigned char *getRedirDN4NotifyMsg(const unsigned char *data) = 0;
		virtual const unsigned char *getCause4ReleaseMsg(const unsigned char *data) = 0;

		/* RELEASE_COMPLETE msg */
		virtual const unsigned char *getCause4ReleaseCompleteMsg(const unsigned char *data) = 0;

		/* SUSPEND msg */
		virtual const unsigned char *getCallId4SuspendMsg(const unsigned char *data) = 0;
		
		/* RETRIEVE msg */
		virtual const unsigned char *getChanId4RetrieveMsg(const unsigned char *data) = 0;
		
#if 0
		/* FACILITY msg */
		virtual const unsigned char *getFacilityPoint4FacilityMsg(const unsigned char *data) = 0;
#endif
		/* return prim field in msg */
		virtual int getPrimType(const msg_t *msg) = 0;

		using MsgCodec::getType;
		ISDN_METHOD getType(const msg_t *msg);

	protected:

	private:

};

}
#endif

