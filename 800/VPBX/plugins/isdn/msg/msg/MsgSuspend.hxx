#ifndef __MSG_SUSPEND_HXX__
#define __MSG_SUSPEND_HXX__
/*
* $Id: MsgSuspend.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "IsdnMsg.hxx"

namespace Assist
{

class IeCallId;

/* CC_SUSPEND INDICATION */
class MsgSuspend : public IsdnMsg
{
	public:
		MsgSuspend(const msg_t *msg, IsdnCtrlDevice *ctrl)  throw(IsdnMsgParserException &);
		MsgSuspend(IsdnCtrlDevice *ctrl );
		MsgSuspend(const MsgSuspend&);
//		MsgSuspend(Sptr <IeCallState> _callState);
		virtual ~MsgSuspend();

		bool operator ==(const MsgSuspend& src);  
		MsgSuspend& operator =(const MsgSuspend&);

		ISDN_METHOD getType() const;
		int getMsgType() const ;

		Sptr <IeCallId>  getCallId() const;
		void setCallId(Sptr <IeCallId> callId);

#if WITH_DEBUG
		void debugInfo();
#endif

		int getCCType() const ;
		
		virtual const char *getName();

	protected:
	
		virtual void encode( msg_t *msg) ;
		int decode(const unsigned char  *iesData)  throw(IsdnMsgParserException &) ;

	private:
		/* only  one IE, that is optional ,length 2-10, u-->n */
		Sptr <IeCallId> 	ieCallId;
};
 
}

#endif

