#ifndef __MSG_HOLD_HXX__
#define __MSG_HOLD_HXX__

#include "IsdnMsg.hxx"
/*
* $Id: MsgHold.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/
namespace Assist
{

/* CC_HOLD INDICATION */
/* no info is keep in this Msg */
class MsgHold : public IsdnMsg//Command
{
	public:
		MsgHold(const msg_t *msg, IsdnCtrlDevice *ctrl)   throw(IsdnMsgParserException &);
		MsgHold(IsdnCtrlDevice *ctrl);
		MsgHold(const MsgHold&);
		~MsgHold();

		bool operator ==(const MsgHold& src);  
		MsgHold& operator =(const MsgHold&);

		ISDN_METHOD getType() const;
		int getMsgType() const ;
		
		void setDisplay(Sptr <IeDisplay> _display);

#if WITH_DEBUG
		void debugInfo();
#endif

		int getCCType() const ;
		
		virtual const char *getName();

	protected:
	
		virtual void encode( msg_t *msg) ;
		int decode(const unsigned char *iesData)  throw(IsdnMsgParserException &) ;

	private:
		
};

}
#endif

