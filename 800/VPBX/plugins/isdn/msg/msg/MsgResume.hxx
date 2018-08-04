#ifndef __MSG_RESUME_HXX__
#define __MSG_RESUME_HXX__
/*
* $Id: MsgResume.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "IsdnMsg.hxx"

namespace Assist
{

class IeCallId;

class MsgResume : public IsdnMsg
{
	public:
		MsgResume(const msg_t *msg, IsdnCtrlDevice *ctrl)  throw(IsdnMsgParserException &) ;
		MsgResume(IsdnCtrlDevice *ctrl);
		MsgResume(const MsgResume&);

		~MsgResume();

		MsgResume& operator =(const MsgResume&);
		bool  operator ==(const MsgResume& src); 

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
		int decode(const unsigned char *iesdata)  throw(IsdnMsgParserException &) ;

    private:
		/* only one IE in the RESUME , p.26 */
		/* optional, u-->n , length 2-10 */
		Sptr <IeCallId> 				ieCallId;
		
};
 
}

#endif

