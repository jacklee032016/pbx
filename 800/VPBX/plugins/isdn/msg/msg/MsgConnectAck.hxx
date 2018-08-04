#ifndef __MSG_CONNECT_ACK_HXX__
#define __MSG_CONNECT_ACK_HXX__

#include "IsdnMsg.hxx"
/*
* $Id: MsgConnectAck.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

namespace Assist
{

class IeChanId;
class IeDisplay;
class IeSignal;

/* CC_CONNECT_ACK INDICATION */
class MsgConnectAck : public IsdnMsg//Command
{
	public:
		MsgConnectAck(const msg_t *msg, IsdnCtrlDevice *ctrl)   throw(IsdnMsgParserException &) ;
		MsgConnectAck(IsdnCtrlDevice *ctrl);
		MsgConnectAck(const MsgConnectAck& src);

		virtual ~MsgConnectAck();

		MsgConnectAck& operator=(const MsgConnectAck&);
		bool operator ==(const MsgConnectAck& src); 

		virtual ISDN_METHOD getType() const;
		int getMsgType() const ;

		void setDisplay(Sptr <IeDisplay> display);
		Sptr <IeDisplay> getDisplay();
		
		void setSignal(Sptr <IeSignal> display);
		Sptr <IeSignal> getSignal();
		

#if WITH_DEBUG
		void debugInfo();
#endif

		int getCCType() const ;
		
		virtual const char *getName();

	protected:
	
		virtual void encode( msg_t *msg) ;
		int decode(const unsigned char *iesData)  throw(IsdnMsgParserException &) ;
		
	private:

		/* No mandatory IEs in the CONNECT_ACK , p.18 */
		/* optional IE, n-->u */
		Sptr <IeDisplay>		ieDisplay;

		/* optional IE, n-->u */
		Sptr <IeSignal>		ieSignal;

};
 
}
 
#endif

