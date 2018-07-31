/*
* $Id: MsgDecoderTE.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/
#ifndef __MSG_DECODER_TE_HXX__
#define __MSG_DECODER_TE_HXX__

#include "MsgDecoder.hxx"

namespace Assist
{

class MsgDecoderTE : public MsgDecoder
{
	public:
		MsgDecoderTE( );
		
		~MsgDecoderTE();

		virtual int getPrimType(const msg_t *msg) ;
		/* SETUP msg */
		virtual const unsigned char *getCallingPN4SetupMsg(const unsigned char *data) ;
		virtual const unsigned char *getCalledPN4SetupMsg(const unsigned char *data);
#ifdef CENTREX
		virtual const unsigned char *getCentrex4SetupMsg(const unsigned char *data) ;
#endif
		virtual const unsigned char *getUserUser4SetupMsg(const unsigned char *data) ;
#if 0
		virtual const unsigned char *getComplete4SetupMsg(const unsigned char *data) ;
#endif
		virtual const unsigned char *getRedirNR4SetupMsg(const unsigned char *data) ;
		virtual const unsigned char *getBearer4SetupMsg(const unsigned char *data) ;
		virtual const unsigned char *getChanId4SetupMsg(const unsigned char *data) ;

		/* INFO msg */
		virtual const unsigned char *getKeypad4InfoMsg(const unsigned char *data) ;
		virtual const unsigned char *getCalledPN4InfoMsg(const unsigned char *data) ;
#if 0
		virtual const unsigned char *getComplete4InfoMsg(const unsigned char *data);
#endif
	

		virtual const unsigned char *getChanId4SetupAckMsg(const unsigned char *data);
		virtual const unsigned char *getProgress4SetupAckMsg(const unsigned char *data) ;
		
		virtual const unsigned char *getChanId4ProceedingMsg(const unsigned char *data);
		virtual const unsigned char *getProgress4ProceedingMsg(const unsigned char *data) ;
		
		virtual const unsigned char *getChanId4AlertingMsg(const unsigned char *data);
		virtual const unsigned char *getProgress4AlertingMsg(const unsigned char *data) ;


		/* CONNECTION msg */
		virtual const unsigned char *getChanId4ConnectMsg(const unsigned char *data) ;
#if 0		
		virtual const unsigned char *getConnectPN4ConnectMsg(const unsigned char *data) ;
#endif

#ifdef CENTREX
		virtual const unsigned char *getCentrex4ConnectMsg(const unsigned char *data) ;
#endif

		virtual const unsigned char *getProgress4DisConnMsg(const unsigned char *data) ;
		virtual const unsigned char *getCause4DisConnMsg(const unsigned char *data) ;
		virtual const unsigned char *getCharge4DisConnMsg(const unsigned char *data) ;


		virtual const unsigned char *getNotify4NotifyMsg(const unsigned char *data) ;
		virtual const unsigned char *getRedirDN4NotifyMsg(const unsigned char *data) ;
		
		virtual const unsigned char *getCause4ReleaseMsg(const unsigned char *data) ;

		virtual const unsigned char *getCause4ReleaseCompleteMsg(const unsigned char *data) ;

		virtual const unsigned char *getCallId4SuspendMsg(const unsigned char *data) ;
		
		virtual const unsigned char *getChanId4RetrieveMsg(const unsigned char *data) ;
		
		static Q931_info_t *getQ931Info(const  iframe_t *frm);
#if 0
		virtual const char *getFacilityPoint4FacilityMsg(const iframe_t *frm) ;
#endif

	protected:
		
	private:
		
		
		const  unsigned char *getChanId(const unsigned char *data);

		const  unsigned char *getCallId(const unsigned char *data);

		const  unsigned char *getProgress(const unsigned char *data);
		
		const  unsigned char *getFacility(const unsigned char *data) ;
		const  unsigned char *getCalledPN(const unsigned char *data) ;
#if 0
		const  unsigned char *getComplete(const unsigned char *data) ;
#endif
		const unsigned char *getCause(const unsigned char *data);

		const unsigned char *checkTeIE(const unsigned char *ieData, int ieType);

};

}
#endif

