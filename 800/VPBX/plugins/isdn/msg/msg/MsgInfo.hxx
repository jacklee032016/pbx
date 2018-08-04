#ifndef __MSG_INFO_HXX__
#define __MSG_INFO_HXX__
/*
* $Id: MsgInfo.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "IsdnMsg.hxx"

namespace Assist
{

class IeCalledPN;
class IeKeypad;
//class IeComplete;

/* CC_INFORMATION INDICATION */
class MsgInfo : public IsdnMsg //Command
{
	public:
		MsgInfo(const msg_t *msg, IsdnCtrlDevice *ctrl) throw(IsdnMsgParserException &);
		MsgInfo(IsdnCtrlDevice *ctrl);
		MsgInfo(const MsgInfo&);
		
		~MsgInfo();

		MsgInfo& operator =(const MsgInfo&);
		bool operator ==(const MsgInfo&);

		ISDN_METHOD getType() const;
		int getMsgType() const ;
		
		void setDisplay(Sptr <IeDisplay> _display);
		Sptr <IeDisplay>  getDisplay();

		void setKeypad(Sptr <IeKeypad> keypad);
		Sptr <IeKeypad> getKeypad();

		void setSignal(Sptr <IeSignal> _signal);
		Sptr <IeSignal> getSignal();

#if 0
		void setCalledPN(Sptr <IeCalledPN> _calledPn);
		Sptr <IeCalledPN> getCalledPN();

		void setComplete(Sptr <IeComplete> _calledPn);
		Sptr <IeComplete> getComplete();
#endif

#if WITH_DEBUG
		void debugInfo();
#endif

		int getCCType() const ;
		
		virtual const char *getName();

	protected:
		
		virtual void encode( msg_t *msg) ;
		int decode(const unsigned char  *iesData)  throw(IsdnMsgParserException &) ;

	private:
		/* No Mandatory IE in the INFOMATION msg */

		/* optional and only n-->u */
		Sptr <IeDisplay>				ieDisplay;
		/* optional and only u-->n */
		Sptr <IeKeypad> 				ieKeypad;

		/* optional and only n-->u */
		Sptr <IeSignal>				ieSignal;
#if 0
		/* following IEs are not offered in INS for this msg */
		Sptr <IeCalledPN> 	ieCalledPN;
		Sptr <IeComplete> 	ieComplete; 
#endif

		/* other IEs are Locking shift, etc. will be defined future */
};
 
}

#endif

